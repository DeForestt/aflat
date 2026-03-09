# First Steps with AFlat

## Installation
***Remember!!! If you already Insalled aflat, remember to update the repository on your system***<br>
Please see the [installation instructions](README.md) for the steps to download and build.  If aflat is already installed, then you can move on the the next section. **This Tutorial tracks the latest version of aflat**

To update aflat run the following command

```bash
aflat update
```
You should see a bunch of output

### Development Best Practices
When building the compiler from source use CMake:
```bash
mkdir build && cd build
cmake .. && make
```
Format any C++ changes with clang-format and run `./rebuild-libs.sh` after editing `libraries/std/src`.
Run `./bin/aflat run` to verify everything still works.

## First Project
To create the tutorial project, run the following command in your project directory.
```bash
aflat make tutorial
```
Use `--lib` before `make` if you want a library project instead of an executable.

The tutorial project will be created in the `tutorial` directory.  It includes
an `aflat.cfg` file that tells the build system what to compile and link.  The
project also contains a `src/main.af` entry point and a `src/test/test.af` file
used by the `aflat test` command.

### Adding dependencies
List additional modules under `[dependencies]` in `aflat.cfg`. For external
repositories run:

```bash
aflat install <git_repo>
```

This clones the repository and updates the configuration automatically.

Open up the `main.af` file in your favorite text editor.  The file will look something like this:
```js
.needs <std>
import * from "io" under io;

fn main() -> int {
    io.print("Hello, world!\n");
    return 0;
};
```

Lets walk through the program line by line.
- The first line is the `.needs` directive.  This tells aflat that it needs the standard library.  The standard library is a collection of useful functions that are included in the aflat distribution.  The standard library is included in the `aflat/libraries/std/head` directory.
- The second line is the `import * from "io" under io;` command.  This tells aflat to import the `io` library from the `aflat/libraries/std/src` directory.  The `io` library is a collection of functions that are used to read and write to the standard input and output.  The `io` library is included in the `aflat/libraries/std/src` directory.  The under command tells aflat that the `io` library should be imported under the name `io` any functions called from this library must start with `io.`.
- the fourth line defines the main function.  This is the function that is called when the program is run.
- The fifth line calls the `io.print` function.  This function prints the string `Hello, world!` to the standard output.
- The sixth line returns 0.  This is the value that is returned to the operating system when the program is run.

Now type `aflat run` in the terminal to run the program.  The program will print `Hello, world!` to the standard output.

## Making it your own
Now we will make a few edits to the project.  First, it would be nice for the would to know who the "Hello" is from.  We do this by adding a new call to the `io.print` function. Give it a try and then look at our implementation below. Remember that \n represents a newline.

```js
.needs <std>
import * from "io" under io;

fn main() -> int {
    io.print("Hello, world!\n");
    io.print("From, The aflat Team\n");
    return 0;
};
```

## Working with strings
When you show your program to your friends, it will be nice to have the program print out their name. Take a look at the code below.

```js
.needs <std>
import string from "String";
import { readString, print } from "String" under str;

fn main() -> int {
	let name = str.readString("What is your name? ");
	str.print(`Hello, {name}.\nFrom the AFlat Team.\n`);
	return 0;
};
```

Lets walk take a look at the new lines.
- The first new line is the `import string from "String";` command.  This tells AFlat to import the `string` data type from the `String` library.
- The second new line is the `import { readString, print } from "String" under str;` command.  This tells AFlat to import the `readString` function from the `String` library.  The under command tells AFlat that the `readString` function should be imported under the name `str`.  Any functions called from this library must start with `str.`. It also imports the `print` function under the same namespace. Importing functions by name is recommended rather than importing * which can sometimes lead to undefined behavior (this is a bug that we are working on fixing).
- The third new line is the `let name = str.readString("What is your name? ");` command.  This tells AFlat to create a new variable called `name` and assign it the value returned by the `str.readString` function. The `let` keyword allows AFlat to infer the type of the new variable. The `str.readString` function takes an optional string as an argument and returns a string.  The string that is passed to the `str.readString` function is the prompt that is printed to the standard output.
- The last new line is the ``str.print(`Hello, {name}.\nFrom the AFlat Team.\n`);`` command.  This tells AFlat to call the `str.print` function.  The `str.print` function takes a string as an argument and prints it to the standard output.  The string that is passed to the `str.print` function is the string that is printed to the standard output.  The `{name}` is a placeholder for the value of the `name` variable.  The `\n` is a newline character.
- Run the program with `aflat run` and see what happens.

