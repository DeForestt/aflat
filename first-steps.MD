# First Steps with AFlat

## Installation
***Remember!!! If you already Insalled aflat, remember to update the repository on your system***<br>
Please see the [installation instructions](README.md) for the steps to download and build.  If aflat is already installed, then you can move on the the next section.

To update the repository, run the following commands:

```bash
cd aflat # Change to the aflat directory
git pull
make
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

int main() {
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

int main() {
    io.print("Hello, world!\n");
    io.print("From, The aflat Team\n");
    return 0;
};
```

## Working with strings
When you show your program to your friends, it will be nice to have the program print out their name. Take a look at the code below.

```js
.needs <std>
import * from "io" under io;
import String from "String";
import * from "String" under str;

int main(){
	io.print("Please enter your name: ");
	String name = str.getString();
	io.print("Hello, ");
	str.printString(name);
	io.print("!\n");
	io.print("From, The aflat Team\n");
	return 0;
};
```

Lets walk take a look at the new lines.
- The thrid line, imports the 'String' object from the 'String' library.  The 'String' object makes it easy to deal with text. When importing everything '*' from a module, only functions are imported. Objects must be imported explicitly.
- The fourth line, imports all of the functions from the 'String' library.
- Line eight creats a new String object and assigns it to the variable `name`.  The `getString` function is used to get the string that the user enters in the standard input.
- Line ten uses the `printString` function to print the name that the user entered to the standard input.

run the program with `aflat run` and see what happens.

## If else statements
I take it back.  We don't want other people to see our program... in fact, we want to make sure that the program is only run if the user enters the correct name.  We can do this by adding an if else statement.

```js
.needs <std>
import * from "io" under io;
import String from "String";
import * from "String" under str;

int main(){
	io.print("Please enter your name: ");
	String name = str.getString();

	if name == "Name"{
		io.print("Hello, ");
		str.printString(name);
		io.print("!\n");
		io.print("From, The aflat Team\n");
	} else {
		io.print("I don't know you... \nGo away!\n");
	};

	return 0;
};
```

The if statment will check if the two strings are equal. If they are equal, then the program statment will be executed.  If they are not equal, then the else statement will be executed.  Keep in mind that the else statement is optional.


## Guessing Game
Now we will make a simple guessing game.  The program will ask the user to guess a number.  If the user guesses the number, the program will tell them they won.  If the user guesses the wrong number, the program will tell them  too high or too low. Explinations will be found in the comments.

```js
.needs <std>
import * from "io" under io;
import String from "String";
import * from "String" under str;

int main(){
	int secret = 42;	// one of the most common types of variables in aflat is an int.  This stores an integer. 42 as the answer.
	io.print("Please enter a guess: ");  // ask the user to guess the number
	
	String guess = str.getString(); // get the string that the user entered
	int guessInt = guess.toInt(); // the String object provides a toInt() function that converts the string to an integer.

	if guessInt == secret // now that we have the same type we can compare them with the '==' operator
		io.print("You guessed it!\n")
	else if guessInt < secret // we can check if the guess is too low with the '<' operator
		io.print("Too low!")
	else
		io.print("Too high!");
	
	// notice that the curly braces are not needed in the if statement.  The curly braces are only needed when there is a block of code to be executed.

	return 0;
};
```

## Loops
This game is a bit too hard.  We cannot expect to guess the number in just one try.  We need to keep track of how many guesses the user has made.  We can do this by using a loop.

```js
.needs <std>
import * from "io" under io;
import String from "String";
import * from "String" under str;

int main(){
	int secret = 42;
	io.print("Please enter a guess: ");
	

	for int i = 0; i < 10; i++ {
		String guess = str.getString();
		int guessInt = guess.toInt();

		if guessInt == secret {
			io.print("You guessed it!\n");
			return 0;	// return causes the program to exit
		} else if guessInt < secret
			io.print("Too low!")
		else
			io.print("Too high!");
	};
	io.print("You ran out of guesses!\n");
	return 0;
};
```

The for loop looks like this:

```js
for int i = 0; i < 10; i++ {
	// do something
};
```

