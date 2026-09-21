# String

`String.af` exports the lowercase `string` class. Its current declaration is
`unique safe dynamic class string`: it is heap constructed and participates in
unique ownership cleanup. There is no uppercase `String` class in this module.
The class retains legacy reference-count methods, but a borrowed string is not
an independent owner. For APIs using `uni_string`, import that separate module.

## Construction, copying, and borrowing

`new string(raw)` copies a null-terminated buffer into owned storage.
`copy()` and `__copy__()` create independent strings. `get()` has a borrowing
`-> loan string` overload and a sink `-> string` overload; a named receiver
selects the sink form only through `$value.get()`.

`cstr()` returns the address of the internal buffer. Keep the string alive and
do not mutate or destroy that buffer while using the address. Copying an `adr`
does not copy its bytes. `del()` releases the buffer; ordinary ownership cleanup
handles destruction. `seal()` changes the legacy permanent flag and returns a
loan, not a new string.

## Common methods

| Method | Result / behavior |
| --- | --- |
| `at(index)` | `char`; out-of-range access returns `\0`. |
| `cstr()` | `adr` into the string's buffer. |
| `copyTo(buffer)` | Copies into caller-provided storage. |
| `len()` | `int` length. |
| `copy()` / `__copy__()` | Owned `string` copy. |
| `toInt()` | `int!` (`result::<int>`), not the legacy `Result` class. |
| `split(delim)` / `splitAtFirst(delim)` | Owned `vector::<string>`. |
| `next()` / `peek()` | `option::<char>`; these currently use heap wrappers. |
| `tell()` / `consume(text)` | Iterator position / conditional consumption. |
| `toUpper()` / `toLower()` / `title()` | Owned transformed string. |
| `replaceChar(old, replacement)` | Owned transformed string. |
| `find(sub, start)` / `indexOf(ch)` | `int` position. |
| `replace(sub, replacement)` | Owned string; both arguments are strings. |
| `subString(start, end)` / `trim(*char ch)` | Owned string. |
| `push(ch)` / `insert(index, text)` / `sort()` | Owned string. |
| `forEach(callback, *args)` | Calls a callback for each character. |

Use `aflat docs String` or [the source](../src/String.af) for all overloads.

## Exported helpers

- `print(const string obj) -> int`
- `readString(*const string prompt) -> string`
- `floatToString(const float value) -> string`
- `fString(const string format, *adr args) -> string`
- `readInt(const string prompt, ?const int tries) -> int`
- `itos(const int value) -> string`

`readInt` wraps `tries` locally. An omitted value or zero means no supplied
retry count, so the implementation uses three attempts. `readString` uses a
raw defaulted parameter (`*`), not an option parameter (`?`).

## Example

```aflat
.needs <std>
import string from "String";
import {print} from "String" under str;

fn main() -> int {
    const string text = new string("42");
    const let parsed = text.toInt();
    match parsed {
        Ok(value) => str.print(`Value: {value}\n`),
        Err(error) => str.print(`Invalid number: {error}\n`)
    };
    return 0;
};
```

See [the ownership guide](../../../Ownership.md) for borrowed versus consuming
result extraction and automatic cleanup.