## If else statements
I take it back.  We don't want other people to see our program... in fact, we want to make sure that the program is only run if the user enters the correct name.  We can do this by adding an if else statement.

```js
.needs <std>

import string from "String";
import { readString, print } from "String" under str;

fn main() -> int {
	let name = str.readString("What is your name? ");

	if name == "Name" {
		str.print(`Hello, {name}. \nFrom the AFlat Team.\n`);
	} else {
		str.print("I don't know you... \nGo away!\n");
	};
	return 0;
};
```

The if statment will check if the two strings are equal. If they are equal, then the program statement will be executed.  If they are not equal, then the else statement will be executed.  Keep in mind that the else statement is optional. Alsoinit Keep in mind that if the if statement is only one line, then the curly braces are optional.


## Guessing Game
Now we will make a simple guessing game.  The program will ask the user to guess a number.  If the user guesses the number, the program will tell them they won.  If the user guesses the wrong number, the program will tell them  too high or too low. Explinations will be found in the comments.

```js
.needs <std>
import string from "String";
import { readString, print } from "String" under str;
import Error from "Utils/Error"; // we need to import the Error type to use it in the match call for error handling
import result from "Utils/result"; // we need to import the Result type to use it in the match call for error handling
import { accept, reject } from "Utils/result"; // result expects these two functions to be imported

fn main() -> int {
		// here we will specify the type of the variable instead of letting the compiler infer it... this is suggested when using numbers that could be int, long, or short.
	int secret = 42; // one of the most common types of variables in aflat is an int.  This stores an integer. 42 as the answer.

	let guess = str.readString("Enter your guess: "); // get the string that the user entered
	let guessInt = match guess.toInt() {
		Ok(i) => i, // if the conversion is successful, return the int value
		Err(err) => panic(`Error converting string to int: {err}`.cstr()) // if the conversion fails, panic with an error message.
	};

	if guessInt == secret // now that we have the same type we can compare them with the '==' operator
		str.print("You guessed it!\n")
	else if guessInt < secret // we can check if the guess is too low with the '<' operator
		str.print("Too low!\n")
	else
		str.print("Too high!\n");

	// notice that the curly braces are not needed in the if statement.  The curly braces are only needed when there is a block of code to be executed.

	return 0;
};
```

## Loops
This game is a bit too hard.  We cannot expect to guess the number in just one try.  We need to keep track of how many guesses the user has made.  We can do this by using a loop.

```js
.needs <std>
import string from "String";
import { readString, print } from "String" under str;
import Error from "Utils/Error";
import result from "Utils/result";
import { accept, reject } from "Utils/result";

fn main() -> int {
	// here we will specify the type of the variable instead of letting the compiler infer it... this is suggested when using numbers that could be int, long, or short.
	int secret = 42; // one of the most common types of variables in aflat is an int.  This stores an integer. 42 as the answer.
	
	for mutable int i = 0; i < 10; i = i + 1 {
		let guess = str.readString("Enter your guess: "); // get the string that the user entered
		let guessInt = match guess.toInt() {
			Ok(i) => i, // if the conversion is successful, return the int value
			Err(err) => panic(`Error converting string to int: {err}`.cstr()) // if the conversion fails, panic with an error message.
		}; // convert the string to an int.  If the conversion fails, panic with an error message.

		if guessInt == secret { // now that we have the same type we can compare them with the '==' operator
			str.print("You guessed it!\n");
			break; // exit the loop if the guess is correct
		} else if guessInt < secret // we can check if the guess is too low with the '<' operator
			str.print("Too low!\n")
		else
			str.print("Too high!\n");
	};
	str.print(`The secret number was {secret}\n`); // we can use the backtick to create a string that can contain variables.  The variables are surrounded by {} and the variable name is inside the braces.  The variable name is evaluated and the result is inserted into the string.  The string is then converted to a string and printed.
	// notice that the curly braces are not needed in the if statement.  The curly braces are only needed when there is a block of code to be executed.

	return 0;
};
```

