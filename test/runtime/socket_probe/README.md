# Shim-free socket regression

From the repository root, with `bin/aflat` built using CMake:

```sh
python3 test/runtime/socket_probe/run.py
python3 test/runtime/socket_probe/run.py --valgrind
```

The ordinary run is also registered with Catch2 as `[socket]`. Local socket
access is required; run outside a sandbox that denies networking syscalls.

`abi.c` tests the assembly wrappers independently of compiler code generation.
It poisons `r10` before calling `sys_setsockopt` and `sys_sendto`, proving that
argument four is copied from the function ABI's `rcx`. It checks the resulting
socket option, ordinary data delivery, partial writes, negative errno returns,
and suppression of SIGPIPE after a peer closes.

`main.af` is a synchronous, single-process AFlat TCP echo server. The runner
links it only with `asm.s`, `allocator.s`, and platform runtime libraries.
It does not link `request.s`, the HTTP libraries, or the async runtime.
The explicit 16-byte IPv4 address layout is Linux x86-64 specific; the test
does not establish C-compatible layout for arbitrary AFlat classes.

The runner checks all 256 byte values, an 8 MiB payload, fragmented headers
and bodies, half-closes, early EOF, resets during receive and send, 200 repeated
connections, and 32 concurrent clients queued on the synchronous server.
Slow readers and a small send buffer force partial writes and EAGAIN; the
server requires both to occur before reporting success. Guard bytes surround
socket address and heap buffers, and descriptor counts and live allocation
counts must return to baseline.

The protocol is a four-byte little-endian payload length followed by the
payload, echoed unchanged. Standard input configures the port and number of
connections; the runner picks an available loopback port. This is a regression
probe, not a production server: its nonblocking send loop deliberately busy
retries, it handles one accepted connection at a time, and it does not parse
HTTP or implement async cancellation.
