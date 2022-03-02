# Documentation For AFlat Programming Language

## Trust the programmer philosophy
AFlat aims to trust that developers know what they are doing.  It allows and encurages things that many other modern languages do not such as pointer arythmatic, memory managment, and so on.
<br>

## Types
### int
A 4 byte signed int
```c
int i = 0;
```
### float
A 4 byte signed floating point value
```c
float f = 0.3;
```
### long
An 8 byte signed int any long litteral must begin with a #
```c
long l = #12;
```
### short
A 2 byte integer
```c
short s = 1;
```

### adr
An 8 bit memory address.  In aflat, pointers point to any type
```c
adr a = #0;
```

## Functions

### Syntax
Functions in aflat are defined with the following syntax:
```c
<return Type> <function name>(<arguments>){
    <function body>
};
```
- Functions can be defined with a return type of any of the above types.  Note that there is no return type for void functions.
- If a function does not return a value with the `return` keyword, a return statement with no value is implied. *
    - Keep in mind that this is unsafe as the function will 'return' whatever value is currently in the EAX register.
- For now cunctions can only have up to 6 arguments.  This is due to not passing arguments on the stack and only using the registers.

Example:
```c
int add(int a, int b){
    return a + b;
};
```
### The main Function
The main function is the entry point for aflat.  It is the first function called when aflat is run. It can take optionak arguments int argc and adr argv.
```c
int main(int argc, adr argv){
    // do stuff
    return 0;
};
```

It can also be implemented as follows if you do not need to pass command line arguments:
```c
int main(){
    // do stuff
    return 0;
};
```
### Function Pointers
The name of a function without parenthesis returns a pointer to the function.  This is useful for passing functions as arguments to other functions.
```c
int add(int a, int b){
    return a + b;
};

int main(){
    adr foo = add;
    int i = foo(1, 2);
    return 0;
};
```
### Calling Functions
Functions can be called with the following syntax:
```c
<function name>(<arguments>);
```
- The aflat compiler will not check for the type or number of arguments.  It will simply call the function with the arguments passed. This allows functions to have optional arguments.  It also makes it easier to define function pointers.
    - Strict functions with type checking will be added in the future. They will use the keyword `strict` in the definition.
- Pointers can be called with the same syntax.

example:
```c
int add(int a, int b){
    return a + b;
};

int main(){
    int i = add(1, 2);
    return 0;
};
```

### Including modules
Much like in c or c++, aflat modules are made up of header and source files.  The header file contains the function and class definitions.  The source file contains the implementation of the functions and classes.  Header files should have the extension '.gs' and source files should have the extension '.af'.

Modules from the aflat standard library are included with the following syntax:
```c
.needs <module name>
```

All other modules must be included with the following syntax:
```c
.needs "path/to/module/header.gs" // aflat will assume the .gs if it is left out
```
A root directory for header files can be specified with the following syntax:
```c
.root "path/to/root/directory"
```
If a root directory is not specified, the current directory is used.

The list of standard modules is as follows:
- Collections
    - Handles arrays and list and the ICollection interface
- concurrency
    - Handles threads and pipes defines the Process class
- files
    - Handles file IO and defines the File class
- io
    - Handles input and output to the console
- math
    - Handles math functions
- std
    - Deals with mempry managment
- strings
    - Functions to deal with strings and convert between other types and strings
- asm
    - a repository of syscall wrappers

example of using the io module:
```c
.needs <io> // the io module is included

int main(){
    print("Hello World!\n"); // the io module contains the print function
    return 0;
};
```