The for loop looks like this:

```js
for int i = 0; i < 10; i++ {
	// do something
};
```

The first section creates a variable called `i` and sets it to 0.  The second section is the condition.  The condition is checked every time the loop runs.  The condition is checked to see if the loop should continue.  The third section is the increment.  The increment is executed every time the loop runs.  The increment is used to increase the value of the variable `i`.  The loop will continue until the condition is false.

## Random numbers
This game gets boring quickly.  We need to make sure that the program doesn't always guess the same number.  We can do this by using a random number generator.

```js
.needs <std>
.needs <std>
import string from "String";
import { readString, print } from "String" under str;
import Error from "Utils/Error";
import result from "Utils/result";
import { accept, reject } from "Utils/result";

fn main() -> int {
	// here we will specify the type of the variable instead of letting the compiler infer it... this is suggested when using numbers that could be int, long, or short.
	int secret = Random().nextInt(100); // generate a random number between 0 and 99
	
	for mutable int i = 0; i < 10; i = i + 1 {
		let guess = str.readString("Enter your guess: "); // get the string that the user entered
		let guessInt = match guess.toInt() {
			Ok(i) => i, // if the conversion is successful, return the int value
			Err(err) => panic(`Error converting string to int: {err}`.cstr()) // if the conversion fails, panic with an error message.
		}; // convert the string to an int.  If the conversion fails, panic with an error message.

		if guessInt == secret // now that we have the same type we can compare them with the '==' operator
			str.print("You guessed it!\n")
		else if guessInt < secret // we can check if the guess is too low with the '<' operator
			str.print("Too low!\n")
		else
			str.print("Too high!\n");
	};
	str.print(`The secret number was {secret}\n`); // we can use the backtick to create a string that can contain variables.  The variables are surrounded by {} and the variable name is inside the braces.  The variable name is evaluated and the result is inserted into the string.  The string is then converted to a string and printed.
	// notice that the curly braces are not needed in the if statement.  The curly braces are only needed when there is a block of code to be executed.

	return 0;
};
```

Now we have imported the Random class from the math library.  The Random class has a nextInt(int n) function that returns a random number between 0 and n.



## Classes
A class is a blueprint for creating an object.  We can create a class by using the class keyword.  The class keyword is followed by the name of the class.  The class name is followed by a block of code.  The block of code is enclosed in curly braces.

```js
class <classname> {
	// code goes here
};
```

A class can contain fields and methods.  Fields are data that is stored in the class.  Methods are functions that can act on the data in the class. Lets create a student class.

```js
class Student {
	mutable int id; // variables need to be tagged as mutable if they are going to be changed otherwise the compiler will throw an error
	mutable string name = "";
	mutable int average;
};
```
Lets see if we can use the class in a program.

```js
.needs <std>

import string from "String";
import { print } from "String" under str;

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;
};

fn main() -> int {
	let s = Student();
	s.id = 1;
	s.name = "John";
	s.average = 90;
	str.print(`Student: {s.name} id: {s.id} average: {s.average}\n`);
};
```

Take a look at the output.  It should look something like this.

```
Student: John id: 1 average: 90
```

## Struct literals
We can also use a struct literal to create the object as follows:

```js
.needs <std>

import string from "String";
import { print } from "String" under str;

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;
};

fn main() -> int {
	Student s = {
		1,
		`John`,
		90
	}; // This is a struct literal.  It is a way of statically creating an object.  The values are separated by commas and enclosed in curly braces.
	str.print(`Student: {s.name} id: {s.id} average: {s.average}\n`);
	return 0;
};
```

**Note:** Keep in mind that using Struct Literals in this way is inherently unsafe. If the order of the fields in the struct changes, the code will break. It is recommended to use the constructor method to create objects.
**Just Don't Do This Unless You Are Trying To Do Something Clever.**

