# CLArgs.af Documentation

## Overview
`CLArgs.af` provides a lightweight command line argument parser for Aflat programs. It allows a program to declare expected options and parse them into a convenient structure.

## Classes

### `Spec`
Represents a single command line option.
- `long`: Long form name (`--name`).
- `short`: Short form name (`-n`).
- `needsValue`: Indicates if the option expects a value.
- `description`: Brief usage description.
- `defaultVal`: Value used when the option is specified without an explicit value.

### `Parsed`
Stores the value of an option after parsing.
- `spec`: The `Spec` definition for the option.
- `value`: The value supplied on the command line.

### `ArgsList`
Main parser class.
- `addOption(long, short, needsValue, description, defaultVal)`: Register a new option.
- `init(argc, argv)`: Parse the provided arguments.
- `get(name)`: Retrieve a parsed option as a `Result`.
- `has(name)`: Check if an option was supplied.
- `help()`: Build a usage string describing all registered options.

## Example Usage
```js
ArgsList args = new ArgsList();
args.addOption("help", "h", false, "Show help", "");
args.addOption("output", "o", true, "Output file", "a.out");
args.init(argc, argv);

if args.has("help") {
    io.print(args.help());
    return 0;
};

string out = args.get("output").value([] => { return "a.out"; });
```
