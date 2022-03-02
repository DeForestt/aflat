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
### Annonymous Functions
Functions can be defined without a name.  This is useful for passing functions as arguments to other functions.
```c
[<parameters>]=>{
    <function body>
};
```
example:
```c
int main(){
    
    adr add = [int a, int b]=>{
        return a + b;
    };

    return add(1, 2);
};
```

## Statements

### Declarations
Declarations are used to define variables.  They are used to define variables and functions.
```bnf
<declaration> ::= <type> <identifier>;
```
### Declare and assign
```bnf
<declaration> ::= <type> <identifier> = <expression>;
```
### Assign
```bnf
<assignment> ::= <identifier> = <expression>;
```
### Return
```bnf
<return> ::= return <expression>;
```
### reference
```bnf
<reference> ::= ?<identifier>;
```
- returns the address of the variable

### Load
```bnf
<load> ::= <identifier> := <expression>;
```
- Loads the value of the expression into the memory address held in the identifier.

## Control Flow

### If Statements
If statements are used to execute code based on a condition.  The syntax is:
```c
if(<condition>){
    <code to execute if condition is true>
};
```
- The condition is evaluated before the code is executed.  If the condition is true, the code is executed.

Condition syntax is as follows:
```bnf
<condition> ::= (<expression> <relational operator> <expression>)
<relational operator> ::= == | != | < | >
```
- Aflat does not support logical operators at this time.

if statments can be used with else statements.  The syntax is:
```c
if(<condition>){
    <code to execute if condition is true>
}else{
    <code to execute if condition is false>
};
```
### While Loops
While loops are used to execute code while a condition is true.  The syntax is:
```c
while(<condition>){
    <code to execute>
};
```
### For Loops
For loops are used to execute code a set number of times.  The syntax is:
```c
for{<code to run once>}(<condition>){<code to run at end of each iteration>}{
    <Internal loop code>
};
```
This syntax is designed to make for loops more flexible than while loops.  The code to run once is executed before the loop starts.  The condition is checked at the start of each iteration.  The code to run at the end of each iteration is executed at the end of each iteration.  The internal loop code is executed between the start and end of each iteration.

example:
```c
.needs <io>

int main(){
    for {int i = 0;}(i < 10){i = i + 1;}{
        printInt(i);
    };
    return 0;
};
```

## Classes
Classes in aflat are effectively structs that can implement functions and support encapsulation and rudimentary inheritance.  The syntax is:
```c
class <class name> signs <parent class>{
    <contract>
    <class variables>
    <class functions>
};
```



## Including modules
Much like in c or c++, aflat modules are made up of header and source files.  The header file contains the function and class definitions.  The source file contains the implementation of the functions and classes.  Header files should have the extension '.gs' and source files should have the extension '.af'.

Modules from the aflat standard library are included with the following syntax:
```c
.needs <module name>
```

All other modules must be included with the following syntax:
```c
.needs "path/to/module/header.gs"
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