The first section creats a variable called `i` and sets it to 0.  The second section is the condition.  The condition is checked every time the loop runs.  The condition is checked to see if the loop should continue.  The third section is the increment.  The increment is executed every time the loop runs.  The increment is used to increase the value of the variable `i`.  The loop will continue until the condition is false.

## Random numbers
This game gets boring quickly.  We need to make sure that the program doesn't always guess the same number.  We can do this by using a random number generator.

```js
.needs <std>

import * from "io" under io;
import String from "String";
import * from "String" under str;
import Random from "math";

int main(){
	Random r = Random();
	io.printChar('\n');
	int secret = r.nextInt(100);

	for int i = 0; i < 10; i++ {
		io.print("Please enter a guess: ");
		String guess = str.getString();
		int guessInt = guess.toInt();

		if guessInt == secret {
			io.print("You guessed it!\n");
			return 0;	// return causes the program to exit
		} else if guessInt < secret
			io.print("Too low!\n")
		else
			io.print("Too high!\n");
	};
	
	return 0;
};
```

Now we have imported the Random object from the math library.  The random object has a nextInt(int n) function that returns a random number between 0 and n.


## Arrays
Lets go back to the program we wrote in the if else section.  Now that we know how to use random, we can make it a little bit more interesting.  Lets make the program select a random greeting to greet the user.

```js
.needs <std>
import * from "io" under io;
import String from "String";
import * from "String" under str;
import Random from "math";

int main(){
	adr[7] salutations = {"Hello", "Greetings", "Good Evening", "Good Afternoon", "Good Morning", "Salutations", "Good Day"};

	io.print("Please enter your name: ");
	String name = str.getString();

	if name == "Name" {
		Random rand = Random();
		io.print(salutations[rand.nextInt(7)]); io.print(", ");
		str.printString(name);
		io.print("!\n");
		io.print("From, The aflat Team\n");
	} else {
		io.print("I don't know you... \nGo away!\n");
	};

	return 0;
};
```

What we have on line 8 is an array.  In aflat an array is declared as follows `<typename>[count] name`

examples:
```js
int[5] myArray;
adr[5] myArray;
String[5] myArray;
```

Elements of an array are accessed by index.  The index is the number of the element.  The first element is 0, the second is 1, and so on.

```js
myArray[0] = "Hello";
myArray[1] = "World";
```


## Classes
A class is a blueprint for creating an object.  We can create a class by using the class keyword.  The class keyword is followed by the name of the class.  The class name is followed by a block of code.  The block of code is enclosed in curly braces.

```js
class <classname> {
	// code goes here
};
```

A class can contain filds and methods.  Filds are data Types that are stored in the class.  Methods are functions that can act on the data in the class. Lets create a student class.

```js
class Student {
	int id;
	adr name;
	int Average;
};
```
Lets see if we can use the class in a program.

```js
.needs <std>

import * from "io" under io;

class Student {
	int id;
	adr name;
	int average;
};

int main(){
	Student s = Student();
	s.id = 1;
	s.name = "John Doe";
	s.average = 90;

	io.print("Student information:\n");
	io.print("\tID: "); io.printInt(s.id); io.print("\n");
	io.print("\tName: "); io.print(s.name); io.print("\n");
	io.print("\tAverage: "); io.printInt(s.average); io.print("\n");

	return 0;
};
```

Take a look at the output.  It should look something like this.

```
Student information:
	ID: 1
	Name: John Doe
	Average: 90
```

## Class constructors
A class constructor is a function that is called when an object is created from the class.  The constructor is used to initialize the object.  The constructor is the first function that is called when an object is created from the class.  The name of a constructor function must be 'init'.

```js
class Student {

	int id;
	adr name;
	int average;

	init(int id, adr name, int average){
		io.print("The constructor has just been called!!!\n");
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};
};
```

Now we can use the constructor to make our 'Student' program a lot simpler.

```js
.needs <std>

import * from "io" under io;

class Student {
	int id;
	adr name;
	int average;

	Student init(int id, adr name, int average){
		io.print("The constructor has just been called!!!\n");
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};
};

int main(){
	Student s = Student(1, "Jhon Doe", 90);

	io.print("Student information:\n");
	io.print("\tID: "); io.printInt(s.id); io.print("\n");
	io.print("\tName: "); io.print(s.name); io.print("\n");
	io.print("\tAverage: "); io.printInt(s.average); io.print("\n");

	return 0;
};
```