## Class constructors
A class constructor is a function that is called when an object is created from the class.  The constructor is used to initialize the object.  The constructor is the first function that is called when an object is created from the class.  The name of a constructor function must be 'init'.

```js
.needs <std>

import string from "String";
import { print } from "String" under str;

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;

	fn init(int id, string name, int average) -> Student {
		str.print("The constructor has just been called!!!\n");
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};
};

fn main() -> int {
	let s = Student(1, "John", 90);
	str.print(`Student: {s.name} id: {s.id} average: {s.average}\n`);
	return 0;
};
```
Take a look at our program now, it gives the same output as before but it is much simpler.
lets add another student.

```js
.needs <std>

import string from "String";
import { print } from "String" under str;

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;

	fn init(int id, string name, int average) -> Student { 
		str.print("The constructor has just been called!!!\n");
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};
};

fn main() -> int {
	let JohnDoe = Student(1, "John Doe", 90);
	let JaneDoe = Student(2, "Jane Doe", 95);
	str.print(`Student: {JohnDoe.name} id: {JohnDoe.id} average: {JohnDoe.average}\n`);
	str.print(`Student: {JaneDoe.name} id: {JaneDoe.id} average: {JaneDoe.average}\n`);
	return 0;
};
```

The constructor made creating our objects much easier, but we still have some repetitive code.  You can see that identical code is being used to print out the student information.  In order to make this simpler, we can create a class method to print out the student information.

```js
.needs <std>

import string from "String";
import { print } from "String" under str;

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;

	fn init(int id, string name, int average) -> Student { 
		str.print("The constructor has just been called!!!\n");
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};

	fn printInfo() -> void {
		str.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
	};
};

fn main() -> int {
	let JohnDoe = Student(1, "John Doe", 90);
	let JaneDoe = Student(2, "Jane Doe", 95);

	JohnDoe.printInfo();
	JaneDoe.printInfo();
};
```
As you can see, the code is much cleaner.  We can now call the method to print out the student information. in stead of repeating the same code over and over again.

## Vectors
The code that we have here is great, but we can see how it can become very long if the number of students goes up. The aflat standard library provides a `vector` class.  The `vector` class is meant to be used as a list of objects.  In stead of using 's1' 's2' and so on, lets use a  vector to hold our students.

```js
.needs <std>

import string from "String";
import { print } from "String" under str;
import vector from "Collections/Vector"; // We need to import the Vector class to create a list of students
import option from "Utils/option"; // vector expects the option module to be imported
import {Some, None} from "Utils/option"; // vector expects the Some and None classes to be imported
// Vectors can be iterated over directly; Scroller is optional for manual control

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;

	fn init(int id, string name, int average) -> Student { 
		str.print("The constructor has just been called!!!\n");
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};

	fn printInfo() -> void {
		str.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
	};

	fn toString() -> string { // vector expects any class that uses it to implement the toString method
		return `Student: {my.name} id: {my.id} average: {my.average}`;
	};
};

fn main() -> int {
        let roster = [
                new Student(1, "John Doe", 90),
                new Student(2, "Jane Doe", 80)
        ];

        foreach student in roster {
                student.printInfo();
        };
};
```

Now we can add as many students as we want to the roster, and keep the code clean.  Try adding a few more students to the roster.

## Modules
It is helpful to hide our classes in another file, It helps to give us less to look at when we are working on the program.  In order to create a module, we can type the following in the terminal: 

```bash
aflat module Student
```

This will create a new file called 'Student/mod.af' in the src folder. Copy the Student class from the previous section and paste it into the new file. 

```js
.needs <std>
import string from "String";
import { print } from "String" under str;

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;

	fn init(int id, string name, int average) -> Student { 
		str.print("The constructor has just been called!!!\n");
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};

	fn printInfo() -> void {
		str.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
	};

	fn toString() -> string { // vector expects any class that uses it to implement the toString method
		return `Student: {my.name} id: {my.id} average: {my.average}`;
	};
};
```

Now it can bee imported into our main program the same way we would import any class.

