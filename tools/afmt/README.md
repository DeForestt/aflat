# afmt

This project was generated with the AFlat toolchain. Use the commands below to build, run, test, and explore the codebase.

## Build & Run
- `aflat build` - compile sources defined in `aflat.cfg`.
- `aflat run` - build and execute the resulting binary.
- `aflat test` - build and run the configured test target.
- `aflat test --coverage` - report executable production lines covered by tests.
- `python3 src/test/cli_test.py` - test CLI output modes after building.

`./bin/afmt file.af` prints formatted source to stdout without changing the file.
`./bin/afmt --inplace file.af` (or `-i`) overwrites the file and produces no formatted stdout output.

Coverage excludes the test tree and dependencies added with `aflat install`; unreached lines are flagged as possibly dead or untested.

## Inspecting Code
- `aflat docs src/main.af` lists classes, unions, transforms, and functions in this project.
- `aflat docs String` explores a standard library module.
- `aflat docs Async` lists the native timer, task-control, wakeup, and asynchronous I/O runtime surface.
- The complete async/await guide is available at https://github.com/DeForestt/aflat/blob/main/Async.md.

### Creating modules
Use `aflat module <module name>` to scaffold `src/<module name>/mod.af` and automatically register it under [dependencies]. For single files, run `aflat file <name>`.

### Standard library overview
AFlat ships a batteries-included standard library under `libraries/std`. The modules below are the most commonly used building blocks:

#### Runtime & strings
- `std` - Arena allocator, `af_malloc`/`af_free`, `af_memcpy`, `panic`, `sleep`, and other intrinsics (libraries/std/src/std.af).
- `Async` - Native `task<T>` scheduler support for `async fn`, `await`, `spawn`, `run`, `yield`, `pause`, cancellation, detachment, timers, and async file I/O (libraries/std/head/Async.gs).
- `std-cmp` - Compatibility shim that swaps in libc memory helpers plus panic/assert when the arena can't be used (libraries/std/src/std-cmp.af).
- `Memory` - RefCounted base class alongside the generic `Box`/`wrap` helpers for owning values (libraries/std/src/Memory.af).
- `String` - Assembly-backed string primitive covering allocation, slicing, iteration, comparisons, casing, and conversions (libraries/std/src/String.af).
- `strings` - Standalone utilities such as `len`, concat, ASCII helpers, and int/float parsing/formatting (libraries/std/src/strings.af).
- `System` - Thin syscall wrappers for `execve`, `exec`, shell execution, and environment-variable helpers (libraries/std/src/System.af).

#### Numerics & IO
- `math` - Numeric helpers including `Random`, integer/float exponentiation, square roots, conversions, and absolute values (libraries/std/src/math.af).
- `io` - Terminal IO functions for reading strings, printing chars/ints/hex, and emitting ANSI-colored output (libraries/std/src/io.af).
- `files` - File descriptor wrapper with `FileError`/`ReadError`, buffered reads, iterators, synchronous write helpers, and native async read/write tasks (libraries/std/src/files.af).
- `DateTime` - Epoch-based `DateTime` struct with parsing, getters, formatting, and day-of-week math (libraries/std/src/DateTime.af).

#### Collections
- `Collections/Vector` - Type-parametric `vector` supporting copy/move semantics, iterators, sorting, and optional accessors (libraries/std/src/Collections/Vector.af).
- `Collections/unordered_map` - Hash-tree-backed associative container with `set`/`get`/`keys` and Option-aware lookups (libraries/std/src/Collections/unordered_map.af).
- `Collections/Tuple` - Value-type tuples with ownership-safe destruction and `make_tuple` helper (libraries/std/src/Collections/Tuple.af).