Take a look at our program now, it gives the same output as before but it is much simpler.
lets add another student.

```js
.needs <std>

import * from "io" under io;

class Student {
	int id;
	adr name;
	int average;

	Student init(int id, adr name, int average){
		io.print("The constructor has just been called!!!\n");
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};
};

int main(){
	Student s = Student(1, "Jhon Doe", 90);
	Student s2 = Student(2, "Jane Doe", 80);

	io.print("Student information:\n");
	io.print("\tID: "); io.printInt(s.id); io.print("\n");
	io.print("\tName: "); io.print(s.name); io.print("\n");
	io.print("\tAverage: "); io.printInt(s.average); io.print("\n");

	io.print("\n");

	io.print("Student information:\n");
	io.print("\tID: "); io.printInt(s2.id); io.print("\n");
	io.print("\tName: "); io.print(s2.name); io.print("\n");
	io.print("\tAverage: "); io.printInt(s2.average); io.print("\n");

	return 0;
};
```

The constructor made creating our objects much easier, but we still have a lot of repetitive code.  You can see that identical code is being used to print out the student information.  In order to make this simpler, we can create a class method to print out the student information.

```js
.needs <std>

import * from "io" under io;

class Student {
	int id;
	adr name;
	int average;

	Student init(int id, adr name, int average){
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};

	int printInfo(){
		io.print("Student information:\n");
		io.print("\tID: "); io.printInt(my.id); io.print("\n"); // notice that 'my' is used again to access the fields of the class
		io.print("\tName: "); io.print(my.name); io.print("\n");
		io.print("\tAverage: "); io.printInt(my.average); io.print("\n");
	};
};

int main(){
	Student s = Student(1, "Jhon Doe", 90);
	Student s2 = Student(2, "Jane Doe", 80);

	s.printInfo();
	io.print("\n");
	s2.printInfo();

	return 0;
};
```
As you can see, the code is much cleaner.  We can now call the method to print out the student information. in stead of repeating the same code over and over again.

## Linked Lists
The code that we have here is great, but we can see how it can become very long if the number of students gose up. The aflat standard library provides a LinkedList class.  The LinkedList class is meant to be used as a list of objects.  In stead of using 's1' 's2' and so on, lets use a linked list to hold our students.

```js
.needs <std>

import * from "io" under io;
import LinkedList from "Collections";  // First we import the LinkedList class from the Collections module.

class Student {
	int id;
	adr name;
	int average;

	Student init(int id, adr name, int average){
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};

	int printInfo(){
		io.print("Student information:\n");
		io.print("\tID: "); io.printInt(my.id); io.print("\n"); // notice that 'my' is used again to access the fields of the class
		io.print("\tName: "); io.print(my.name); io.print("\n");
		io.print("\tAverage: "); io.printInt(my.average); io.print("\n");
	};
};

int main(){
	LinkedList roster = LinkedList(); // We create a new LinkedList object.
	Student s = new Student(1, "John Doe", 90); // we will talk more about when to use 'new' later on.
	roster.append(s); // We add a new student to the roster.
	s = new Student(2, "Jane Doe", 80);
	roster.append(s); // We add a new student to the roster.

	// We can use the 'forEach' method of the LinkedList class to iterate through the list.
	roster.forEach([Student student] => {
		student.printInfo();
		io.print("\n");
	});  // I know that the 'forEach' method can look a bit confusing. For now, just know that it iterates through the list and calls the function you pass to it.

	return 0;
};
```

Now we can add as many students as we want to the roster, and keep the code clean.  Try adding a few more students to the roster.

## Modules
It is helpful to hide our classes in another file, It helps to give us less to look at when we are working on the program.  in order to create a module, we can type the following in the terminal: 

```bash
aflat module Student
```

this will create a new file called 'Student.af' in the src folder. Copy the Student class from the previous section and paste it into the new file. 