```js
.needs <std>

import string from "String";
import { print } from "String" under str;
import vector from "Collections/Vector"; // We need to import the Vector class to create a list of students
import option from "Utils/option"; // vector expects the option module to be imported
import {Some, None} from "Utils/option"; // vector expects the Some and None classes to be imported

import Student from "./Student"; // Now we can import the Student module.  Note that If `Student.af` does not exist, it will look for `Student/mod.af`


fn main() -> int {
        let roster = [
                new Student(1, "John Doe", 90),
                new Student(2, "Jane Doe", 80)
        ];

        foreach student in roster {
                student.printInfo();
        };
};
```

We should see that the code is much easier to look at, and we get the same output as before.

## A Class Challenge
Now that we know how to make modules and classes, see if you can create an Assignment module. with an Assignment class.  The assignment class should have the following fields:
	- string name
	- int grade
The constructor should take in the name and the grade and set the fields.

## Adding to the student class
Now that we have the Assignment class, add a list of assignments as a field in our student class.  The list should be a vector.  Our new student module should look like this:

```js
.needs <std>
import string from "String";
import { print } from "String" under str;
import vector from "Collections/Vector";
import option from "Utils/option";
import {Some, None} from "Utils/option";
import Assignment from "../Assignment";

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;
    vector::<Assignment> assignments = new vector::<Assignment>(); // We create a new list of assignments using the Vector class

	fn init(int id, string name, int average) -> Student {
		str.print("The constructor has just been called!!!\n");
		my.id = id;
		my.name = name;
		my.average = average;
	};

	fn printInfo() -> void {
		str.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
	};

	fn toString() -> string {
		return `Student: {my.name} id: {my.id} average: {my.average}`;
	};
};
```

Now we should create a method to add an assignment to the student's list of assignments.  The method should take in an assignment object and add it to the list.

```js
.needs <std>
import string from "String";
import { print } from "String" under str;
import vector from "Collections/Vector";
import option from "Utils/option";
import {Some, None} from "Utils/option";
import Assignment from "../Assignment";

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;
    vector::<Assignment> assignments = new vector::<Assignment>(); // We create a new list of assignments using the Vector class

	fn init(int id, string name, int average) -> Student {
		str.print("The constructor has just been called!!!\n");
		my.id = id;
		my.name = name;
		my.average = average;
	};

	fn printInfo() -> void {
		str.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
	};

	fn toString() -> string {
		return `Student: {my.name} id: {my.id} average: {my.average}`;
	};

    fn addAssignment(Assignment &&assignment) -> void {
        my.assignments.push_back($assignment); // push_back takes ownership of the assignment
    };
};
```

We should also add a way to update the student's average each time we add an assignment. here is how we change the addAssignment method:
We will need to add a total variable to the student class.  This variable will keep track of the total of all the assignments.

```js
mutable int totalPoints = 0; // add this code to the student class
```

now we can calculate the average by dividing the total by the number of assignments.
```js
    fn addAssignment(Assignment &&assignment) -> void {
        my.assignments.push_back($assignment); // transfer ownership of assignment
        my.totalPoints = my.totalPoints + assignment.grade;
                int count = my.assignments.count();
        my.average = my.totalPoints / count;
    };
```

Now lets rewrite our main program to use our new student module.

```js
.needs <std>

import string from "String";
import { print } from "String" under str;
import vector from "Collections/Vector";
import option from "Utils/option";
import {Some, None} from "Utils/option";
import Student from "./Student";
import Assignment from "./Assignment";

fn main() -> int {
        let s = new Student(1, "John Doe", 0);
        s.addAssignment(Assignment("Math Homework", 95)); // Assignment(...) returns an owned value
        s.addAssignment(Assignment("Science Project", 88)); // ownership moves into addAssignment
        s.printInfo();
        return 0;
};
```

Now we can see that the updated average is correct. Try adding some more assignments and see if the average is still correct.

## Encapsulation
You may have noticed that there is no reason for anything outside of the student class to update the average.  What if a student finds our main module and adds code to update his grade... we cant have that.  That is why we use encapsulation.  We can hide the average field from the rest of the code by making it private. as follows:

