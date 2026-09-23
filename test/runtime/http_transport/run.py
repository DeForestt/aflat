#!/usr/bin/env python3
"""Build isolated Aflat projects and validate the migrated HTTP server."""
import argparse
import concurrent.futures
from contextlib import contextmanager
from pathlib import Path
import shutil
import socket
import struct
import subprocess
import tempfile
import time

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]


def build(source, directory):
    (directory / 'src').mkdir(parents=True, exist_ok=True)
    (directory / 'bin').mkdir(exist_ok=True)
    shutil.copyfile(HERE / source, directory / 'src/main.af')
    (directory / 'aflat.cfg').write_text('[build]\nmain = main\noutput = ./bin/server\ndebug = true\n')
    result = subprocess.run([str(ROOT / 'bin/aflat'), '--no-cache', 'build'], cwd=directory,
                            capture_output=True, text=True, timeout=240)
    (directory / 'build.log').write_text(result.stdout + result.stderr)
    assert result.returncode == 0, result.stdout + result.stderr
    return directory / 'bin/server'


@contextmanager
def server(executable, settings, valgrind=False):
    with socket.socket() as reservation:
        reservation.bind(('127.0.0.1', 0))
        port = reservation.getsockname()[1]
    command = [str(executable)]
    if valgrind:
        command = ['valgrind', '--error-exitcode=99', '--leak-check=full',
                   '--errors-for-leak-kinds=definite,indirect', *command]
    with tempfile.TemporaryFile(mode='w+') as errors:
        process = subprocess.Popen(command, stdin=subprocess.PIPE,
                                   stdout=subprocess.PIPE, stderr=errors)
        process.stdin.write(struct.pack('<' + 'i' * (len(settings) + 1), port, *settings))
        process.stdin.close()

        def connect():
            end = time.monotonic() + 15
            while True:
                try:
                    peer = socket.create_connection(('127.0.0.1', port), timeout=10)
                    peer.settimeout(15)
                    return peer
                except ConnectionRefusedError:
                    assert process.poll() is None, f'server exited: {process.returncode}'
                    if time.monotonic() >= end:
                        raise
                    time.sleep(.01)
        try:
            yield process, connect
        finally:
            if process.poll() is None:
                process.terminate()
                try:
                    process.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    process.kill()
            process.wait()
            errors.seek(0)
            diagnostics = errors.read()
            if diagnostics:
                print(diagnostics, end='', flush=True)
            if valgrind:
                assert 'ERROR SUMMARY: 0 errors' in diagnostics, 'Valgrind reported errors or did not finish'


def request(body=b'', path=b'/echo'):
    return b'POST ' + path + b' HTTP/1.1\r\nHost: localhost\r\nContent-Length: ' + str(len(body)).encode() + b'\r\n\r\n' + body


def response(peer):
    data = bytearray()
    while True:
        try:
            chunk = peer.recv(65536)
        except ConnectionResetError:
            # Closing a rejected request with unread input can reset TCP after
            # its HTTP error response has already been delivered.
            assert data, 'reset without a response'
            break
        if not chunk:
            break
        data.extend(chunk)
    if not data:
        return 0, b'', b''
    headers, body = bytes(data).split(b'\r\n\r\n', 1)
    return int(headers.split(b' ', 2)[1]), headers, body


def exchange(connect, data, expected=200, body=None, fragments=False, delay=0, halfclose=True):
    with connect() as peer:
        if fragments:
            for offset in range(0, len(data), 31):
                peer.sendall(data[offset:offset + 31])
                time.sleep(.0001)
        else:
            peer.sendall(data)
        if halfclose:
            peer.shutdown(socket.SHUT_WR)
        if delay:
            time.sleep(delay)
        status, headers, actual = response(peer)
        assert status == expected, (status, expected, headers, data[:100])
        if body is not None:
            assert actual == body, (len(actual), len(body), actual[:80], body[:80])
        return headers, actual