```js
.needs <std>
import * from "io" under io;

class Student {
	int id;
	adr name;
	int average;

	Student init(int id, adr name, int average){
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};

	int printInfo(){
		io.print("Student information:\n");
		io.print("\tID: "); io.printInt(my.id); io.print("\n"); // notice that 'my' is used again to access the fields of the class
		io.print("\tName: "); io.print(my.name); io.print("\n");
		io.print("\tAverage: "); io.printInt(my.average); io.print("\n");
	};
};
```

Now it can bee imported into our main program the same way we would import any class.

```js
.needs <std>

import * from "io" under io;
import LinkedList from "Collections";  // First we import the LinkedList class from the Collections module.
import Student from "./src/Student";  // Then we import the Student class from the Student module.

int main(){
	LinkedList roster = LinkedList();
	Student s = new Student(1, "John Doe", 90); // we will talk more about when to use 'new' later on.
	roster.append(s); 
	s = new Student(2, "Jane Doe", 80);
	roster.append(s);

	// We can use the 'forEach' method of the LinkedList class to iterate through the list.
	roster.forEach([Student student] => {
		student.printInfo();
		io.print("\n");
	});  // I know that the 'forEach' method can look a bit confusing. For now, just know that it iterates through the list and calls the function you pass to it.

	return 0;
};
```

We should see that the code is much easier to look at, and we get the same output as before.

## A Class Challenge
Now that we know how to make modules and classes, see if you can create an Assignment module. with an Assignment class.  The assignment class should have the following fields:
	- adr name
	- int grade
The constructor should take in the name and the grade and set the fields.

## Adding to the student class
Now that we have the Assignment class, add a list of assignments as a field in our student class.  The list should be a LinkedList.  Our new student module should look like this:

```js
.needs <std>
import * from "io" under io;
import Assignment from "./src/Assignment"; // Here, we import the assignment class
import LinkedList from "Collections"; // We also need to import the LinkedList class

class Student {
	int id;
	adr name;
	int average;
    LinkedList assignments = new LinkedList(); // Notice that we create a new linkedList object and initialize it to an empty list

	Student init(int id, adr name, int average){
		my.id = id; // 'my' is a way of accessing the fields of the class
		my.name = name;
		my.average = average;
	};

	int printInfo(){
		io.print("Student information:\n");
		io.print("\tID: "); io.printInt(my.id); io.print("\n"); // notice that 'my' is used again to access the fields of the class
		io.print("\tName: "); io.print(my.name); io.print("\n");
		io.print("\tAverage: "); io.printInt(my.average); io.print("\n");
	};
};
```

Now we should create a method to add an assignment to the student's list of assignments.  The method should take in an assignment object and add it to the list.

```js
.needs <std>
import * from "io" under io;
import Assignment from "./src/Assignment"; // Here, we import the assignment class
import LinkedList from "Collections"; // We also need to import the LinkedList class

class Student {
	int id;
	adr name;
	int average;
    LinkedList assignments = new LinkedList(); // Notice that we create a new linkedList object and initialize it to an empty list

	Student init(int id, adr name, int average){
		my.id = id;
		my.name = name;
		my.average = average;
	};

	int printInfo(){
		io.print("Student information:\n");
		io.print("\tID: "); io.printInt(my.id); io.print("\n"); // notice that 'my' is used again to access the fields of the class
		io.print("\tName: "); io.print(my.name); io.print("\n");
		io.print("\tAverage: "); io.printInt(my.average); io.print("\n");
	};

    int addAssignment(Assignment a){
        my.assignments.append(a);
    };
};
```

We chould also add a way to update the student's average each time we add an assignment. here is how we change the addAssignment method:
We will need to add a total variable to the student class.  This variable will keep track of the total of all the assignments.

```js
int total; // add this code to the student class
```

now we can calculate the average by dividing the total by the number of assignments.
```js
    int addAssignment(Assignment a){
        my.assignments.append(a);
        my.total = my.total + a.grade;
        int count = my.assignments.size();
        my.average = my.total / count;
    };
```

Now lets rewrite our main program to use our new student module.

