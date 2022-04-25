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
	String name;
	int grade;
	float gpa; // a float is a number with a decimal point. ie: 3.14
};

```

## Done So Soon?
Don't worry, more tutorial to come...