```js
.needs <std>
import string from "String";
import { print } from "String" under str;
import vector from "Collections/Vector";
import option from "Utils/option";
import {Some, None} from "Utils/option";
import Assignment from "../Assignment";

class Student {
	mutable int id;
	mutable string name = "";
	private mutable int average;
    private mutable int totalPoints = 0;
    vector::<Assignment> assignments = new vector::<Assignment>(); // We create a new list of assignments using the Vector class

	fn init(int id, string name, int average) -> Student {
		my.id = id;
		my.name = name;
		my.average = average;
	};

	fn printInfo() -> void {
		str.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
	};

	fn toString() -> string {
		return `Student: {my.name} id: {my.id} average: {my.average}`;
	};

    fn addAssignment(Assignment &&assignment) -> void {
        my.assignments.push_back($assignment); // move assignment into vector
        my.totalPoints = my.totalPoints + assignment.grade;
                int count = my.assignments.count();
        my.average = my.totalPoints / count;
    };
};
```
Now if you try to use the average field outside of the student class, you will get an error.  This is because the average field is private.

If you want to access the average field, you can create a getter method that returns the average.  The getter method should look like this:

```js
fn getAverage() -> int {
	return my.average;
};
```

Now we can read the getAverage method and see that the average value.

## Building out a system
Now lets build out a student roster system. Delete everything in the main module.  add the following imports:

```js
.needs <std>

import string from "String";
import { print, readString } from "String" under str;
import vector from "Collections/Vector";
import option from "Utils/option";
import {Some, None} from "Utils/option";
import Student from "./Student";
import Assignment from "./Assignment";

```

Now we will make our main method and create the student roster list


```js
fn main() -> int {
	List roster = new List(Student);
};
```

Now we should add an infinite loop to our main method.  This loop will keep running until the user chooses to quit.

```js
fn main() -> int {
	List roster = new List(Student);

	while true { // things inside of this loop will run forever

	};
};
```

Inside of the loop you can print out all of the student information.  You can use the printInfo method from the student class.

```js
fn main() -> int {
        let roster = new vector::<Student>();

        while true {
                foreach s in roster {
                        s.printInfo();
                };
        };

	return 0;
};
```

now we will print out a menu to the user: 

```js
fn main() -> int {
	let roster = List(Student);

	while true {
		str.print("\n--Student Roster--\n");
		roster.forEach([Student s]=>{
			str.print(`{s.getInfo()}\n`); // We use the getInfo() method to print the student's information.
		});
		str.print("--------------------\n");
		str.print("Menu : 1. Add a student | 2. Add Grade | quit\n> "); // We print the menu.
		let input = str.readString("Enter a command: "); // We read the user's input.
	};
};
```

Now we need to create a function to add a student to the roster.  The function will take in a LinkedList of students and add a new student to the list.  This function should go under the imports and over the main function.

```js
// This is a function to add a student to the list.
fn addStudent(vector::<Student> roster) -> void {
        let name = str.readString("Enter student name: ");
        let id = roster.count() + 1;
        roster.push_back(new Student(id, name, 0)); // roster now owns the new student
};
```

Now we can call it from the main loop.

```js
fn main() -> int {
        let roster = new vector::<Student>();

        while true {
                str.print("\n--Student Roster--\n");
                foreach s in roster {
                        s.printInfo();
                };
                str.print("--------------------\n");
		str.print("Menu : 1. Add a student | 2. Add Grade | quit\n> "); // We print the menu.
		let input = str.readString("Enter a command: "); // We read the user's input.

		if input == "1"
			addStudent(roster);
	};

	return 0;
};
```

Next we will add an addGrade function that will add an assignment to the student. This should go under the imports and over the main function.