```js
.needs <std>

import * from "io" under io;
import Student from "./src/Student";  // First we import the Student class from the Student module.
import Assignment from "./src/Assignment";  // Then we import the Assignment class from the Assignment module.

int main(){
	Student s = new Student(1, "John Doe", 0);

	Assignment a1 = new Assignment(100, "Math test");
	Assignment a2 = new Assignment(90, "English test");

	s.addAssignment(a1);
	s.addAssignment(a2);
	s.printInfo();
	return 0;
};
```

Now we can see that the updated average is correct. Try adding some more assignments and see if the average is still correct.

## Encapsulation
You may have noticed that there is no reason for anything outside of the student class to update the average.  What if a student finds our main module and adds code to update his grade... we cant have that.  That is why we use encapsulation.  We can hide the average field from the rest of the code by makeing it private. as follows:

```js
class Student {
	int id;
	adr name;
	private int total; // we can also make total private since it is not used anywhere else
	private int average;
    LinkedList assignments = new LinkedList(); // Notice that we create a new linkedList object and initialize it to an empty list

	Student init(int id, adr name, int average){
		my.id = id;
		my.name = name;
		my.average = average;
	};

	int printInfo(){
		io.print("Student information:\n");
		io.print("\tID: "); io.printInt(my.id); io.print("\n"); // notice that 'my' is used again to access the fields of the class
		io.print("\tName: "); io.print(my.name); io.print("\n");
		io.print("\tAverage: "); io.printInt(my.average); io.print("\n");
	};

    int addAssignment(Assignment a){
        my.assignments.append(a);
    };
};
```
Now if you try to use the average field outside of the student class, you will get an error.  This is because the average field is private.

If you want to access the average field, you can create a getter method that returns the average.  The getter method should look like this:

```js
int getAverage(){
	return my.average;
};
```

Now we can read the getAverage method and see that the average value.

## Building out a system
Now lets build out a student roster system. Delete everything in the main module.  add the following imports:

```js
.needs <std>

import * from "io" under io;
import Student from "./src/Student";  // First we import the Student class from the Student module.
import Assignment from "./src/Assignment";  // Then we import the Assignment class from the Assignment module.
import LinkedList from "Collections";
import String from "String";
import * from "String" under str;

```

Now we will make our main method and create the student roster list


```js
int main(){
	LinkedList roster = new LinkedList();
};
```

Now we should add an infinite loop to our main method.  This loop will keep running until the user chooses to quit.

```js
int main(){
	LinkedList roster = new LinkedList();

	while true { // things inside of this loop will run forever

	};
};
```

Inside of the loop you can print out all of the student information.  You can use the printInfo method from the student class.

```js
int main(){
	LinkedList roster = new LinkedList();

	while true { // things inside of this loop will run forever
		
		// print all students in the list
		roster.forEach([Student s, int index] => {
			s.printInfo();
		});
	};
};
```
Note that the Student.printInfo method has been changed to the following for formatting reasons: 
```js
	int printInfo(){
		io.print("ID: "); io.printInt(my.id); // notice that 'my' is used again to access the fields of the class
		io.print("\tName: "); io.print(my.name);
		io.print("\tAverage: "); io.printInt(my.average);
		io.print("\n");
	};
```

now we will print out a menu to the user: 

```js
int main(){
	LinkedList roster = LinkedList();


	while true { // we use while true to repeat the menu until the user chooses to exit.

		// print all students in the list
		roster.forEach([Student s, int index] => {
			s.printInfo();
		});


		// Print out a menu of options.
		io.print("Menu : 1. Add a student | 2. Add Grade | quit\n> ");
		String choice = str.getString();
	};
	return 0;
};
```

Now we need to create a function to add a student to the roster.  The function will take in a LinkedList of students and add a new student to the list.  This function should go under the imports and over the main function.

```js
// This a function to add a student to the list.
int addStudent(LinkedList roster){
	io.print("Enter the student's name: ");
	String name = str.getString();
	int id = roster.size();
	int average = 0;
	Student student = new Student(id, name.getPrimitive(), average);
	roster.append(student);
};
```

Now we can call it from the main loop.

