# Documentation For AFlat Programming Language
## Trust the programmer philosophy
AFlat aims to trust that developers know what they are doing.  It allows and encourages things that many other modern languages do not, such as pointer asthmatic, memory management, and so on. While aflat does allow some developers to do some dangerous things, it also provides optional safety features that can be used to prevent some of the more dangerous things.  AFlat is not a language for everyone, but it is a language for careful developers who want to have more control over their code and their programs.
<br>

## Types
### int
A 4 byte signed int
```js
int i = 0;
```
### float
A 4 byte floating point value
```js
float f = 0.3;
```
### long
An 8 byte signed integer
```js
long l = 12;
```
### short
A 2 byte integer
```js
short s = 1;
```
### bool
A 1 byte bool
```js
bool b = false;
```
### adr
An 8 bit memory address.  In aflat, pointers point to any type so you can have a pointer to an int, a pointer to a float, and so on.  This is different from C where pointers point to a specific type.  This is done to allow for more flexibility in the language. Because of this, pointers in aflat are a bit dangerous. It is suggested to use boxing instead of pointers wherever possible.
```js
adr a = NULL;
```
### generic
The generic type is essentially the same as the adr type, but it is used to indicate that the pointer is meant to point to some object. This is meant to be used with a function or class that needs to preform operations on any sort of reference type object. The generic type also suppresses implicit casting, which is useful when you want to make sure that the pointer is pointing to the correct type.
```js
generic g = NULL;
```

### type inference
Aflat supports type inference at declaration with the `let` keyword. This should not be used when declaring a numeric type such as short, int, or long.  Because aflat returns type `number` from an int literal.  The number type is a union of all numeric types and will break your code if it is tied to a variable.  You should also take note that implicit casting will not happen when using type inference.

```js
let i = "hello"; // this will assign an adr to i
```

## User Defined Types...
Any Type Defined by a user will be an 8 bit memory reference to the given type
```js
Type a = new Type();
```
A user defined type can be cast to and from an adr or generic... be careful! This is not type safe!
```js
adr b = NULL;
Type a = b;
```

You will see more about this in the class section.
## Functions

### Syntax
Functions in AFlat are now defined with the following syntax:

```c
fn <function name>(<arguments>) -> <return type>? {
    <function body>
};
```

- The `fn` keyword is now used to define functions.
- Return types are optional; if omitted, they are inferred where possible.
- If a function has an optional return type (denoted with `?`), it is automatically wrapped in `Option`.
- If a function does not return a value with the `return` keyword, a return statement with no value is implied. In the case of `Option`, `return;` will return `none`.
- Functions are still limited to six arguments due to register-based argument passing.
- **The old syntax is still supported for backwards compatibility.**

#### Example:
```js
fn add(int a, int b) -> int {
    return a + b;
};
```

#### Optional Arguments
- **`*` (Nullable Argument):** An argument prefixed with `*` is considered nullable and defaults to `NULL` or a type-specific default.
- **`?` (Optional Argument):** An argument prefixed with `?` is considered optional and is automatically wrapped in `Option`.

Example:
```js
fn add(int a, *int b) -> int {
    return a + b; // b defaults to 0 if not provided
};

fn maybeDivide(int a, ?int b) -> int? {
    b.map({
        "some": [int x] => return a / x,
        "none": [] => return NULL
    })
};
```

### The `main` Function
The `main` function is the entry point for AFlat programs. It can optionally take `argc` and `argv` for command-line arguments.

```js
fn main(int argc, adr argv) -> int {
    // do stuff
    return 0;
};
```

It can also be defined without parameters if command-line arguments are not needed:
```js
fn main() -> int {
    // do stuff
    return 0;
};
```

### Function Pointers
The name of a function without parentheses returns a pointer to the function, allowing functions to be passed as arguments.

```js
fn add(int a, int b) -> int {
    return a + b;
};

fn main() -> int {
    adr foo = add;
    int i = foo(1, 2);
    return 0;
};
```

### Calling Functions
Functions are called using the following syntax:
```c
<function name>(<arguments>);
```
- The AFlat compiler checks argument count and types for function calls, but not for function pointers.
- Function pointers can be called using the same syntax.

Example:
```js
fn add(int a, int b) -> int {
    return a + b;
};

fn main() -> int {
    int i = add(1, 2);
    return 0;
};
```