```js
// Add Grade
fn addGrade(vector::<Student> roster) -> bool {
	let id = str.readString("Enter student id: ");
	let id = match id.toInt() {
		Ok(i) => i,
		Err => {
			str.print("Invalid ID. Please enter a valid integer.\n");
			return false;
		}
	};
	/*
	 * below the findFirst function takes a function as an argument.
	 * This function checks if the student id matches the input id.
	 * We pass the id as a reference because there is no really good way to
	 * pass a value into the anonymous function.
	 */
	let student = match roster.findFirst( fn (Student s, int& i) return s.id == i, ?id) {
		Some(s) => s,
		None => {
			str.print("Student not found.\n");
			return false;
		}
	};

	let assignmentName = str.readString("Enter assignment name: ");
	let grade = str.readString("Enter assignment grade: ");
        match grade.toInt() {
                Ok(g) => {
                        let assignment = new Assignment(assignmentName, g); // 'new' returns an owned assignment
                        student.addAssignment($assignment); // transfer ownership to the student's vector
                        roster.set(student.id - 1, $student); // set takes ownership of the updated student
                return true;
                },
		Err => {
			str.print("Invalid grade. Please enter a valid integer.\n");
			return false;
		}
	};
};

```

Now we can call it from the main function.  and we can call the addGrade function to the main function and add the exit option.

```js
fn main() -> int {
	let roster = new vector::<Student>();

        while true {
                str.print("\n--Student Roster--\n");
                foreach s in roster {
                        s.printInfo();
                };
                str.print("--------------------\n");
		str.print("Menu : 1. Add a student | 2. Add Grade | quit\n> "); // We print the menu.
		let input = str.readString("Enter a command: "); // We read the user's input.

		if input == "1" {
			addStudent(roster);
		} else if input == "2" {
			if !addGrade(roster) {
				str.print("Failed to add grade.\n");
			};
		} else if input == "quit" | input == "exit" {
			str.print("Exiting program.\n");
			break;
		} else {
			str.print("Invalid command. Please try again.\n");
		};
	};

	return 0;
};
```

All together, the main module should look like this:

```js
.needs <std>

import string from "String";
import { print, readString } from "String" under str;
import vector from "Collections/Vector";
import option from "Utils/option";
import result from "Utils/result";
import {Some, None} from "Utils/option";
import Student from "./Student";
import Assignment from "./Assignment";

fn addStudent(vector::<Student> roster) -> void {
        let name = str.readString("Enter student name: ");
        let id = roster.count() + 1;
        roster.push_back(new Student(id, name, 0)); // push_back takes ownership of the new student
};

fn addGrade(vector::<Student> roster) -> bool {
	let id = str.readString("Enter student id: ");
	let id = match id.toInt() {
		Ok(i) => i,
		Err => {
			str.print("Invalid ID. Please enter a valid integer.\n");
			return false;
		}
	};
	/*
	 * below the findFirst function takes a function as an argument.
	 * This function checks if the student id matches the input id.
	 * We pass the id as a reference because there is no really good way to
	 * pass a value into the anonymous function.
	 */
	let student = match roster.findFirst( fn (Student s, int& i) return s.id == i, ?id) {
		Some(s) => s,
		None => {
			str.print("Student not found.\n");
			return false;
		}
	};

	let assignmentName = str.readString("Enter assignment name: ");
	let grade = str.readString("Enter assignment grade: ");
        match grade.toInt() {
                Ok(g) => {
                        let assignment = new Assignment(assignmentName, g); // owned assignment
                        student.addAssignment($assignment); // ownership is moved to the student
                        return true;
                },
		Err => {
			str.print("Invalid grade. Please enter a valid integer.\n");
			return false;
		}
	};
};

fn main() -> int {
	let roster = new vector::<Student>();

        while true {
                str.print("\n--Student Roster--\n");
                foreach s in roster {
                        s.printInfo();
                };
                str.print("--------------------\n");
		str.print("Menu : 1. Add a student | 2. Add Grade | quit\n> "); // We print the menu.
		let input = str.readString("Enter a command: "); // We read the user's input.

		if input == "1" {
			addStudent(roster);
		} else if input == "2" {
			if !addGrade(roster) {
				str.print("Failed to add grade.\n");
			};
		} else if input == "quit" | input == "exit" {
			str.print("Exiting program.\n");
			break;
		} else {
			str.print("Invalid command. Please try again.\n");
		};
	};

	return 0;
};
```

Play around with the program and see how it works.