def transport_checks(executable, valgrind):
    get = b'GET / HTTP/1.1\r\nHost: localhost\r\n\r\n'
    fragmented = request(b'fragmented\r\n' * 500)
    large = request(b'0123456789abcdef' * 131072)  # 2 MiB
    cases = [
        (get, 200, get),
        (request(), 200, request()),
        (b'GET / HTTP/1.0\r\n\r\n', 200, b'GET / HTTP/1.0\r\n\r\n'),
        (get + get, 200, get),
        (b'GET / HTTP/1.1\r\nX-Content-Length: 9000\r\n\r\n', 200, b'GET / HTTP/1.1\r\nX-Content-Length: 9000\r\n\r\n'),
        (b'POST / HTTP/1.1\r\ncOnTeNt-LeNgTh:\t3 \t\r\n\r\nabc', 200, b'POST / HTTP/1.1\r\ncOnTeNt-LeNgTh:\t3 \t\r\n\r\nabc'),
        (b'POST / HTTP/1.1\r\nContent-Length: 1\r\nContent-Length: 1\r\n\r\na', 400, b''),
        (b'POST / HTTP/1.1\r\nContent-Length: -1\r\n\r\n', 400, b''),
        (b'POST / HTTP/1.1\r\nContent-Length: 1x\r\n\r\n', 400, b''),
        (b'POST / HTTP/1.1\r\nContent-Length: 999999999999999999999\r\n\r\n', 413, b''),
        (b'POST / HTTP/1.1\r\nContent-Length: 8388608\r\n\r\n', 413, b''),
        (b'POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\n\r\n', 501, b''),
        (b'POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Length: 0\r\n\r\n', 400, b''),
        (b'GET / HTTP/1.1\r\nInvalid Header\r\n\r\n', 400, b''),
        (b'GET / HTTP/1.1\r\nX-Nul: a\x00b\r\n\r\n', 400, b''),
        (b'garbage\r\n\r\n', 400, b''),
        (b'GET / HTTP/1.1\r\nX-Large: ' + b'a' * 65536 + b'\r\n\r\n', 431, b''),
        (b'GET / HTTP/1.', 400, b''),
        (b'POST / HTTP/1.1\r\nContent-Length: 99\r\n\r\nshort', 400, b''),
        (b'', 0, b''),
    ]
    repeated, queued = 100, 64
    count = len(cases) + 2 + 2 + 2 + repeated + queued
    # Extra pairs: fragmented/large, idle/trickling timeouts, receive/send resets.
    with server(executable, [4, count, 700, 8388608, 65536, 0, 0], valgrind) as (process, connect):
        baseline_fds = None
        for data, status, body in cases:
            exchange(connect, data, status, body)
            if baseline_fds is None:
                baseline_fds = len(list(Path(f'/proc/{process.pid}/fd').iterdir()))
        exchange(connect, fragmented, body=fragmented, fragments=True)
        exchange(connect, large, body=large, delay=.1)
        exchange(connect, b'G', 408, b'', halfclose=False)
        with connect() as peer:
            start = time.monotonic()
            for byte in b'GET':
                peer.sendall(bytes([byte]))
                time.sleep(.25)
            assert response(peer)[0] == 408
            assert time.monotonic() - start < 1.4, 'deadline restarted on each read'
        with connect() as peer:
            peer.sendall(b'POST / HTTP/1.1\r\nContent-Length: 99\r\n\r\nshort')
            peer.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack('ii', 1, 0))
        with connect() as peer:
            peer.sendall(large)
            time.sleep(.1)
            peer.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack('ii', 1, 0))
        for _ in range(repeated):
            exchange(connect, get, body=get)
        end = time.monotonic() + 5
        while len(list(Path(f'/proc/{process.pid}/fd').iterdir())) != baseline_fds:
            assert time.monotonic() < end, 'connection descriptors were not closed'
            time.sleep(.01)
        assert len(list(Path(f'/proc/{process.pid}/task').iterdir())) == 5
        with concurrent.futures.ThreadPoolExecutor(max_workers=16) as pool:
            list(pool.map(lambda _: exchange(connect, get, body=get), range(queued)))
        assert process.wait(timeout=30) == 0, 'transport failure or allocation leak'
        assert process.stdout.read() == b'clean shutdown\n'
    print('HTTP transport: framing, limits, deadlines, resets, large/fragmented transfers, fixed pool, and clean shutdown passed', flush=True)
    for legacy in (0, 1):
        with server(executable, [1, 1, 1000, 4096, 4096, legacy, 1], valgrind) as (process, connect):
            exchange(connect, get, body=get)
            assert process.wait(timeout=20) == 0
    print('HTTP transport: synchronous and legacy callbacks passed', flush=True)
    with server(executable, [0, 1, 1000, 4096, 4096, 0, 0], valgrind) as (process, connect):
        exchange(connect, get, body=get)
        assert process.wait(timeout=20) == 0
    with socket.socket() as occupied:
        occupied.bind(('127.0.0.1', 0))
        occupied.listen()
        args = [occupied.getsockname()[1], 4, 1, 1000, 4096, 4096, 0, 0]
        result = subprocess.run([str(executable)], input=struct.pack('<8i', *args),
                                capture_output=True, timeout=10)
        assert result.returncode == 98, ('bind error', result.returncode)
    blocked_reply = request(b'x' * (6 * 1024 * 1024))
    with server(executable, [1, 1, 1000, 8388608, 65536, 0, 0], valgrind) as (process, connect):
        with connect() as peer:
            peer.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1024)
            peer.sendall(blocked_reply)
            # The peer never drains a response larger than the TCP send queue.
            # The write deadline must let the worker terminate and release it.
            assert process.wait(timeout=30) == 0
            assert process.stdout.read() == b'clean shutdown\n'
    print('HTTP transport: default workers, bind failure, and stalled-writer cleanup passed', flush=True)


