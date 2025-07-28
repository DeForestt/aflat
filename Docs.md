# Documentation For AFlat Programming Language
## Trust the programmer philosophy
AFlat aims to trust that developers know what they are doing.  It allows and encourages things that many other modern languages do not, such as pointer asthmatic, memory management, and so on. While aflat does allow some developers to do some dangerous things, it also provides optional safety features that can be used to prevent some of the more dangerous things.  AFlat is not a language for everyone, but it is a language for careful developers who want to have more control over their code and their programs.
<br>

### Development Best Practices
* Build the compiler with CMake (`mkdir build && cd build && cmake .. && make`).
* Rebuild the standard library with `./rebuild-libs.sh` after editing `libraries/std/src`.
* Format C++ code using clang-format.
* Run `./bin/aflat run` to execute tests whenever you change the source.

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
- If a function has an optional return type (denoted with `?`), it is sugar for
  returning `option<T>`.
- Prefixing an argument with `?` likewise treats it as `option<T>` and allows the
  caller to omit the argument entirely, in which case `None()` is passed.
- The returned `option` is created automatically: returning a value yields `Some(value)` while `return;` (or falling off the end) yields `None()`.
- If a function does not return a value with the `return` keyword, a return statement with no value is implied. In the case of `option`, `return;` will return `None()`.
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
 - **`?` (Optional Argument):** An argument prefixed with `?` is considered optional and is automatically wrapped in `option`.
 -   When calling such a function you may omit the argument to pass `None()`. Inside
    the function the parameter type is `option<T>`.

Example:
```js
fn add(int a, *int b) -> int {
    return a + b; // b defaults to 0 if not provided
};

fn maybeDivide(int a, ?int b) -> int? {
    match b {
        Some(x) => return a / x,
        None() => return;
    };
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
<import> ::= import <item>, <item> from "path" [under <ident>]

<item>   ::= {<function>, <function>}
           | <Class>
           | *
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

### Foreach Loops
`foreach` iterates over any object that implements a `next()` method returning
`option`. The loop binds each yielded value to an identifier and executes the
body until `None` is returned. The syntax is:

```c
foreach <ident> in <iterator> {
    <body>
};
```

Example:

```c
let nums = [1, 2, 3];
foreach n in nums {
    printInt(n);
};
```

### Match Statements
`match` inspects a union and executes one arm based on the active alias. Each
arm specifies an alias and may bind the alias payload to a variable. The result
of the chosen arm becomes the value of the entire statement.

```c
match <union expr> {
    Alias1(x) => {
        // use x
    },
    Alias2 => {
        // handle Alias2 which has no payload
    }
};
```

All aliases of the union must be accounted for or a compile-time error is
emitted. Because `match` is an expression, it can be directly returned or stored
in a variable.

### List Literals
AFlat provides bracket syntax to construct a `vector` from values. The type of
the resulting list is inferred from the first element or from the assignment
target.

```c
let names = ["Tom", "Sue", "Bob"];
vector::<int> numbers = [1, 2, 3];
```

Empty lists require an explicit target type:

```c
vector::<string> words = [];
```

The literal expands to creating a `vector` instance and pushing each element in
order.

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

### Safe methods
Use the `safe` keyword on a class method when it does not modify `my`. Only
`safe` methods may be invoked on an `immutable` instance. Attempting to call a
non-safe method on an immutable object is a compile-time error.

```aflat
class Counter {
    mutable int value = 0;

    safe fn get() -> int {
        return my.value;
    };

    fn inc() -> void {
        my.value = my.value + 1;
    };
};

fn main() -> int {
    immutable Counter c = new Counter();
    c.inc();      // error: cannot call non-safe method on immutable object
    return c.get(); // ok
};
```

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

## Templates
Templates allow classes and functions to operate on any number of types.
Declare template parameters using the `types` keyword. List as many type names
as you need inside the parentheses separated by commas. When the argument
types fully determine the template parameters, you may omit `::<...>` and let
the compiler infer them. If the compiler cannot infer the types, specify them
explicitly with `::<Type>`.

Example:
```aflat
types(T)
fn echo(T value) {
    str.print(`{value}\n`);
};

fn main() {
    echo(5);        // T inferred as int
    echo("hi");     // T inferred as string
    echo::<bool>(true); // explicit when needed
};
```

You may list multiple template types:

```aflat
types(Key, Value)
class Pair {
    Key key = key;
    Value value = value;
};