As a side project, see if you can add a way to view all of the assignments in a students assignment list.

## Templates
Templates let you write classes and functions that operate on any number of
types. Add a `types(T)` line before the definition—replace `T` with as many type
names as you need (for example `types(Key, Value)`). Template functions can
usually infer the type arguments from their parameters, but class templates
cannot. Always instantiate a templated class with `::<Type>` to specify the
desired type.

## Error Handling in AFlat
The aflat standard library offers a way to handle errors using the `result`
union. It replaces the older `Result` class. `Result` stored values as `any` and
used reference counting, which meant extra casts and allocations. The new
`result` keeps the payload typed so the compiler can check uses at compile time.
You still instantiate templated results with `::<type>` when the type cannot be
inferred.

### Importing and Using Results
Import the helpers from `Utils/result` under a namespace of your choice. Use
`accept` to wrap a success value and `reject` for an error. Functions that may
fail should return `result::<T>` or use the shorthand `T!` for the return type.

Example of a function that returns a result:

```js
import {accept, reject} from "Utils/result" under pr;

fn divide(int a, int b) -> int! {
        if b == 0
                return pr.reject::<int>(Error("Cannot divide by zero"));
        return a / b; // automatically wrapped in Ok
};
```

You can handle the result with the `match` method, providing handlers for the
`ok` and `err` cases.

Example of calling a function that returns a result:

```js
fn main() -> int {
        match divide(10, 0) {
                Ok(value) => {
                    str.print(`Value: {value}\n`);
                },
                Err(e) => {
                    str.print(`Error: {e}\n`);
                }
        };
        return 0;
};
```

The `divide` function illustrates the `int!` shorthand. Returning a bare value
automatically wraps it with `accept`, while an error uses `reject`. This lets
errors propagate much like exceptions without additional boilerplate.

### Adding Error Handling to the Student Roster
We will add error handling to the addGrade function.  We will also add a way to
remove a student from the roster. Instead of returning a boolean, we will return
a result. We can then resolve or reject that result based on the outcome of the
function.

Lets take a look at the updated addGrade function:
```js
fn addGrade(vector::<Student> roster) -> bool! {
	let id = str.readString("Enter student id: ");
	let id = id.toInt()!; // We can now use the bubble operator (!) to unwrap the result of toInt. 
	// If the conversion fails, the error will be returned to the caller (not stack unwinding just returning).

	/*
	 * below the findFirst function takes a function as an argument.
	 * This function checks if the student id matches the input id.
	 * We pass the id as a reference because there is no really good way to
	 * pass a value into the anonymous function.
	 */
	let student = match roster.findFirst( fn (Student s, int& i) return s.id == i, ?id) {
		Some(s) => s,
		None => return res.reject::<bool>(new Error(`Student with id {id} not found.`)) // for reject, we have to tell it what it could have been
	};

        let assignmentName = str.readString("Enter assignment name: ");
        let grade = str.readString("Enter assignment grade: ");
       let assignment = new Assignment(assignmentName, grade.toInt()!); // owned assignment object
       student.addAssignment($assignment); // move assignment into the student
       roster.set(student.id - 1, $student); // transfer ownership to the roster
       return true;
};
```

Now we need to update the main function to handle the result.

```js
fn main() -> int {
        let roster = new vector::<Student>();

        while true {
                str.print("\n--Student Roster--\n");
                foreach s in roster {
                        s.printInfo();
                };
                str.print("--------------------\n");
		str.print("Menu : 1. Add a student | 2. Add Grade | quit\n> "); // We print the menu.
		let input = str.readString("Enter a command: "); // We read the user's input.

		if input == "1" {
			addStudent(roster);
		} else if input == "2" {
			match addGrade(roster) {
				Ok => {
					str.print("Grade added successfully.\n");
				},
				Err(e) => {
					str.print(`Error: {e}\n`);
				},
			};
		} else if input == "quit" | input == "exit" {
			str.print("Exiting program.\n");
			break;
		} else {
			str.print("Invalid command. Please try again.\n");
		};
	};

	return 0;
};
```

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
## Done so soon?
Don't worry, more tutorial to come...