```js
	while true { // we use while true to repeat the menu until the user chooses to exit.

		// print all students in the list
		roster.forEach([Student s, int index] => {
			s.printInfo();
		});


		// Print out a menu of options.
		io.print("Menu : 1. Add a student | 2. Add Grade | quit\n> ");
		String choice = str.getString();
		if choice == "1"{
			addStudent(roster);
		} else {
			io.print("Invalid choice.\n");
		};
	};
```

Next we will add an addGrade function that will add an assignment to the student. This should go under the imports and over the main function.

```js
// Add Grade
int addGrade(LinkedList roster){
	io.print("Enter the student's ID: ");
	String id = str.getString(); // Get the student's ID from the user.
	int index = id.toInt(); // Convert the ID to an integer.
	if index >= roster.size() { // If the ID is greater than the size of the list, then the student doesn't exist.
		io.print("Student not found.");
		return 0; // quit the function
	};
	Student student = roster.get(index); // use the LinkedList.get() function to get the student from the list at the index.
	io.print("Enter the assignment's name: ");
	String name = str.getString(); // Get the assignment's name from the user.
	io.print("Enter the assignment's grade: ");
	String grade = str.getString(); // Get the assignment's grade from the user.
	int intGrade = grade.toInt();
	
	Assignment assignment = new Assignment(intGrade, name.getPrimitive()); // Create a new Assignment object.
	student.addAssignment(assignment); // Add the assignment to the student.

	return 0;
};
```

Now we can call it from the main function.  and we can call the addGrade function to the main function and add the exit option.

```js
	while true { // we use while true to repeat the menu until the user chooses to exit.

		// print all students in the list
		roster.forEach([Student s, int index] => {
			s.printInfo();
		});


		// Print out a menu of options.
		io.print("Menu : 1. Add a student | 2. Add Grade | quit\n> ");
		String choice = str.getString();
		if choice == "1"{
			addStudent(roster);
		} else if choice == "2"{
			addGrade(roster);
		} else if choice == "quit" {
			return 0; // quit
		} else {
			io.print("Invalid choice.\n");
		};
		io.print("\n\n");
	};
```

All togeather, the main module should look like this:

```js
.needs <std>

import * from "io" under io;
import Student from "./src/Student";  // First we import the Student class from the Student module.
import Assignment from "./src/Assignment";  // Then we import the Assignment class from the Assignment module.
import LinkedList from "Collections";
import String from "String";
import * from "String" under str;

// This a function to add a student to the list.
int addStudent(LinkedList roster){
	io.print("Enter the student's name: ");
	String name = str.getString();
	int id = roster.size();
	int average = 0;
	Student student = new Student(id, name.getPrimitive(), average);
	roster.append(student);
};

// Add Grade
int addGrade(LinkedList roster){
	io.print("Enter the student's ID: ");
	String id = str.getString(); // Get the student's ID from the user.
	int index = id.toInt(); // Convert the ID to an integer.
	if index >= roster.size() { // If the ID is greater than the size of the list, then the student doesn't exist.
		io.print("Student not found.");
		return 0; // quit the function
	};
	Student student = roster.get(index); // use the LinkedList.get() function to get the student from the list at the index.
	io.print("Enter the assignment's name: ");
	String name = str.getString(); // Get the assignment's name from the user.
	io.print("Enter the assignment's grade: ");
	String grade = str.getString(); // Get the assignment's grade from the user.
	int intGrade = grade.toInt();
	
	Assignment assignment = new Assignment(intGrade, name.getPrimitive()); // Create a new Assignment object.
	student.addAssignment(assignment); // Add the assignment to the student.

	return 0;
};

int main(){
	LinkedList roster = LinkedList();


	while true { // we use while true to repeat the menu until the user chooses to exit.

		// print all students in the list
		roster.forEach([Student s, int index] => {
			s.printInfo();
		});


		// Print out a menu of options.
		io.print("Menu : 1. Add a student | 2. Add Grade | quit\n> ");
		String choice = str.getString();
		if choice == "1"{
			addStudent(roster);
		} else if choice == "2"{
			addGrade(roster);
		} else if choice == "quit" {
			return 0; // quit
		} else {
			io.print("Invalid choice.\n");
		};
		io.print("\n\n");
	};
	return 0;
};
```

Play around with the program and see how it works.

## Done So Soon?
Don't worry, more tutorial to come...