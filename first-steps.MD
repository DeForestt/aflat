# First Steps with AFlat

## Installation
***Remember!!! If you already Insalled aflat, remember to update the repository on your system***<br>
Please see the [installation instructions](README.md) for the steps to download and build.  If aflat is already installed, then you can move on the the next section. **This Tutorial tracks the latest version of aflat**

To update aflat run the following command

```bash
aflat update
```
You should see a bunch of output

## First Project
To create the tutorial project, run the following command in your project directory.
```bash
aflat make tutorial
```

The tutorial project will have been created in the `tutorial` directory.  The tutorial project will have a `aflat.cfg` file that will tell the aflat build system what to build and link.  The tutorial project will have a `src/main.af` file that will be the entry point for the project. It will also have a `src/test/test.af` file that is will be used to test the project with the `aflat test` command.

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
import { readString, print } from "String" under st;

fn main() -> int {
	let name = st.readString("What is your name? ");
	st.print(`Hello, {name}.\nFrom the AFlat Team.\n`);
	return 0;
};
```

Lets walk take a look at the new lines.
- The first new line is the `import string from "String";` command.  This tells AFlat to import the `string` data type from the `String` library.
- The second new line is the `import { readString, print } from "String" under st;` command.  This tells AFlat to import the `readString` function from the `String` library.  The under command tells AFlat that the `readString` function should be imported under the name `st`.  Any functions called from this library must start with `st.`. It also imports the `print` function under the same namespace. Importing functions by name is recommended rather than importing * which can sometimes lead to undefined behavior (this is a bug that we are working on fixing).
- The third new line is the `let name = st.readString("What is your name? ");` command.  This tells AFlat to create a new variable called `name` and assign it the value returned by the `st.readString` function. The `let` keyword allows AFlat to infer the type of the new variable. The `st.readString` function takes an optional string as an argument and returns a string.  The string that is passed to the `st.readString` function is the prompt that is printed to the standard output.
- The last new line is the ``st.print(`Hello, {name}.\nFrom the AFlat Team.\n`);`` command.  This tells AFlat to call the `st.print` function.  The `st.print` function takes a string as an argument and prints it to the standard output.  The string that is passed to the `st.print` function is the string that is printed to the standard output.  The `{name}` is a placeholder for the value of the `name` variable.  The `\n` is a newline character.
- Run the program with `aflat run` and see what happens.

## If else statements
I take it back.  We don't want other people to see our program... in fact, we want to make sure that the program is only run if the user enters the correct name.  We can do this by adding an if else statement.

```js
.needs <std>

import string from "String";
import { readString, print } from "String" under st;

