# First Steps with AFlat

### Installation
Please see the [installation instructions](README.md) for the steps to done.  If aflat is already installed, then you can move on the the next section.

### First Project
To create the tutorial project, run the following command in your project directory.
```bash
aflat make tutorial
```

The tutorial project will have been created in the `tutorial` directory.  The tutorial project will have a `aflat.cfg` file that will tell the aflat build system what to build and link.  The tutorial project will have a `src/main.af` file that will be the entry point for the project. It will also have a `src/test/test.af` file that is will be used to test the project with the `aflat test` command.

Open up the `main.af` file in your favorite text editor.  The file will look something like this:
```js
.needs <std>
import * from "io" under io;

int main() {
    io.print("Hello, world!");
    return 0;
};
```

Lets walk through the program line by line.
- The first line is the `.needs` directive.  This tells aflat that it needs the standard library.  The standard library is a collection of useful functions that are included in the aflat distribution.  The standard library is included in the `aflat/libraries/std/head` directory.
- The second line is the `import * from "io" under io;` command.  This tells aflat to import the `io` library from the `aflat/libraries/std/src` directory.  The `io` library is a collection of functions that are used to read and write to the standard input and output.  The `io` library is included in the `aflat/libraries/std/src` directory.  The under command tells aflat that the `io` library should be imported under the name `io` any functions called from this library must start with `io.`.
- the fourth line defines the main function.  This is the function that is called when the program is run.
- The fifth line calls the `io.print` function.  This function prints the string `Hello, world!` to the standard output.
- The sixth line returns 0.  This is the value that is returned to the operating system when the program is run.