fn main() {
    let p = Pair::<int, string>(1, "one");
};
```

### Template Classes
A class can also be templated. Provide the `types` line before the class and
specify the concrete type when constructing. Unlike template functions, class
templates do **not** support type inference, so you must always supply the
concrete type using `::<Type>` when creating an instance.

```aflat
types(A)
class Box {
    mutable A value = value;

    fn init(A value) -> Self {
        return my;
    };

    fn get() -> A {
        return my.value;
    };
};

fn main() {
    let b = Box::<int>(5);
    let s = Box::<string>("hi");
    str.print(`{b.get()} {s.get()}\n`);
};
```

### When Clauses
Use a `when` clause to conditionally compile a function or method based on
template parameters. A clause consists of predicates combined with `and` inside
parentheses. Supported predicates are:

- `T is <trait>` / `T is not <trait>`
- `T has <method>` / `T missing <method>`

The item is generated only if all predicates are satisfied.

Example:

```c
types(T)
when (T is dynamic and T has toString)
fn printAll(vector::<T> vals) {
    foreach v in vals {
        str.print(v.toString());
    };
};
```

## Transforms
`transform` provides a macro-like facility to generate code at compile time. A
transform is declared using `transform <name>` followed by a template between
`~` delimiters:

```aflat
transform observable
~
${scope} ${mut} ${type} __${ident} = ${expr};
fn ${ident}() -> Map : Property {
    "get": [const ${Self} this] => {
        return this.__${ident};
    },
    "set": [const ${Self} this, const ${type} x] => {
        this.__${ident} = x;
        return;
    }
};
~;
```

Placeholders like `${ident}`, `${type}`, and `${expr}` are substituted when the
transform is applied. Annotate a declaration with `@Name` to expand the template
using that declaration's identifier, type and expression. Any arguments supplied
to the annotation can be referenced as `${arg}` within the template.

Transforms are parsed into normal AFlat code before compilation, allowing users
to build reusable patterns such as property helpers.

## Unions
Unions define a single type that can represent several *aliases*. An alias may
wrap a value, represent a constant, or carry no data at all. They are useful when
a variable must hold values of different shapes but only one at a time.

```c
union Value {
    Int(int),         // alias carrying an int
    Float(float),     // alias carrying a float
    Text(string),     // another typed alias
    None,             // empty alias with no payload
    ErrorCode = 404   // alias bound to a constant expression
};
```

The compiler allocates enough space for the largest alias plus a four byte tag
indicating which alias is active. After the alias list you may define methods
just like a class body.

### Constructing a Union
Create a value with `new <Union>-><Alias>(args)` or by calling helper functions
if the module provides them.

```c
Value v1 = new Value->Int(42);
Value v2 = new Value->Text("hi");
Value none = new Value->None();
```

### Matching on a Union
Use `match` to perform different logic based on the active alias. The payload of
an alias can be bound to a variable inside the case.

```c
match v1 {
    Int(i) => printInt(i),
    Float(f) => printFloat(f),
    Text(t) => str.print(t),
    None() => {},
    ErrorCode => print("error")
};
```

The expression evaluates to whatever the executed case returns, allowing it to
be embedded in other expressions.
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

#### Importing classes and functions together
AFlat now allows mixing both forms in a single statement.
example:
```js
import Player, {spawn} from "./src/GameEngin" under game;
```


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
  - Provides the testing framework for AFlat
  - `beforeEach` registers a callback to run before each `it` block
  - Use `describe` to group tests and `it` for individual cases
  - Fixtures can be defined with `fix` and accessed via `getFixture`
  - `getFixture` returns an untyped `any` and its result must be stored in a
    variable with an explicit type
  - Fixtures are torn down at the end of every `it` block so teardown functions
    can manage resources
  - Assertion helpers include `assertEqual`, `assertNotEqual`, `assertTrue`,
    `assertFalse`, `assertNull`, `assertNotNull`, `assertError`, and
    `assertSuccess`
- Utils/result
  - A templated result type used for error handling
- CLArgs
  - Parser for command line options and flags

Example:
```js
.needs <std>
import {describe, it, beforeEach, fix, getFixture,
        assertEqual, assertTrue, assertNotNull} from "ATest" under test;

fn main() -> int {
    test.fix("counter", fn () -> int { return 5; }, fn (int v) {
        print(`clean {v}`);
    });

    test.beforeEach(fn () { print("setup"); });

    test.describe("Numbers", fn () -> bool {
        test.it("fixture works", fn () {
            const int c = test.getFixture("counter");
            test.assertNotNull(c);
            test.assertEqual(c, 5);
        });
        test.it("math works", fn () {
            test.assertTrue(1 + 1 == 2);
        });
        return true;
    });
    return 0;
};

