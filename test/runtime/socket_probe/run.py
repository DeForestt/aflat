#!/usr/bin/env python3
"""Build and exercise the shim-free AFlat TCP probe (Linux x86-64).

Run from any directory: python3 test/runtime/socket_probe/run.py [--valgrind]
Requires permission to use local sockets, gcc, and a built bin/aflat.
"""
import argparse
import concurrent.futures
from pathlib import Path
import select
import socket
import struct
import subprocess
import tempfile
import time


HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]


def run(command, **kwargs):
    subprocess.run([str(arg) for arg in command], check=True, timeout=90, **kwargs)


def exercise(executable, valgrind, directory):
    # Reserve a free loopback port until immediately before starting the probe.
    reservation = socket.socket()
    reservation.bind(("127.0.0.1", 0))
    port = reservation.getsockname()[1]
    command = [str(executable)]
    if valgrind:
        command = ["valgrind", "--error-exitcode=99", "--leak-check=full",
                   "--errors-for-leak-kinds=definite,indirect", *command]
    log = directory / "server.log"
    repeated = 200
    queued = 32
    # Five regular exchanges, four disconnects, repeated and queued clients.
    count = 5 + 4 + repeated + queued
    reservation.close()
    with log.open("w+") as errors:
        server = subprocess.Popen(command, stdin=subprocess.PIPE,
                                  stdout=subprocess.PIPE, stderr=errors)
        try:
            server.stdin.write(struct.pack("!H", port) + struct.pack("<i", count))
            server.stdin.close()
            assert select.select([server.stdout], [], [], 15)[0], "startup timeout"
            ready = server.stdout.readline()
            assert ready == b"ready\n", ("startup failed", ready, server.poll())
            baseline_fds = len(list(Path(f"/proc/{server.pid}/fd").iterdir()))

            def connect():
                client = socket.create_connection(("127.0.0.1", port), timeout=15)
                client.settimeout(20)
                return client

            def exchange(payload, fragment=False, slow=False):
                with connect() as client:
                    header = struct.pack("<i", len(payload))
                    if fragment:
                        for byte in header:
                            client.sendall(bytes([byte]))
                            time.sleep(0.002)
                        for offset in range(0, len(payload), 37):
                            client.sendall(payload[offset:offset + 37])
                            time.sleep(0.0001)
                    else:
                        client.sendall(header + payload)
                    client.shutdown(socket.SHUT_WR)
                    if slow:
                        time.sleep(0.15)  # Force send queue pressure / EAGAIN.
                    received = bytearray()
                    while chunk := client.recv(65536):
                        received.extend(chunk)
                    assert received == payload, (len(received), len(payload))

            exchange(b"")
            exchange(b"hello TCP")
            exchange(bytes(range(256)) * 33, fragment=True)
            exchange(bytes(range(256)) * 32768, slow=True)  # 8 MiB, binary
            exchange(b"after large transfer")
            print("AFlat TCP: empty, small, fragmented, binary, 8 MiB, slow reader passed", flush=True)

            # EOF before header, mid-header, and reset in a partial body.
            with connect():
                pass
            with connect() as client:
                client.sendall(b"\x00\x10")
            with connect() as client:
                client.sendall(struct.pack("<i", 1048576) + b"partial body")
                client.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER,
                                  struct.pack("ii", 1, 0))
            with connect() as client:
                client.sendall(struct.pack("<i", 2097152) + b"x" * 2097152)
                time.sleep(0.1)  # Reset while the server is sending its reply.
                client.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER,
                                  struct.pack("ii", 1, 0))
            for index in range(repeated):
                exchange(struct.pack("<i", index) + bytes(range(256)) * 8)
            # Await an idle server before checking fd cleanup; the last group
            # below keeps it alive while we inspect /proc.
            assert len(list(Path(f"/proc/{server.pid}/fd").iterdir())) == baseline_fds
            with concurrent.futures.ThreadPoolExecutor(max_workers=queued) as pool:
                list(pool.map(lambda i: exchange(bytes([i]) * 16384), range(queued)))
            assert server.wait(timeout=20) == 0, "server failed"
            assert server.stdout.read() == b"passed\n", "missing completion"
            print(f"AFlat TCP: disconnect recovery, {repeated} repeated and {queued} queued clients; fd, allocation, and buffer guards passed", flush=True)
        finally:
            if server.poll() is None:
                server.kill()
            server.wait()
            errors.seek(0)
            diagnostics = errors.read()
            if diagnostics:
                print(diagnostics, end="", flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--valgrind", action="store_true")
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="aflat-socket-probe-") as temporary:
        directory = Path(temporary)
        abi = directory / "abi"
        run(["gcc", "-O2", "-Wall", "-Wextra", HERE / "abi.c",
             ROOT / "libraries/std/asm.s", "-o", abi])
        run([abi])
        assembly = directory / "main.s"
        executable = directory / "server"
        run([ROOT / "bin/aflat", HERE / "main.af", "-o", assembly], cwd=ROOT)
        # Deliberately exclude request.s, HTTP.s, and the async runtime.
        run(["gcc", "-g", "-no-pie", assembly, ROOT / "libraries/std/asm.s",
             ROOT / "libraries/std/allocator.s", "-pthread", "-o", executable])
        exercise(executable, args.valgrind, directory)


if __name__ == "__main__":
    main()
