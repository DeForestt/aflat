# CLArgs.af Documentation

## Overview
`CLArgs.af` provides utilities for building command line interfaces. It defines a simple argument parser and a value wrapper so AFlat programs can accept options and subcommands.

## Union `CLAValue`
Represents a parsed argument value.

- `String(string)` – Stores a string argument.
- `Number(int)` – Stores an integer argument.
- `Boolean(bool)` – Represents a flag.
- `None` – Indicates no value present.

### Methods
- `getString() -> string!` – Returns the contained string or an error if the value is not a string.
- `toString() -> string` – Converts the contained value to a printable string.

## Class `Argument`
Describes a single accepted option.

Fields are immutable once constructed:
- `longName` – Long form, e.g. `--help`.
- `shortName` – Short form, e.g. `-h`.
- `description` – Help text shown in `printHelp()`.
- `defaultValue` – Optional default when the option is not provided.

### Methods
- `init(longName, shortName, description, ?defaultValue)` – Constructor.
- `toString() -> string` – Formats the argument for help output.

## Class `CommandLineArgs`
Handles parsing of the program arguments.

### Construction
`CommandLineArgs(argc, argv)` builds the object. Use `with_arguments([...])` to register a list of `Argument`s before calling `parse()`.

### Methods
- `parse() -> bool!` – Parses the provided `argv` array. Returns an error when an unknown tag is encountered.
- `printHelp()` – Prints usage information with all registered arguments.
- `popCommand() -> CLAValue?` – Removes and returns the next positional argument.
- `hasTag(tag) -> bool` – Checks if an option was supplied.
- `getTag(tag) -> CLAValue?` – Retrieves the value for a given tag.

### Fields
- `programName` – The name of the executable after `parse()`.

## Example
```js
.needs <std>
import string from "String";
import { print } from "String" under str;
import {Some, None, optionWrapper} from "Utils/option" under opt;
import {accept, reject, resultWrapper} from "Utils/result" under res;
import CLAValue, Argument, CommandLineArgs from "CLArgs";

fn main(int argc, adr argv) -> int {
    let args = new CommandLineArgs(argc, argv).with_arguments([
        new Argument("help", "h", "Display this help message"),
        new Argument("version", "v", "Display the version of the program"),
        new Argument("config", "c", "Path to the configuration file", "default.conf"),
        new Argument("verbose", "V", "Enable verbose output")
    ]);

    match args.parse() {
        Ok => {},
        Err(e) => {
            str.print(`Error parsing command line arguments: {e}\n`);
            args.printHelp();
            return 1;
        }
    };

    str.print(`Program Name: {args.programName}\n`);
    if args.hasTag("help") {
        args.printHelp();
        return 0;
    };

    let command = match args.popCommand() {
        Some(cmd) => resolve cmd.getString().expect(`Command is not string`),
        None => resolve "[NULL COMMAND]"
    };

    let config = match args.getTag("config") {
        Some(configPath) => resolve configPath.getString().expect(`Failed to get config path`),
        None => resolve "default.conf",
    };

    str.print(`running {command} with config {config}\n`);

    return 0;
};
```
This example shows how to define options, parse them, and access both tagged and positional arguments.
