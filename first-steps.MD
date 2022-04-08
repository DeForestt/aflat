# First Steps with AFlat

## Installation
Please see the [installation instructions](README.md) for the steps to download and build.  If aflat is already installed, then you can move on the the next section.

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
    io.print("Hello, world!");
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

## Done So Soon?
Don't worry, more tutorial to come...