```

### Utils/result.af
The `result` module defines a templated `result` union used for error handling.
Use `accept` to create a success and `reject` for an error. Call `match` on the
result to access the value or handle the error.

`result` supersedes the older `Result` class. `Result` stored its payload as
`any` and relied on reference counting, which introduced extra allocations and
loss of type information. The new union keeps the success value typed via
`Ok(T)` and errors via `Err(Error)`. This allows compile‑time checks and avoids
the overhead of dynamic dispatch. The trade‑off is that you must pattern match
on the value and manage ownership yourself rather than relying on the class to
clean up automatically.

#### `!` return shorthand

Functions that return a `result` can use `T!` as a shorthand for
`result::<T>`. Returning a bare value implicitly wraps it in `accept`, while
errors are returned with `reject`.

Example:

```js
.needs <std>
import {print} from "String" under str;
import string from "String";
import Error from "Utils/Error";
import {Some, None} from "Utils/option";
import result from "Utils/result";
import {accept, reject} from "Utils/result" under res;

fn divide(int a, ?int b) -> int! {
    match b {
        Some(val) => return a / val,
        None() => return res.reject::<int>(new Error("Divide by 0"))
    };
};

fn main() -> int {
    let d = divide(10);
    match d {
        Ok(v) => str.print(`Returned value: {v}\n`),
        Err(e) => str.print(`Error got an error: {e}\n`)
    };
    return 0;
};
```

This style lets errors bubble up without breaking control flow—similar to an
exception system but without unwinding the stack.

#### Bubble operator `!`

When working with functions that return a `result`, the `!` bubble operator can
be appended to an expression to automatically propagate any error. It expands to
matching on the `result` and returning an error early if one is encountered.

Example:

```js
fn divideString(string a, string b) -> int! {
    let aInt = a.toInt()!; // bubble an error if parsing fails
    let bInt = b.toInt()!;
    return divide(aInt, bInt);
};
```

This is equivalent to:

```js
fn divideString(string a, string b) -> int! {
    let aIntRes = a.toInt();
    match aIntRes {
        Ok(val) => aIntRes = val,
        Err(e) => return e,
    };
    let bIntRes = b.toInt();
    match bIntRes {
        Ok(val) => bIntRes = val,
        Err(e) => return e,
    };
    return divide(aIntRes, bIntRes);
};
```

Using `!` keeps code concise and clearly signals that any encountered error will
immediately return from the surrounding function. However, it should not be
overused. Reserve the operator for helper functions or short routines where
propagating errors is expected. In other situations prefer explicit error
handling so callers can see when failures might occur.

### Utils/option.af
The `option` union is the standard way to express an optional value. It has two
aliases: `Some(T)` containing a value and `None` for the empty state. Helper
functions `Some(value)` and `None()` construct the respective aliases.

```c
types(T)
union option {
    Some(T),
    None
};
```

Common methods are:

- `isSome()` / `isNone()` – query the state
- `expect(msg)` – return the contained value or abort with a message
- `or(default)` – return the value or a default
- `toString()` – for debugging

Example usage:

```c
fn index(int[] arr, int idx) -> int? {
    if (idx >= arr.length) {
        return;       // yields None()
    }
    return arr[idx]; // yields Some(value)
};