#### Data modeling & serialization
- `JSON` - Tagged union covering JSON primitives, casting helpers, mutation, and pretty printing (libraries/std/src/JSON.af).
- `JSON/Parse` - Wrapper that routes to `JSON.parse` for turning strings into JSON values (libraries/std/src/JSON/Parse.af).
- `JSON/Property` - Reserved module for future JSON/property bindings (libraries/std/src/JSON/Property.af).

- `JSON/Property/Fields` - JSON property field decorators (libraries/std/src/JSON/Property/Fields.af).

#### HTTP & networking
- `HTTP` - HTTP errors, verb enum, request parser, `HTTPMessage`/`HTTPResponse`, and `listen` helper (libraries/std/src/HTTP.af).
- `HTTP/Endpoint` - Endpoint wrapper/registry plus pluggable NotFound handler (libraries/std/src/HTTP/Endpoint.af).
- `HTTP/Endpoints` - Sugar classes for registering GET/POST/PUT/DELETE/etc. handlers (libraries/std/src/HTTP/Endpoints.af).
- `HTTP/Server` - Middleware-aware server that dispatches endpoints, supports wildcards, and formats error responses (libraries/std/src/HTTP/Server.af).
- `HTTP/Middleware` - Before/after middleware registration helper applied per request (libraries/std/src/HTTP/Middleware.af).
- `request.c` - C shim exposing `request`, `_aflat_server_spinUp`, and `serve` socket utilities (libraries/std/src/request.c).

#### Utility types & error handling
- `Utils/Option` - Ref-counted Option class with `resolve`, `match`, and defaulting helpers (libraries/std/src/Utils/Option.af).
- `Utils/option` - Lightweight union mirroring Rust's Option with `Some`/`None` constructors (libraries/std/src/Utils/option.af).
- `Utils/Result` - Class-based result value for bridging APIs that expect dynamic success/error payloads (libraries/std/src/Utils/Result.af).
- `Utils/result` - Rust-style `result<T>` union with ergonomic constructors and unwrap helpers (libraries/std/src/Utils/result.af).
- `Utils/Error` - Base `Error` class with type metadata, render hooks, and pattern matching (libraries/std/src/Utils/Error.af).
- `Utils/Error/Render` - Decorator that lets errors plug in a render callback (libraries/std/src/Utils/Error/Render.af).

#### Utility infrastructure & patterns
- `Utils/Functions` - Capturing function wrapper that invokes stored callbacks with optional captures (libraries/std/src/Utils/Functions.af).
- `Utils/Defer` - RAII helper that runs a `Function` when the object leaves scope unless dismissed (libraries/std/src/Utils/Defer.af).
- `Utils/Map` - Tree-backed map with hashing, normalization, and Option-returning lookups (libraries/std/src/Utils/Map.af).
- `Utils/Object` - Dynamic object base with type tagging, validation hooks, and match support (libraries/std/src/Utils/Object.af).
- `Utils/Properties` - Property decorators (`computed`, `readonly`, `lazy`, `observable`, `clamped`, `with`) plus supporting classes (libraries/std/src/Utils/Properties.af).
- `Utils/Observable` - Observer list that notifies subscribers when events fire (libraries/std/src/Utils/Observable.af).

#### Tooling & frameworks
- `ATest` - BDD-style testing DSL with fixtures, describe contexts, hooks, and reporting helpers (libraries/std/src/ATest.af).
- `concurrency` - Legacy/process-oriented concurrency abstractions (`Process`, `MProcess`), message pipes, and `AsyncResult` handles; native `async fn` uses the separate task runtime (libraries/std/src/concurrency.af).
- `CLArgs` - Command-line parser with long/short flags, typed values, validation, and help text (libraries/std/src/CLArgs.af).
- `Web/Content` - File-backed templating utility that applies bindings before rendering (libraries/std/src/Web/Content.af).
- `Web/Content/Bind` - Binding helper that registers template placeholders and preprocessors (libraries/std/src/Web/Content/Bind.af).

## Refreshing This README
Run `aflat readme [path] [name]` to regenerate these instructions for any folder.