fn main() -> int {
	let name = st.readString("What is your name? ");

	if name == "Name" {
		st.print(`Hello, {name}. \nFrom the AFlat Team.\n`);
	} else {
		st.print("I don't know you... \nGo away!\n");
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
import { readString, print } from "String" under st;
import Error from "Utils/Error"; // we need to import the Error type to use it in the match call for error handling

fn main() -> int {
		// here we will specify the type of the variable instead of letting the compiler infer it... this is suggested when using numbers that could be int, long, or short.
	int secret = 42; // one of the most common types of variables in aflat is an int.  This stores an integer. 42 as the answer.
	
	let guess = st.readString("Enter your guess: "); // get the string that the user entered
	int guessInt = guess.toInt().match({
		"ok": [int i] => return i,
		"_": [Error w] => panic(`Error converting string to int: {w}`.cstr())
	}); // convert the string to an int.  If the conversion fails, panic with an error message.

	if guessInt == secret // now that we have the same type we can compare them with the '==' operator
		st.print("You guessed it!\n")
	else if guessInt < secret // we can check if the guess is too low with the '<' operator
		st.print("Too low!\n")
	else
		st.print("Too high!\n");
	
	// notice that the curly braces are not needed in the if statement.  The curly braces are only needed when there is a block of code to be executed.

	return 0;
};
```

## Loops
This game is a bit too hard.  We cannot expect to guess the number in just one try.  We need to keep track of how many guesses the user has made.  We can do this by using a loop.

```js
.needs <std>
import string from "String";
import { readString, print } from "String" under st;
import Error from "Utils/Error";

fn main() -> int {
	// here we will specify the type of the variable instead of letting the compiler infer it... this is suggested when using numbers that could be int, long, or short.
	int secret = 42; // one of the most common types of variables in aflat is an int.  This stores an integer. 42 as the answer.
	
	for mutable int i = 0; i < 10; i = i + 1 {
		let guess = st.readString("Enter your guess: "); // get the string that the user entered
		int guessInt = guess.toInt().match({
			"ok": [int i] => return i,
			"_": [Error w] => panic(`Error converting string to int: {w}`.cstr())
		}); // convert the string to an int.  If the conversion fails, panic with an error message.
		// this is the main error handling mechanism in aflat. Note that the match method name is a convention and not a keyword.
		// it may be added to the language in the future.

		if guessInt == secret // now that we have the same type we can compare them with the '==' operator
			st.print("You guessed it!\n")
		else if guessInt < secret // we can check if the guess is too low with the '<' operator
			st.print("Too low!\n")
		else
			st.print("Too high!\n");
	};
	st.print(`The secret number was {secret}\n`); // we can use the backtick to create a string that can contain variables.  The variables are surrounded by {} and the variable name is inside the braces.  The variable name is evaluated and the result is inserted into the string.  The string is then converted to a string and printed.
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
import string from "String";
import { readString, print } from "String" under st;
import Error from "Utils/Error";
import Random from "math";

fn main() -> int {
	// here we will specify the type of the variable instead of letting the compiler infer it... this is suggested when using numbers that could be int, long, or short.
	int secret = Random().nextInt(100); // generate a random number between 0 and 99
	
	for mutable int i = 0; i < 10; i = i + 1 {
		let guess = st.readString("Enter your guess: "); // get the string that the user entered
		int guessInt = guess.toInt().match({
			"ok": [int i] => return i,
			"_": [Error w] => panic(`Error converting string to int: {w}`.cstr())
		}); // convert the string to an int.  If the conversion fails, panic with an error message.
		// this is the main error handling mechanism in aflat. Note that the match method name is a convention and not a keyword.
		// it may be added to the language in the future.

		if guessInt == secret // now that we have the same type we can compare them with the '==' operator
			st.print("You guessed it!\n")
		else if guessInt < secret // we can check if the guess is too low with the '<' operator
			st.print("Too low!\n")
		else
			st.print("Too high!\n");
	};
	st.print(`The secret number was {secret}\n`); // we can use the backtick to create a string that can contain variables.  The variables are surrounded by {} and the variable name is inside the braces.  The variable name is evaluated and the result is inserted into the string.  The string is then converted to a string and printed.
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
	Student s = Student();
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
};
```

**Note:** Keep in mind that using Struct Literals in this way is inherently unsafe. If the order of the fields in the struct changes, the code will break. It is recommended to use the constructor method to create objects.
**Just Don't Do This!**

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
	Student s = Student(1, "John", 90);
	str.print(`Student: {s.name} id: {s.id} average: {s.average}`);
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
	Student JohnDoe = Student(1, "John Doe", 90);
	Student JaneDoe = Student(2, "Jane Doe", 95);
	str.print(`Student: {JohnDoe.name} id: {JohnDoe.id} average: {JohnDoe.average}\n`);
	str.print(`Student: {JaneDoe.name} id: {JaneDoe.id} average: {JaneDoe.average}\n`);
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
	Student JohnDoe = Student(1, "John Doe", 90);
	Student JaneDoe = Student(2, "Jane Doe", 95);

	JohnDoe.printInfo();
	JaneDoe.printInfo();
};
```
As you can see, the code is much cleaner.  We can now call the method to print out the student information. in stead of repeating the same code over and over again.

## Lists
The code that we have here is great, but we can see how it can become very long if the number of students goes up. The aflat standard library provides a List class.  The List class is meant to be used as a list of objects.  In stead of using 's1' 's2' and so on, lets use a  List to hold our students.

```js
.needs <std>

import string from "String";
import { print } from "String" under str;
import List from "Collections";
import Scroller from "Collections/Scroller"; // We need to import the Scroller class to iterate over the list

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
	List roster = List(Student);
	roster.pushBack(Student(1, "John Doe", 90)); // The pushBack method is a way of adding elements to the list
	roster.pushBack(Student(2, "Jane Doe", 80));

	// this is a foreach loop that iterates over an iterable object
	foreach Student student : roster.scroll() { // A List is not an iterable object, calling the `scroll` method makes it iterable and allows us to use the foreach loop
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

This will create a new file called 'Student.af' in the src folder. Copy the Student class from the previous section and paste it into the new file. 

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
```

Now it can bee imported into our main program the same way we would import any class.

```js
.needs <std>
import string from "String";
import { print } from "String" under str;
import List from "Collections";
import Scroller from "Collections/Scroller"; // We need to import the Scroller class to iterate over the list
import Student from "./src/Student"; // Then we import the Student class from the Student module. Note that for project level imports, we need to use the relative path to the project root

fn main() -> int {
	List roster = List(Student);
	roster.pushBack(Student(1, "John Doe", 90)); // The pushBack method is a way of adding elements to the list
	roster.pushBack(Student(2, "Jane Doe", 80));

	// this is a foreach loop that iterates over an iterable object
	foreach Student student : roster.scroll() { // A List is not an iterable object, calling the `scroll` method makes it iterable and allows us to use the foreach loop
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
Now that we have the Assignment class, add a list of assignments as a field in our student class.  The list should be a List.  Our new student module should look like this:

```js
.needs <std>
import string from "String";
import List from "Collections";
import Assignment from "./src/Assignment";
import { print } from "String" under str;

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;
	List assignments = new List(Assignment);

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
```

Now we should create a method to add an assignment to the student's list of assignments.  The method should take in an assignment object and add it to the list.

```js
.needs <std>
import string from "String";
import List from "Collections";
import Assignment from "./src/Assignment";
import { print } from "String" under str;

class Student {
	mutable int id;
	mutable string name = "";
	mutable int average;
	List assignments = new List(Assignment);

	fn init(int id, string name, int average) -> Student { 
		str.print("The constructor has just been called!!!\n");
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};
	

	fn printInfo() -> void {
		str.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
	};

	fn addAssignment(Assignment assignment) -> void {
        my.assignments.pushBack(assignment);
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
    fn addAssignment(Assignment assignment) -> void {
        my.assignments.pushBack(assignment);
        my.totalPoints = my.totalPoints + assignment.grade;
		int count = my.assignments.getCount();
        my.average = my.totalPoints / count;
    };
```

Now lets rewrite our main program to use our new student module.

```js
.needs <std>

import string from "String";
import { print } from "String" under str;
import List from "Collections";
import Student from "./src/Student";
import Assignment from "./src/Assignment";

fn main() -> int {
	let s = new Student(1, "John Doe", 0);

	s.addAssignment(Assignment("Math test", 100));
	s.addAssignment(Assignment("English test", 90));
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
import List from "Collections";
import Assignment from "./src/Assignment";
import { print } from "String" under str;

class Student {
	mutable int id;
	mutable string name = "";
	private mutable int average;
	private mutable int totalPoints = 0;
	List assignments = new List(Assignment);

	fn init(int id, string name, int average) -> Student { 
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};
	

	fn printInfo() -> void {
		str.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
	};

    fn addAssignment(Assignment assignment) -> void {
        my.assignments.pushBack(assignment);
        my.totalPoints = my.totalPoints + assignment.grade;
		int count = my.assignments.getCount();
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
import { print } from "String" under str;
import List from "Collections";
import Student from "./src/Student";
import Assignment from "./src/Assignment";

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
	List roster = new List(Student);

	while true { // things inside of this loop will run forever
		
		// print all students in the list
		roster.forEach([Student s] => {
			s.printInfo();
		});
	};
};
```
We will add a new getInfo method to the student class.  This method will return a string with all of the student information.
```js
string getInfo() {
	return `Student: {my.name} id: {my.id} average: {my.average}\n`;
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
// This a function to add a student to the list.
fn addStudent(List roster) -> void {
	let name = str.readString("Enter student name: ");
	let id = roster.getCount() + 1;
	roster.pushBack(Student(id, name, 0));
};
```

Now we can call it from the main loop.

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

		if input == "1"
			addStudent(roster);
	};
};
```

Next we will add an addGrade function that will add an assignment to the student. This should go under the imports and over the main function.

```js
// Add Grade
fn addGrade(List roster) -> bool {
	let id = str.readString("Enter student id or name: ");
	
	Student student = if id.isNumeric() roster.findFirst([Student s, string i] => { return s.id == i.toInt().resolve([int value] => return value, NULL) }, id)
		else roster.findFirst([Student s, string i] => return s.name == i, id);

	// above we use the ternary operator to check if the id is numeric.  If it is, we search for the student by id.  If it is not, we search for the student by name.

	if student == NULL {
		str.print("Student not found.\n");
		let junk = str.readString("Press enter to continue...");
		return false;
	};

	let assignment = str.readString("Enter assignment name: ");
	let grade = str.readString( `Enter {student.name}'s grade:`);
	if !grade.isNumeric() {
		str.print("Grade must be a number.\n");
		let junk = str.readString("Press enter to continue...");
		return false;
	};
	student.addAssignment(Assignment(assignment, grade.toInt().resolve([int value] => return value, NULL))); // We use the resolve() method to get the value of the Result.
	// We will talk more about products later.
	return true;
};

```

Now we can call it from the main function.  and we can call the addGrade function to the main function and add the exit option.

```js
fn main() -> int {
	let roster = List(Student);

	while true {
		io.clear(); // We clear the screen.
		str.print("\n--Student Roster--\n");
		roster.forEach([Student s]=>{
			str.print(`{s.getInfo()}\n`); // We use the getInfo() method to print the student's information.
		});
		str.print("--------------------\n");
		str.print("Menu : 1. Add a student | 2. Add Grade | quit\n> "); // We print the menu.
		let input = str.readString("Enter a command: "); // We read the user's input.

		if input == "1"
			addStudent(roster)
		else if input == "2"
			addGrade(roster)
		else if input == "quit"
			return 0
		else {
			str.print("Invalid command\n");
			let junk = str.readString("Press enter to continue...");
		};
	};
};
```

All together, the main module should look like this:

```js
.needs <std>

import { clear } from "io" under io;
import string from "String";
import { print, readString } from "String" under str;
import List from "Collections";
import Student from "./src/Student";
import Assignment from "./src/Assignment";
import Result from "Utils/Result";

// Add Grade
fn addGrade(List roster) -> bool {
	let id = str.readString("Enter student id or name: ");
	
	Student student = if id.isNumeric() roster.findFirst([Student s, string i] => { return s.id == i.toInt().resolve([int value] => return value, NULL) }, id)
		else roster.findFirst([Student s, string i] => return s.name == i, id);

	// above we use the ternary operator to check if the id is numeric.  If it is, we search for the student by id.  If it is not, we search for the student by name.

	if student == NULL {
		str.print("Student not found.\n");
		let junk = str.readString("Press enter to continue...");
		return false;
	};

	let assignment = str.readString("Enter assignment name: ");
	let grade = str.readString( `Enter {student.name}'s grade:`);
	if !grade.isNumeric() {
		str.print("Grade must be a number.\n");
		let junk = str.readString("Press enter to continue...");
		return false;
	};
	student.addAssignment(Assignment(assignment, grade.toInt().resolve([int value] => return value, NULL))); // We use the resolve() method to get the value of the Result.
	// We will talk more about products later.
	return true;
};

// This a function to add a student to the list.
fn addStudent(List roster) -> void {
	let name = str.readString("Enter student name: ");
	let id = roster.getCount() + 1;
	roster.pushBack(Student(id, name, 0));
};

fn main() -> int {
	let roster = List(Student);

	while true {
		io.clear(); // We clear the screen.
		str.print("\n--Student Roster--\n");
		roster.forEach([Student s]=>{
			str.print(`{s.getInfo()}\n`); // We use the getInfo() method to print the student's information.
		});
		str.print("--------------------\n");
		str.print("Menu : 1. Add a student | 2. Add Grade | quit\n> "); // We print the menu.
		let input = str.readString("Enter a command: "); // We read the user's input.

		if input == "1"
			addStudent(roster)
		else if input == "2"
			addGrade(roster)
		else if input == "quit"
			return 0
		else {
			str.print("Invalid command\n");
			let junk = str.readString("Press enter to continue...");
		};
	};
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
The aflat standard library offers a way to handle errors using `Result`. Results
force you to handle possible failures. With the template system introduced by
the `types` keyword, a `Result` can retain the type of the value it wraps.
Instantiate templated classes or functions with `::<type>` when the type cannot
be inferred.

### Importing and Using Results
Import the helpers from `Utils/result` under a namespace of your choice. Use
`accept` to wrap a success value and `reject` for an error. Functions that may
fail should return `result::<T>`.

Example of a function that returns a result:

```js
import {accept, reject} from "Utils/result" under pr;

fn divide(int a, int b) -> result::<int> {
        if b == 0
                return pr.reject(Error("Cannot divide by zero"));
        return pr.accept(a / b);
};
```

You can handle the result with the `match` method, providing handlers for the
`ok` and `err` cases.

Example of calling a function that returns a result:

```js
fn main() -> int {
        divide(10, 0).match({
                ok: fn (int value) {
                        str.print(`Value: {value}\n`);
                },
                err: fn (Error e) {
                        str.print(`Error: {e}\n`);
                }
        });
        return 0;
};
```

### Adding Error Handling to the Student Roster
We will add error handling to the addGrade function.  We will also add a way to remove a student from the roster. Instead of returning a boolean, we will return a product.  We can then resolve or reject the product based on the result of the function.

Lets take a look at the updated addGrade function:
```js
// Add Grade
fn addGrade(List roster) -> result::<void> {
	let id = str.readString("Enter student id or name: ");
	
	Student student = if id.isNumeric() roster.findFirst([Student s, string i] => { return s.id == i.toInt().resolve([int value] => return value, NULL) }, id)
		else roster.findFirst([Student s, string i] => return s.name == i, id);

	// above we use the ternary operator to check if the id is numeric.  If it is, we search for the student by id.  If it is not, we search for the student by name.

        if student == NULL {
                return pr.reject(Error("Student not found."));
        };

	let assignment = str.readString("Enter assignment name: ");
	let grade = str.readString( `Enter {student.name}'s grade:`);
	string errorMsg = "";
        int grade = grade.toInt().resolve([int value] => return value, [string error, mutable string msg] => {
                msg = error;
                return 0;
        }, errorMsg);
        if errorMsg != "" {
                return pr.reject(Error(errorMsg));
        };

        student.addAssignment(Assignment(assignment, grade));
        return pr.accept(NULL);
};
```

Now we need to update the main function to handle the product.

```js
	fn main() -> int {
	let roster = List(Student);

	while true {
		io.clear(); // We clear the screen.
		str.print("\n--Student Roster--\n");
		roster.forEach([Student s]=>{
			str.print(`{s.getInfo()}\n`); // We use the getInfo() method to print the student's information.
		});
		str.print("--------------------\n");
		str.print("Menu : 1. Add a student | 2. Add Grade | quit\n> "); // We print the menu.
		let input = str.readString("Enter a command: "); // We read the user's input.

		if input == "1"
			addStudent(roster)
                else if input == "2"
                        addGrade(roster).match({
                                ok: fn (void _) {
                                        return NULL;
                                },
                                err: fn (Error error) {
                                        str.print(`Error: {error}\n`);
                                        let __junk = str.readString("Press enter to continue...");
                                        return NULL;
                                }
                        })
		else if input == "quit"
			return 0
		else {
			str.print("Invalid command\n");
			let __junk = str.readString("Press enter to continue...");
		};
	};
};
```
## Done So Soon?
Don't worry, more tutorial to come...

## Ownership, References, and Selling
AFlat uses an explicit ownership model. Objects remain valid until they go out of scope or are deleted with `delete`. Class instances are passed by reference, so assigning them to another variable does not copy the object. You can take mutable or constant references using `mutable` or `const` in front of the reference type.

Example:
```aflat
class Data {
    mutable int value = 10;
};

fn modifyData(Data d) -> void {
    d.value = 42;
};

fn main() -> int {
    Data d = new Data();
    modifyData(d); // d is passed by reference
    str.print(`Updated value: {d.value}\n`);
    return 0;
};
```

The `$` operator transfers ownership, preventing reuse of the original variable.
```aflat
fn takeOwnership(mutable int &&a) -> void {
    print(`Taking ownership of {a}`);
};

fn main() -> int {
    int x = 10;
    takeOwnership($x); // x is now invalid
    return 0;
};
```

