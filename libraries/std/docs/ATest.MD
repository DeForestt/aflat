# ATest.af Documentation

## Overview
`ATest.af` provides a lightweight testing framework for AFlat programs. It allows grouping tests, managing fixtures, running setup code before each test, and asserting conditions.

## Test Structure
Functions in `ATest` are all untyped `any` function pointers unless otherwise noted.
- **`describe(text: string, fn: any<> )`** – Groups a set of tests under a common description.
- **`it(text: string, fn: any<> )`** – Defines a single test case. After the test, all fixtures are torn down.
- **`beforeEach(fn: any<> )`** – Registers a callback that runs before every `it` block.
- **`fix(name: adr, setUpFn: any<>, tearDownFn?: any<any>)`** – Defines a named fixture. `setUpFn` returns the fixture data. `tearDownFn`, if provided, receives that data when the fixture is destroyed. This teardown mechanism should be used to release resources.
- **`getFixture(name: string)`** – Retrieves or lazily builds fixture data by name. It returns an untyped `any`, so the result must be assigned to a variable with an explicit type and cannot be used directly in template strings.
- **`tearDownFixtures()`** – Runs teardown callbacks for any built fixtures and clears cached data. This is automatically called at the end of each `it`.

## Assertion Helpers
- **`assertEqual(expected, actual)`** – Fails if the values differ.
- **`assertNotEqual(expected, actual)`** – Fails if the values are equal.
- **`assertTrue(condition)`** – Fails if the condition is false.
- **`assertFalse(condition)`** – Fails if the condition is true.
- **`assertNull(value)`** – Fails if the value is not `NULL`.
- **`assertNotNull(value)`** – Fails if the value is `NULL`.
- **`assertError(result)`** – Expects a `Result` with an error.
- **`assertSuccess(result)`** – Expects a successful `Result`.

## Example
```js
.needs <std>
import {describe, it, beforeEach, fix, getFixture,
        assertEqual, assertNotNull, assertTrue, assertSuccess} from "ATest" under test;
import Result from "Utils/Result";

fn alwaysOk() -> Result {
    return Result.accept(42);
};

fn main() -> int {
    // Define a fixture and tear it down after each test
    test.fix("temp", fn () -> string {
        return "foo";
    }, fn (string value) {
        // teardown used for resource management
        print(`cleaning {value}`);
    });

    test.beforeEach(fn () {
        print("setup before case");
    });

    test.describe("Example Suite", fn () -> bool {
        test.it("uses fixture", fn () {
            // getFixture returns `any` so we must specify the type
            const string tmp = test.getFixture("temp");
            test.assertNotNull(tmp);
            test.assertEqual(tmp, "foo");
        });

        test.it("checks success", fn () {
            const Result res = alwaysOk();
            test.assertSuccess(res);
            test.assertTrue(res.get() == 42);
        });
        return true;
    });
    return 0;
};
```