def application_checks(executable):
    with server(executable, [4]) as (process, connect):
        payload = b'body with\r\nmultiple lines'
        headers, _ = exchange(connect, request(payload), body=payload)
        assert b'X-Middleware: yes' in headers
        exchange(connect, b'GET /path/a/b HTTP/1.1\r\nHost: localhost\r\n\r\n', body=b'a/b')
        exchange(connect, b'GET /missing HTTP/1.1\r\nHost: localhost\r\n\r\n', 404, b'missing')
        headers, body = exchange(connect, b'GET /fail HTTP/1.1\r\nHost: localhost\r\n\r\n', 500)
        assert b'intentional handler failure' in body, body
        for _ in range(30):
            exchange(connect, request(payload), body=payload)
        start = time.monotonic()
        with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
            list(pool.map(lambda _: exchange(connect, b'GET /slow HTTP/1.1\r\nHost: localhost\r\n\r\n', body=b'slow'), range(4)))
        elapsed = time.monotonic() - start
        assert elapsed < .65, f'handlers serialized: {elapsed:.3f}s'
        assert process.poll() is None
    print(f'HTTP application: existing Server/Endpoint/Middleware APIs, wildcard paths, 404/500 responses, and parallel handlers passed ({elapsed:.3f}s)', flush=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--valgrind', action='store_true')
    parser.add_argument('--workdir', type=Path)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='aflat-http-transport-') as temporary:
        directory = args.workdir or Path(temporary)
        transport = build('transport.af', directory / 'transport')
        application = build('application.af', directory / 'application')
        transport_checks(transport, args.valgrind)
        application_checks(application)
        if args.valgrind:
            with server(application, [4], True) as (process, connect):
                for _ in range(8):
                    exchange(connect, request(b'memory check'), body=b'memory check')
                    _, body = exchange(connect, b'GET /fail HTTP/1.1\r\nHost: localhost\r\n\r\n', 500)
                    assert b'intentional handler failure' in body
                assert process.poll() is None
            print('HTTP application: repeated normal/error responses passed Valgrind', flush=True)


if __name__ == '__main__':
    main()