### Anonymous Functions
Anonymous functions allow defining functions inline without a name. The syntax follows:
```js
[<parameters>] => {
    <function body>
};
```
- Type inference is **not** performed on anonymous functions; they assume the developer knows the expected return type.

Example:
```js
fn main() -> int {
    adr add = [int a, int b] => {
        return a + b;
    };

    int i = add(1, 2);
    return 0;
};
```

### Function Access
Functions at the global scope can have different access levels:
- **Public** (default) – Globally accessible.
- **Private** – Accessible only within the module.
- **Export** – Explicitly imported for use in another module to avoid conflicts.

### Function Decorators
Functions can be decorated using decorators, which must take a single reference argument `_arg`. The decorator function must accept two arguments: `adr foo` (the function) and `adr _arg`.

Example:
```js
adr decorator(adr foo, adr _arg) {
    io.print("Hello from Decorator");
    return foo(_arg);
};

fn decorated(adr _arg) : decorator {
    io.print("Hello from Decorated");
    return 0;
};

fn main() -> int {
    decorated();
    return 0;
};
```

**Output:**
```
Hello from Decorator
Hello from Decorated
```

Class decorators are also supported and are documented in the class section.


## Statements

### Declarations
Declarations are used to define variables.  They are used to define variables and functions.
```bnf
<declaration> ::= <type> <identifier>;
```
### Declare and assign
```bnf
<declaration> ::= <type or let> <identifier> = <expression>;
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

### import
```bnf
<import> ::= import {<function>, <function>} from "path" under <ident>
            | import <Class>, <Class> from "path";
            | import * from "path" under <ident>; // try to avoid this
```
Import functions or classes from modules;
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
The string literal returns an adr. like a char * in C.

### Not expr
```bnf
<not Expr> ::= !<expr>
```
eg: `!(a == b)`

### Char Literal
```bnf
<char literal> ::= '<char>'
```
eg: `'a'`


### Identifier
```bnf
<identifier> ::= *<char>
```
eg: `foo`

### Compound Expression
```bnf
<compound expression> ::= <expression> <operator> <expression>
```
eg: `1 + 2 * 3`

### Parenthetical Expression
```bnf
<parenthetical expression> ::= ( <expression> )
```
eg: `(1 + 2) * 3`

This can be used to override the order of operations.

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
### Notes
Note that currly braces are optional when the body is a singal statment;
### If Statements
If statements are used to execute code based on a condition.  The syntax is:
```c
if <expression> {
    <code to execute if condition is true>
};
```
- The condition is evaluated before the code is executed.  If the condition is true, the code is executed. Condition must be a bool

- Boolean conditional operators are
    - `==`
    - `!=`
    - `>`
    - `>=`
    - `<`
    - `<=`


- Logical operators are.  all logical operators are handled operators aflat does not have logical operators, the bitwise operators are used instead.
    - | or
    - & and

if statements can be used with else statements.  The syntax is:
```c
if <expr> {
    <code to execute if condition is true>
}else{
    <code to execute if condition is false>
};
```
### While Loops
While loops are used to execute code while a condition is true.  The syntax is:
```c
while <expr> {
    <code to execute>
};
```
### For Loops
For loops are used to execute code a set number of times.  The syntax is:
```c
for <initializer> <boolean expr> <iterator>{
    <body>
};
```

example:
```c
.needs <io>

