# CLArgs

`CLArgs.af` provides argument descriptors, command-line parsing, and typed
values. Import `CLAValue`, `Argument`, and `CommandLineArgs` from `CLArgs`.

## Value and descriptor types

`CLAValue` is a union with `String(string)`, `Number(int)`, `Boolean(bool)`, and
`None` variants. `getString() -> string!` produces an owned string or an Error;
`toString() -> string` formats any variant.

`Argument` describes an option with const `longName`, `shortName`,
`description`, and `defaultValue` string fields. Its constructor is:

```aflat
fn init(const adr longName, const adr shortName, const adr description,
        ?const adr defaultValue) -> Self;
```

The optional raw default is wrapped locally inside the constructor and
extracted to initialize a string. Omitting it uses the text `"false"`.
`__copy__()` returns an independent descriptor.

## CommandLineArgs

Construct with `new CommandLineArgs(argc, argv)`. `with_arguments` consumes a
`vector::<Argument>` and returns a loan of the parser. Keep a named parser
owner instead of keeping only a loan returned from a temporary receiver.

| Method | Return and meaning |
| --- | --- |
| `with_arguments(const vector::<Argument> &&args)` | `loan CommandLineArgs`; register descriptors. |
| `parse()` | `bool!`; validate and collect arguments. |
| `printHelp()` | `void`; print registered options. |
| `popCommand()` | `local CLAValue?`; remove a positional value into a local option. |
| `popCommandString()` | `string!`; remove a positional string. |
| `popCommandRaw()` | `adr!`; remove a positional raw argv address. |
| `hasTag(const string tag)` | `bool`; whether the tag was supplied. |
| `getTag(const string tag)` | `CLAValue`; absence is its `None` variant, not an outer option. |
| `getTagString(const string tag)` | `string!`; retrieve a required string value. |
| `getTagInt(const string tag)` | `int!`; retrieve an integer value. |

`programName` is set by `parse()`. Raw argv pointers must remain valid while
used. A helper forwarding `popCommand()` must also declare a local return.

## Example

```aflat
.needs <std>
import string from "String";
import {print} from "String" under str;
import CLAValue, Argument, CommandLineArgs from "CLArgs";

fn main(int argc, adr argv) -> int {
    const let args = new CommandLineArgs(argc, argv);
    args.with_arguments([
        new Argument("help", "h", "Display help"),
        new Argument("config", "c", "Configuration file")
    ]);
    match args.parse() {
        Ok(_) => {},
        Err(error) => {
            str.print(`Invalid arguments: {error}\n`);
            return 1;
        }
    };
    if args.hasTag("help") {
        args.printHelp();
        return 0;
    };
    match args.getTag("config") {
        String(path) => str.print(`Config: {path}\n`),
        Number(_) => {},
        Boolean(_) => str.print("Config requires a path\n"),
        None() => str.print("Using default.conf\n")
    };
    return 0;
};
```

See [the source](../src/CLArgs.af) and
[the ownership guide](../../../Ownership.md).
