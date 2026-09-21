# ATest

`ATest.af` is the AFlat test library. Compiler C++ tests instead use the bundled
Catch2 v2 header in `test/catch.hpp`; do not link a second Catch2 implementation.

## Structure and fixtures

- `describe(const adr text, const adr implementation)` groups test callbacks.
- `it(const adr text, const adr implementation)` runs a case and tears down its fixtures.
- `itSkip(const uni_string text, const uni_string reason)` reports a skipped case.
- `beforeEach`, `afterEach`, `beforeAll`, and `afterAll` register callbacks.
- `fix(const adr name, const adr setUp, *const adr tearDown)` registers a lazy fixture.
- `getFixture(const uni_string name) -> loan any` borrows fixture data. Assign it
  an explicit type and keep its use within the fixture's lifetime; it is not a
  new owner to sell or delete.
- `tearDownFixtures()` runs fixture teardown; `it` calls it automatically.
- `summary()` prints accumulated pass/fail/skip counts.

Callbacks are passed as `adr` values. Set up and tear down resource ownership
explicitly; an untyped fixture handle does not infer how to free its resource.

## Assertions

`assertEqual` and `assertNotEqual` compare values. `assertTrue`, `assertFalse`,
`assertNull`, and `assertNotNull` check booleans or pointers. For typed wrappers,
use `assertSome`, `assertNone`, `assertSomeValue`, `assertOk`, `assertErr`, and
`assertOkValue`. The old `assertSuccess`/`assertError` names are not exported.
`fail(message)` records an explicit failure.

## Example

```aflat
.needs <std>
import {describe, it, assertTrue, assertFalse, summary} from "ATest" under test;

fn main() -> int {
    test.describe("Arithmetic", fn () {
        test.it("adds integers", fn () {
            test.assertTrue(2 + 2 == 4);
            test.assertFalse(2 + 2 == 5);
        });
    });
    test.summary();
    return 0;
};
```

Put tests at the `[build] test` path in `aflat.cfg` and run `aflat test`.
`aflat test --coverage` also reports production-line coverage. Inspect the
printed ATest summary: returning zero from `main` is not itself proof that its
recorded assertions passed.

## Current generic-helper limitation

The generic assertion helpers are present in the module, but standalone
imports of `assertEqual`, `assertOk`, and `assertOkValue` currently expose
compiler errors resolving their internal `markFailure` call. Result helpers
can additionally report a borrowed-receiver error for `unwrapErr`. The example
above uses the working non-generic boolean assertions. A comparison or explicit
result match followed by `assertTrue` is an alternative while those generic
imports are unresolved.