int main(){
    for int i = 0; i < 10 i = i + 1
        printInt(i);
    return 0;
};
```

## Classes
Classes in aflat are effectively structs that can implement functions and support encapsulation and rudimentary inheritance.  The syntax is:
```js
class <class name> signs <parent class>{
    <contract>
    <class variable declarations>
    <class functions>
};
```
### Class Functions
Class functions are functions that are declared with the following syntax:
```js
class <class name>{
    <return type> <function name>([<paramiters>]); // this is the function declaration it can be defined
                                                   //with the regular 
                                                   // function syntax. Or it can be defined outside of the class via function
                                                   // scoping. 
};
``` 
- Function scoping allows functions to be defined outside of the class.  This is useful for separating implementation from declaration. syntax:
```js
// scoped function
<return type> <function name>@<class name>([<paramiters>]){
    <function body>
};
```
- A class function automaticly creates a pointer to the object that called it.  It is stored in the my variable.  This is useful for functions that need to access the class variables.

### Constructor
the constructor is a special function that is called when an object is created. The constructor must have the name `init`.  The syntax is:
```js
class <class name>{
    <class name> init([<paramiters>]) {
        <function body>
        return my; // if my is not returned, the compiler wil assume that the constructor returns the address of the object (my).
    };
};
```
### Class fields
Class fields are variables that are declared in the class definition.  They can be accessed from within the class by using the `my` reference.  The syntax is:
```js
class <class name>{
    <mutability> <acess modifier> <type> <field name> = <initial value>;
};
```
#### access modifiers
If an initial value is specified, an assignment is added to the top of the constructor.

The access modifier is used to determine the visibility of the field.  The following are valid access modifiers:
    - `public` : the field is visible to all classes.
    - `private` : the field is visible only to the class that defines it.

### Class Decorators
Class decorators are used to turn a function into an instance of a class.  It is syntactic sugar for passing a function pointer to the constructor.  The syntax is:
```js
class Decorator {
    const adr foo = foo;
    Decorator init(adr foo){
        return my;
    };

    int runFoo(){
        adr foo = my.foo;
        return foo();
    };
};

class HasDecoratedFunction {
    
    int decorated() : Decorator {
        io.print("decorated");
    };

    HasDecoratedFunction init(){
        return my;
    };
};

int main(){
    HasDecoratedFunction hdf = new HasDecoratedFunction();
    hdf.decorated.runFoo(); // decorated is not a function, it is an instance of the Decorator class.
    return 0;
}
```
## Contracts
Contracts are used to create OO interfaces. The allows classes that sign them to behave as the parent class.  The syntax is:
```js
contract {
    <contract var declarations>
};
```
the contract veriables are automatically included in any class that signs the contract.
signing a contract is done with the following syntax:
```js
class <class name> signs <contract name>{
    <contract var declarations>
    <class functions>
};
```
Here is an example of the intended use of a contract:
```js
.needs <std>

import {print} from "io" under io;


class IWorker{
    contract {
        adr work = [] => io.print("Generic worker working...\n");
    };
};

class Plumber signs IWorker{
    // the init function should be the first function called to bootstrap the class
    int init(){
        my.work = []=>{ // this is the implementation of the work function as defined in the contract.  It can also be done using
                        // function pointer
            io.print("Plumber: I am tightening the pipes\n");
        };
    };

    int getClients(){
        io.print("Plumber: I am getting clients\n");
    };
};

class Carpenter signs IWorker{
    // the init function should be the first function called to bootstrap the class
    int init(){
        my.work = []=>{ // this is the implementation of the work function as defined in the contract.  It can also be done using
                        // function pointer
            io.print("Carpenter: I am building a house\n");
        };
    };

    int buyTools(){
        io.print("Carpenter: I am buying tools\n");
    };
};

int doWork(IWorker worker){
    worker.work();
};

int main(){
    Plumber plumber = Plumber();
    Carpenter carpenter = Carpenter();
    IWorker generic = IWorker();

    plumber.getClients();
    carpenter.buyTools();

    plumber.work();
    carpenter.work();
    generic.work();

    doWork(plumber);
    doWork(carpenter);
    doWork(generic);
    return 0;
};
```
output:
```
Plumber: I am getting clients
Carpenter: I am buying tools
Plumber: I am tightening the pipes
Carpenter: I am building a house
Generic worker working...
Plumber: I am tightening the pipes
Carpenter: I am building a house
Generic worker working...
```

- Keep in mind that when calling function pointer that are a part of a class, the first parameter is the pointer to the object. The function can be created with a variable named my or self.

### Class Modifiers
There are two class modifiers currently implemented: `safe` and `dynamic`

#### safe
A safe class cannot be passed as an argument to a function or returned from a function as an l value.  This is useful for keeping track of object ownership.  The syntax is:
```js
safe class <class name> signs <parent class>{
    <contract>
    <class variable declarations>
    <class functions>
};
```
if there is a get() method defined in the class, that method will be implicitly called when attempting to access a safe object.

#### dynamic
A dynamic class MUST be instantiated on the heap with the new keyword.  If implicit casting is used, it will default to declaring on the heap. The syntax is:

```js
dynamic class <class name> signs <parent class>{
    <contract>
    <class variable declarations>
    <class functions>
};
```

### The object life cycle
When an object is created, the following steps are taken:
1. The object is allocated on the heap or the stack depending on if new was used.
2. The init function is called if present. (All initial value assignments are prepended onto the init function)

When an object goes out of scope, the following steps are taken:
1. The endScope function is called if present.

When an object is deleted, the following steps are taken:
1. The destructor function is called if present.
2. The memory for the function is freed.


## Working with header files
Much like in c or c++, aflat supports a header and source file interface; The header file contains the function and class definitions.  The source file contains the implementation of the functions and classes.  Header files should have the extension '.gs' and source files should have the extension '.af'.

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

The list of standard headers are as follows:
- math
    - Handles math functions
- std
    - Deals with memory managment and time. Also carries the assert function
- asm
    - a repository of syscall wrappers


example of using the std header file:
```js
.needs <std> // the std module is included

