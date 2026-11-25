# Error ownership

`Utils/Error.Error` is a unique, move-only value. It owns its type-name and
message strings and releases them when its owner is destroyed. A failed
`result<T>` owns its inline `Error`; discarding the result releases the error.
The older shared `Utils/Result.Result` releases its error when its last reference
is released.

Constructors accept a string literal (`adr`), a borrowed `string`, or a transferred
`string` (`$message`). Literals are copied directly into owned storage. Borrowed
strings are copied, so the caller may destroy its string independently. Passing
an owned temporary or `$message` transfers the message instead. Do not reuse a
transferred value.

`render()` returns an owned string. `kind()` returns a loan into the error.
`unwrapErr()` borrows from a result; its sink overload can extract an owned error
from a consumed result. Loans must not outlive their owner. To propagate a named
error, transfer it explicitly with `$`.

## Concrete errors and results

Classes that `sign Error` must be unique and own their additional resources.
The standard file, HTTP, and date/time errors follow this rule. A render callback
may access concrete fields only while that concrete error is alive.

`reject` consumes a concrete error and takes a snapshot of its kind and rendered
message before destroying it with its concrete destructor. The stored value is
an independent `Error`. This avoids copying a larger concrete layout into the
fixed-size inline `Err(Error)` payload or losing its destructor.

The snapshot preserves the public description and `is`/`kind` classification.
It does not retain concrete fields, the render callback, or `data`. Code that
needs a structured error payload should use a union with that concrete payload
and keep its ownership explicit. In particular, a `match` callback on a snapshot
receives the snapshot, not the original concrete object.

`Error.snapshot()` also exposes this conversion for a borrowed error. Directly
constructing an owning union variant from a different concrete class is rejected;
convert to the declared payload type first. Owning parameters also reject a
concrete signer passed as its base type. For APIs such as `option.toResult` or
the legacy `Result` constructor, pass a base error or an explicit snapshot.
Borrowing a concrete error through its contract remains supported.

Passing a base `Error` to `reject` moves it directly without taking a snapshot.

`data` is a raw borrowed address. Error ownership does not manage its target or
make arbitrary raw pointers safe. Unique ownership also does not make renderer
callbacks or concurrent access automatically safe.

Rebuild applications and standard-library modules together when upgrading. The
Error contract layout and ownership changed; older generated result methods are
not compatible with the new layout.
