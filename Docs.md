# Documentation For AFlat Programming Language

## Trust the programmer philosophy
AFlat aims to trust that developers know what they are doing.  It allows and encourages things that many other modern languages do not, such as pointer arythmatic, memory managment, and so on.
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
The main function is the entry point for aflat.  It is the first function called when aflat is run. It can take optional arguments int argc and adr argv.
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
### Store
```bnf
<load> ::= <identifier> =: <expression>;
```
Stores the value of an expression into the address pointed to by an identifier.
### Return
```bnf
<return> ::= return <expression>;
```
### reference
```bnf
<reference> ::= ?<identifier>;
```
Returns the address of the variable
## Expressions
### Int Literal
```bnf
<int literal> ::= *<int>
```
eg: `123`
### Float Literal
```bnf
<float literal> ::= *<int>.*<int>
```
eg: `123.456`
### String Literal
```bnf
<string literal> ::= "*<char>"
```
eg: `"hello"`

### Char Literal
```bnf
<char literal> ::= '<char>'
```
eg: `'a'`

### Long Literal
```bnf
<long literal> ::= #<int literal>
```
eg: `#123`

### Identifier
```bnf
<identifier> ::= *<char>
```
eg: `foo`

### Compund Expression
```bnf
<compound expression> ::= <expression> <operator> <expression>
```
eg: `1 + 2 * 3`
Please note that compound expressions are evaluated recursively as follows.
`1 + 2 * 3 + 4 + 5` -> `1 + (2 * (3 + (4 * 5)))`

### Parenthetical Expression
```bnf
<parenthetical expression> ::= ( <expression> )
```
eg: `(1 + 2) * 3`

This can be used to override the recursion of compound expressions.

### Function Call
```bnf
<function call> ::= <identifier> ( <arguments> )
```
eg: `foo(1, 2)`

### Lambda Expression
```bnf
<lambda expression> ::= [<parameters>]=>{<function body>}
```
eg: `[int a, int b]=>{return a + b;}`

### Reference
```bnf
<reference> ::= ?<identifier>
```
eg: `?foo`
Returns the address of the identifier.

### Dereference
```bnf
<dereference> ::= <identifier> as <type>
```
eg: `foo as int`
Returns the value stored at the address. Casts the value to the type specified.

### Variable
```bnf
<variable> ::= <identifier>
```
eg: `foo`

### New
```bnf
<new> ::= new <type> ( <arguments> )
```
eg: `new OBJECT(1, 2)`
Dynamically allocates memory for an object of the specified type and calls the constructor with the arguments passed.  The parenthesis are optional. The type must be a class. The constructor must be a function with the name `init`.  The value returned is the address of the object.
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
    <class variable declarations>
    <class functions>
};
```
### Class Functions
Class functions are functions that are declared with the following syntax:
```c++
class <class name>{
    <return type> <function name>([<paramiters>]); // this is the function declaration it can be defined here with the regular 
                                                   // function syntax. Or it can be defined outside of the class via function
                                                   // scoping. 
};
``` 
- Function scoping allows functions to be defined outside of the class.  This is useful for separating implementation from declaration. syntax:
```c++
// scoped function
<return type> <function name>@<class name>([<paramiters>]){
    <function body>
};
```
- A class function automaticly creates a pointer to the object that called it.  It is stored in the my variable.  This is useful for functions that need to access the class variables.

## Contracts
Contracts are used to create OO interfaces. The allows classes that sign them to behave as the parent class.  The syntax is:
```c++
contract {
    <contract var declarations>
};
```
the contract veriables are automatically included in any class that signs the contract.
signing a contract is done with the following syntax:
```c++
class <class name> signs <contract name>{
    <contract var declarations>
    <class functions>
};
```
Here is an example of the intended use of a contract:
```c++
.needs <io>
.needs <std>

class IWorker{
    contract{
        adr work;
    };
};

class Plumber signs IWorker{
    // the init function should be the first function called to bootstrap the class
    int init(){
        my.work = []=>{ // this is the implementation of the work function as defined in the contract.  It can also be done using
                        // function pointer
            print("I am tightening the pipes\n");
        };
    };

    int getClients(){
        print("I am getting clients\n");
    };
};

Plumber newPlumber(){
    Plumber plumber = malloc(Plumber);
    plumber.init();
    return plumber;
};

class Carpenter signs IWorker{
    // the init function should be the first function called to bootstrap the class
    int init(){
        my.work = []=>{ // this is the implementation of the work function as defined in the contract.  It can also be done using
                        // function pointer
            print("I am building a house\n");
        };
    };

    int buyTools(){
        print("I am buying tools\n");
    };
};

Carpenter newCarpenter(){
    Carpenter carpenter = malloc(Carpenter);
    carpenter.init();
    return carpenter;
};

int doWork(IWorker worker){
    worker.work();
};

int main(){
    Plumber plumber = newPlumber();
    Carpenter carpenter = newCarpenter();

    plumber.init();
    carpenter.init();

    plumber.getClients();
    carpenter.buyTools();

    plumber.work();
    carpenter.work();

    doWork(plumber);
    doWork(carpenter);

    return 0;
};
```
output:
```c
I am tightening the pipes
I am building a house
I am getting clients
I am buying tools
I am tightening the pipes
I am building a house
```

- Keep in mind that when calling function pointer that are a part of a class, the first parameter is the pointer to the object. The function can be created with a veriable named my or self.

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

## Package Manager
The Aflat package manager is built into the compiler.

### Bootstrapping a Project
The package manager is used to create a project.  The syntax is:
```bash
aflat make <project name>
```
The project name is will be the name of the directory that will be created. It will create a head, src, and bin directory.  The head directory will contain the header files for the project.  The src directory will contain the source files for the project.  The bin directory will contain the compiled object files for the project.  It will also create an aflat.cfg file wich will contain settings for the compiler.

## Building a Project
The project can be built with the following syntax:
```bash
aflat build
```
This will compile all of the source files in the src directory and call gcc to link them into an executable.  The executable will be placed in the bin directory.

## Running a Project
The project can be run with the following syntax:
```bash
aflat run
```
This will compile all of the source files in the src directory and call gcc to link them into an executable.  The executable will be placed in the bin directory.  The executable will then be run.

## Bootstrapping a Module
The package manager is used to create a module.  The syntax is:
```bash
aflat add <module name>
```
This will create a header and source file in their respective directories.  It will also create and entry in the aflat.cfg file telling the compiler to compile the new source file.