int main(){
    assert(1==1, "failed common assert"); // the std module contains the assert function
    return 0;
};
```

## Working with Modules
Modules are the prefered method of sharing code over header files.

### Creating a Module
A module is only a .af source file with no header.  The module can export functions and classes that can be used by other modules.
#### Exporting a function
A function is exported using the `export` modifier once a function is exported it cannot be accessed via header file.

Example:
```js
export int add(int a, int b){
    return a + b;
};
```

#### Exporting a Class
Classes are automaticaly exported by a module.

### Importing from a module
Importing is done with the import statment.  If there is no "./" in the path given, aflat searches in the standard library directory for the statment.  Otherwise the aflat searches the working directory. .af is automaticly appened to modual names if it is not there.

#### Importing functions
Functions are imported inside of curly braces. All functions can be imported using the * oporator. Functions are imported under a namespace defined using the `under` keyword.  The namespace must be used to access the function.

example:
```js
import {add, sub} from "./src/Mod" under mod;

int foo(){
    int a = mod.sub(5, 6);
    return mod.add(a, 3);
};
```

#### Importing classes
Classes are imported outside of curly braces. No namespace is needed for classes.
example:
```js
import Player from "./src/GameEngin";

int game() {
    Player p1 = Player();
};
```
note -- if a class signs a contract, the base class must be imported before the child.


The list of standard modules is as follows:
- Collections
    - Handles arrays and list and the ICollection interface
- concurrency
    - Handles threads and pipes defines the Process and MProcess classes
- DateTime
    - Provides the DateTime class
- files
    - Handles file IO and defines the File class
- io
    - Handles input and output to the console
- strings
    - Functions to deal with strings and convert between other types and strings
- String
    - provides the Standard string object
- ATest
  - Provides the testing framework for Aflat




Example:
```js
.needs <std>

import {print} from "io" under io;

int main(){
    io.print("Hello World!\n");
};

```
## Package Manager
The Aflat package manager is built into the compiler.

### Bootstrapping a Project
The package manager is used to create a project.  The syntax is:
```bash
aflat make <project name>
```
The project name is will be the name of the directory that will be created. It will create a head, src, and bin directory.  The head directory will contain the header files for the project.  The src directory will contain the source files for the project.  The bin directory will contain the compiled object files for the project.  It will also create an aflat.cfg file which will contain settings for the compiler.

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

## Bootstrapping a src/header pair (just don't use this)
The package manager is used to create a src/header pair.  The syntax is:
```bash
aflat add <src/header name>
```
This will create a header and source file in their respective directories.  It will also create and entry in the aflat.cfg file telling the compiler to compile the new source file.

## Bootstrapping a module
The package manager is used to create a module.  The syntax is:
```bash
aflat module <module name>
```
This will create a source file in the src directory.  It will also create and entry in the aflat.cfg file telling the compiler to compile the new source file.
## Ownership, References, and Selling
AFlat uses explicit ownership. Objects live until they go out of scope or are deleted. Class objects are passed by reference automatically.

Example of references:
```aflat
class Data {
    mutable int value = 10;
};

fn modify(Data d) -> void {
    d.value = 5;
};
```

Use `&` to take references and `$` to transfer ownership:
```aflat
fn take(mutable int &&x) -> void {
    print(x);
};

fn main() -> int {
    int a = 10;
    take($a); // a is moved
    return 0;
};
```

