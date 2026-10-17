# Aflat server migration progress

Status: implemented and validated in the repository. Final verification completed
on September 22, 2026. The session crash did not lose source changes.

## Scope and implementation

Server networking, HTTP request assembly, and worker dispatch now live in Aflat.
The existing server APIs and native worker model are preserved. Async networking
remains a separate phase. C HTTP-client/shell helpers and the general thread and
allocator runtimes remain.

- `libraries/std/src/Socket.af`: owned descriptors, IPv4 listen/accept, absolute
  monotonic deadlines, nonblocking reads, complete writes, and SIGPIPE suppression.
- `libraries/std/src/HTTP/Transport.af`: bounded buffers, framing validation,
  request/header limits, error responses, and a fixed pool fed by a bounded pipe
  queue. Finite serving drains the queue and joins workers before releasing it.
- Preserved `serve`, `serve_pool`, `serve_sync`, and `_aflat_server_spinUp`.
  Synchronous serving invokes handlers on the calling thread.
- `HTTP/Server.af` retains endpoints/middleware and adds `setRequestSizeLimit`
  and `setTimeout`. Defaults: 8 MiB requests, 64 KiB headers, and 30 seconds for
  reading a request and separately writing a response. Handler execution itself
  is not interrupted by these deadlines.
- Removed server implementations from `request.c`; registered both Aflat modules
  in the library rebuild script and compiler linker list.
- Updated README and Comprehensive.md, and added reproducible HTTP/socket
  fixtures under `test/runtime/`.

## Integration fixes and progress record

1. Socket feasibility tests exposed missing fourth-argument register moves in
   `sys_setsockopt` and `sys_sendto`; fixed those and added ABI/runtime coverage.
   Added `sys_poll` and `sys_recvfrom` bindings for the transport.
2. Implemented transport and preserved callback/worker APIs. Corrected the
   transport export namespace by compiling `Transport.s` before renaming it.
3. Wildcard endpoint compilation exposed out-of-bounds function-pointer argument
   comparisons. Added bounds checks and a Catch2 regression.
4. Array indexing overwrote an enclosing expression's `rdx` operand, corrupting
   comparisons and clock arithmetic. Moved the array-base operation to index
   scratch register `r13` and added a runtime regression.
5. A 2 MiB request exposed a pointer-store code-generation bug: two pushes but
   one pop per store overflowed worker stacks. Balanced the generated stack
   operations and tested two million stores under a 1 MiB stack limit.
6. Endpoint testing exposed multiline body truncation in `HTTPMessage.parseReq`.
   It now preserves the complete suffix after the header delimiter.
7. Resumed after the session crash and traced empty HTTP 500 bodies to premature
   destruction of `err.render()` before `HTTPResponse` copied its `cstr()` result.
   Address-like results now retain the receiver through its lexical scope.
   Both the primitive-result and generic `owned` cleanup paths needed correction.
   The strengthened regression checks that the backing allocation remains live,
   then checks cleanup across repeated calls. The exact failing expression now
   passes Valgrind, and generated assembly copies the body before destruction.

## Final validation

- CMake compiler/test build: passed.
- Full standard-library rebuild after the final compiler changes: all libraries
  compiled successfully (`bash ./rebuild-libs.sh -j4`).
- Four focused compiler regressions: 14 assertions passed.
- Related reference/array/function-pointer Catch2 tests: 22 assertions passed
  across seven cases.
- Four new Aflat HTTP unit cases: passed independently, including multiline
  bodies, mixed-case Content-Length, ambiguous framing, and decimal overflow.
- HTTP integration suite: passed framing errors, limits, absolute timeouts,
  fragmented/2 MiB requests, resets, stalled writers, 100 repeated connections,
  64 concurrent clients, fixed thread count, descriptor/allocation cleanup,
  sync/legacy callbacks, default workers, bind failure, and clean pool shutdown.
- Real Server/Endpoint/Middleware fixture: passed multiline echo, wildcard paths,
  404/500 bodies, repeated requests, and concurrent handlers. Four 200 ms handlers
  completed in 0.203 seconds.
- Valgrind: zero errors for all finite transport cases and repeated normal/error
  application responses. Finite cases had no lost allocations and returned to
  tracked allocation baselines. The indefinitely running application was stopped
  with SIGTERM; its report retained live application state and 1,152 bytes of
  possibly lost pthread TLS allocations for its four active workers. It is not
  a graceful application-shutdown leak check.
- Original socket probe: passed syscall ABI checks, fragmented/binary/8 MiB
  transfers, disconnect recovery, repeated/queued clients, resource guards, and
  Valgrind with zero errors and no lost allocations.
- `git diff --check`: passed.

## Saved C comparison

Identical Aflat callback, current compiler/helper libraries, four workers and
clients, 256-byte POST bodies, 1,200 requests per trial, three trials per transport.
The C variant links the saved pre-migration server assembly. Trial order alternates.
No compiler/library builds ran during the final measurement.

| Transport | Requests/sec per trial | Median requests/sec | Median latency across trials |
| --- | --- | --- | --- |
| Saved C | 3,358 / 2,874 / 2,831 | 2,874 | 1.352 ms |
| Aflat | 2,950 / 2,895 / 2,878 | 2,895 | 1.348 ms |

Every response was checked. This small Python-client/loopback comparison shows
similar observed throughput; it does not measure production capacity or establish
a performance improvement. Baseline files, script, and results remain under
`/tmp/aflat-server-migration/`.

## Validation limits

- The complete Aflat test suite cannot currently compile its existing
  `s.replace(new string(""), "x")` case: an owned temporary is passed to a
  non-consuming parameter. The new HTTP cases were run separately and passed.
- A temporary copy of the todo application cannot compile its vendored aflat_db:
  `Row/mod.af:438` returns stack storage as an owned pointer, and
  `TableCatalog/EntryCodec.af:174` has a constructor argument mismatch.
  Full todo-app compatibility is therefore unverified. Original app files were
  not modified.
- Protocol remains one request per connection with text callback buffers.
  Chunked requests receive 501. Keep-alive and async serving are not implemented.

## Reproduce

```sh
cmake --build build -j4
bash ./rebuild-libs.sh -j4
./bin/a.test 'temporary receiver addresses*,callback argument prefixes*,array reads preserve*,repeated pointer stores*'
./bin/a.test '[function-pointer],[reference],[array]'
python3 test/runtime/socket_probe/run.py --valgrind
python3 test/runtime/http_transport/run.py --valgrind
```

Final logs and isolated test projects are in `/tmp/aflat-server-migration/`,
including `libs-final.log`, `focused-tests.log`, `related-tests-final.log`,
`http-unit.log`, `http-final.log`, `socket-final.log`, and `benchmark-results.json`.
No migration implementation work remains pending; the validation limits above
and future async/keep-alive work remain outside this change.

## Missing-file recovery after PR #582

- The merged commit included tracked edits but omitted the new modules and tests.
- Created `fix/http-migration-missing-files` from `origin/main` (`c08cb8e0`) in an
  isolated checkout, restoring 12 migration source/test/progress files.
- Confirmed main's tracked tree matches the previously validated migration tree.
  Reusing those CMake-built compiler/test executables for recovery validation.
- Recovery validation passed: complete standard-library rebuild, all four
  focused compiler regressions (14 assertions), and both socket and HTTP
  integration suites with Valgrind (zero memory errors).
- The user authorized publishing the recovery directly to main. The recovery
  is an additive commit on top of the merged migration; no history rewrite is
  needed.
- Unrelated untracked files and generated binaries are excluded from the commit.
