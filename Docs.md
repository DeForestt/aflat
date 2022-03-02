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