let res = index(nums, 2);
match res {
    Some(v) => printInt(v),
    None() => print("out of range")
};
```

Pattern matching with `match` is the most convenient way to operate on an
`option` value.

## Package Manager

### Bootstrapping a Project
The package manager is used to create a project.  The syntax is:
```bash
aflat make <project name>
```
Add `--lib` before `make` to generate a library template without a `main` file.
The project name is will be the name of the directory that will be created. It will create a head, src, and bin directory.  The head directory will contain the header files for the project.  The src directory will contain the source files for the project.  The bin directory will contain the compiled object files for the project.  It will also create an `aflat.cfg` file using an INI format for build settings and dependencies.

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
This will create a header and source file in their respective directories.  It will also create an entry in the `[dependencies]` section of `aflat.cfg` so the new source file is built.

## Bootstrapping a module
The package manager is used to create a module.  The syntax is:
```bash
aflat module <module name>
```
This creates a directory under `src/` matching the module name and places
`mod.af` inside. An entry is added under `[dependencies]` in `aflat.cfg` so the
new module is compiled. For a single `.af` file without a folder, use
`aflat file <name>` instead.

# \ud83d\udcd8 AFlat Ownership Model

AFlat uses an explicit ownership model for managing memory and ensuring correctness in dynamic allocations. It is designed to strike a balance between **performance**, **safety**, and **control**, while still adhering to AFlat\u2019s philosophy of trusting the programmer.

This model **only applies to non-value types** \u2014 i.e., user-defined classes and heap-allocated objects. Value types like `int`, `float`, etc., are always passed and copied by value and are excluded from ownership checks.

---

## \ud83d\udd11 Core Principles

1. **Ownership implies responsibility**
   If you own a value, you are responsible for its lifetime (e.g., freeing it or transferring it).

2. **You cannot move or return what you don\u2019t own**
   The compiler enforces that only owned values may be moved (sold) or returned.

3. **Ownership only applies to heap-allocated memory**
   Stack variables, literals, and internal references cannot be owned.

---

## \ud83d\udcdc Ownership Rules

### 1. You can only sell things you own

* The `$` operator transfers ownership.
* Attempting to `$` a non-owned variable is a compile-time error.

### 2. You can only own heap-allocated values

* Only objects allocated with `new`, returned from functions that yield ownership, or passed via `&&` are considered "owned."
* Stack values and function-local variables are not ownable.

### 3. You can only return things you own

* Returning a non-owned reference violates ownership.
* Functions must return owned values or wrap borrows in safe containers (e.g., `option`, `result`, etc.).

### 4. The return value of a `CallExpr` is owned

* When calling a function that returns an owned object, the return value is treated as a fresh owned value (e.g., `fn makeFoo() -> Foo!`).

### 5. Anything created with `new` is owned

* `new` always produces a heap-allocated, owned object.
* You do **not** need to use `$` when passing `new Foo()` to a `&&` parameter, since it is an rvalue.

### 6. Arguments marked with `&&` take ownership

* Functions expecting a `&&` parameter require the caller to transfer ownership.
* Example:

  ```aflat
  fn takeIt(Foo &&f) { ... }

  let f = new Foo();
  takeIt($f); // legal
  ```

### 7. Anything else is not owned

* Regular variables, references, and function parameters passed by value are non-owning by default.
* You cannot `$` or return these without wrapping them or copying explicitly.

### 8. Accessing a field of an owned object yields ownership of that field

* If you own an object, you can legally move out of its fields.
* This allows:

  ```aflat
  let obj = new Container();
  let inner = $obj.field; // legal: obj is owned
  ```

> \u26a0\ufe0f This creates *double ownership* (parent and field both considered owners), and the compiler does **not** track invalidation. It's the programmer\u2019s responsibility to avoid use-after-move bugs.

---

## \ud83e\uddea Special Cases and Clarifications

* **Selling (`$`) is only required on variables**, not rvalues:

  ```aflat
  takeIt(new Foo());   // OK \u2014 rvalue is owned
  takeIt($myFoo);      // OK \u2014 transfer ownership
  takeIt(myFoo);       // \u274c Error \u2014 ownership not transferred
  ```

* **Returning fields directly from a method** is currently treated as an ownership transfer.
  Future plans include treating such field returns as **borrows** unless explicitly marked otherwise.

---

## \u2705 Ownership Summary Table

| Expression                   | Owned? | Requires `$`? | Notes                                  |
| ---------------------------- | ------ | ------------- | -------------------------------------- |
| `new Foo()`                  | \u2705 Yes  | \u274c No          | Owned rvalue                           |
| `let f = new Foo();`         | \u2705 Yes  | \u2705 Yes         | Named variable \u2014 must `$f` to move     |
| `someFunc()` returning `T!`  | \u2705 Yes  | \u274c No          | Functions can transfer ownership       |
| `field` from owned object    | \u2705 Yes  | \u2705 Yes         | You can move fields if parent is owned |
| `field` from borrowed object | \u274c No   | \u274c Error       | Cannot move out                        |
| Stack or literal value       | \u274c No   | \u274c N/A         | Not tracked by ownership model         |
| Function param (by value)    | \u274c No   | \u274c Error       | Must use `&&` to pass ownership        |
| Function param (with `&&`)   | \u2705 Yes  | \u2705 Yes or \u274c No | `$var` or `new Foo()` is valid         |
| Return `t` (not owned)       | \u274c No   | \u274c Error       | Must return only owned values          |

---

## \ud83e\udd14 Design Benefits

* \u2705 **Clear semantics**: Ownership rules are easy to reason about.
* \u2705 **No runtime overhead**: All checks are compile-time.
* \u2705 **Opt-in safety**: Works well with AFlat's philosophy of \u201ctrust the programmer.\u201d
* \u2705 **Extendable**: Lays groundwork for future borrow tracking and lifetimes.

---
