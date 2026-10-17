# Aflat HTTP transport integration tests

From the repository root, build the compiler with CMake and rebuild all standard
libraries, then run:

```sh
python3 test/runtime/http_transport/run.py
python3 test/runtime/http_transport/run.py --valgrind
```

The harness builds isolated projects with the repository compiler and libraries.
It requires Python 3, GCC, Linux loopback sockets, and optionally Valgrind.
`--workdir /tmp/aflat-http-check` preserves generated projects and build logs.
The Catch2 case tagged `[http][server][runtime]` invokes the same harness.

Coverage includes framing, malformed and oversized requests, absolute read and
write deadlines, fragmented and large transfers, reset peers, fixed worker
counts, descriptor/allocation cleanup, finite pool shutdown, bind errors,
synchronous and legacy callbacks, plus Server/Endpoint/Middleware routes,
multiline request bodies, wildcard paths, 404/500 responses, and concurrent
handlers. Valgrind covers the finite transport fixtures and repeated normal/error application
responses. The application fixture runs until terminated by the harness.

The protocol remains one request per connection with text callback buffers.
Chunked requests are rejected; keep-alive and async networking are separate work.
