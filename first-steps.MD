# First Steps with AFlat

## Installation

***Remember: If you have already installed AFlat, update the repository on your system.***<br>
Please see the [installation instructions](README.md) for the steps to download and build. If AFlat is already installed, you can move on to the next section. **This tutorial tracks the latest version of AFlat.**

To update AFlat, run the following command:

```bash
aflat update
```

You should see the update output.

### Development Best Practices

When building the compiler from source, use CMake:

```bash
mkdir build && cd build
cmake .. && make
```
Format any C++ changes with clang-format and run `./rebuild-libs.sh` after editing `libraries/std/src`.
Run `./bin/aflat run` to verify everything still works.

## First Project

To create the tutorial project, run the following command in your project directory:

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
repositories, run:

```bash
aflat install <git_repo>
```

This clones the repository and updates the configuration automatically.

Open the `main.af` file in your favorite text editor. The file will look something like this:

```js
.needs <std>
import uni_string from "uni_string";
import {print} from "uni_string" under uni;

fn main() -> int {
    uni.print(`Hello, world!\n`);
    return 0;
};
```

Let's walk through the program line by line.

- The first line is the `.needs` directive. This tells AFlat that it needs the standard library, a collection of useful functions included in the AFlat distribution. The standard library is included in the `aflat/libraries/std/head` directory.
- The second line is the `import uni_string from "uni_string";` command. This tells AFlat to import the `uni_string` class from the `uni_string` library in the `aflat/libraries/std/src` directory. This library contains AFlat's modern string object, `uni_string`, along with associated string utilities.
- The third line is similar to the second, but this time it imports the `print` function from the `uni_string` library. The `under` command tells AFlat that the imported functions will use the specified `uni` namespace. This makes the function available as `uni.print` and helps avoid namespace pollution.
- The fifth line defines the main function. This is the function that is called when the program is run.
- The sixth line calls the `uni.print` function. This function prints the string `Hello, world!` to standard output.
- The seventh line returns 0. This is the value returned to the operating system when the program finishes.

Now type `aflat run` in the terminal to run the program. The program will print `Hello, world!` to standard output.

## Making it your own
Now we will make a few edits to the project. First, it would be nice for the world to know who the "Hello" is from. We do this by adding a new call to the `uni.print` function. Give it a try, and then look at our implementation below. Remember that `\n` represents a newline.

```js
.needs <std>
import uni_string from "uni_string";
import {print} from "uni_string" under uni;

fn main() -> int {
    uni.print("Hello, world!\n");
    uni.print("From the AFlat Team\n");
    return 0;
};
```

## Working with strings
When you show your program to your friends, it will be nice to have the program print out their name. Take a look at the code below.

```js
.needs <std>
import uni_string from "uni_string";
import {print, read_string} from "uni_string" under uni;

fn main() -> int {
	let name = uni.read_string("What is your name? ");
	uni.print(`Hello, {name}.\nFrom the AFlat Team.\n`);
	return 0;
};
```

Let's take a look at the new lines.
- The first new line is the `import uni_string from "uni_string";` command. This tells AFlat to import the `uni_string` data type from the `uni_string` library.
- The second new line is the `import {print, read_string} from "uni_string" under uni;` command. This tells AFlat to import the `print` and `read_string` functions from the `uni_string` library under the `uni` namespace. Calls to these functions must start with `uni.`, as in `uni.print` and `uni.read_string`. Importing functions by name is recommended because wildcard imports can sometimes lead to undefined behavior (this is a bug that is being addressed).
- The third new line is the `let name = uni.read_string("What is your name? ");` command. This tells AFlat to create a variable named `name` and assign it the value returned by `uni.read_string`. The `let` keyword allows AFlat to infer the variable's type. The function accepts an optional `uni_string` prompt and returns a `uni_string`.
- The last new line is the ``uni.print(`Hello, {name}.\nFrom the AFlat Team.\n`);`` command. This tells AFlat to call the `uni.print` function. The function takes a `uni_string` argument and prints it to standard output. The `{name}` is a placeholder for the value of the `name` variable, and `\n` is a newline character.
- Run the program with `aflat run` and see what happens.

## If/else statements
I take it back. We don't want other people to see our program—in fact, we want to make sure that the program runs only if the user enters the correct name. We can do this by adding an if/else statement.

```js
.needs <std>
import uni_string from "uni_string";
import {print, read_string} from "uni_string" under uni;

fn main() -> int {
	let name = uni.read_string("What is your name? ");
	if name == "Name" {
		uni.print(`Hello, {name}. \nFrom the AFlat Team.\n`);
	} else {
		uni.print("I don't know you... \nGo away!\n");
	};

	return 0;
};
```

The if statement checks whether the two `uni_string` values are equal. If they are equal, the if branch is executed. If they are not equal, the else branch is executed. Keep in mind that the else branch is optional. Also, if a branch contains only one statement, the curly braces are optional.


## Guessing Game
Now we will make a simple guessing game. The program will ask the user to guess a number. If the user guesses the number, the program will tell them they won. If the guess is wrong, the program will say whether it was too high or too low. Explanations are included in the comments.

```js
.needs <std>

import uni_string from "uni_string";
import {read_string, print} from "uni_string" under uni;

fn main() -> int {

    // Declare a variable that will hold the secret number.
    // We explicitly use `int` instead of `let` because numeric literals
    // can represent several numeric types (int, long, short, etc.).
    // Being explicit makes our intent clear and avoids ambiguity.
    int secret = 42;

    // Ask the user for input.
    //
    // read_string() displays the prompt and waits until the user presses Enter.
    // Whatever the user types is returned as a string.
    let guess = uni.read_string("Enter your guess: ");

    // The user's input is text, but we need a number before we can compare it.
    //
    // to_int() attempts to convert the string into an integer.
    // Because the conversion can fail (for example, if the user types "hello"),
    // it returns a `result<int>` rather than an int directly.
    //
    // We use `match` to handle both possible outcomes.
    let guessInt = match guess.to_int() {

        // The conversion succeeded.
        // Extract the integer from the Ok value and use it.
        Ok(i) => i,

        // The conversion failed.
        // Stop the program immediately with an error message.
        Err(err) => panic("Error converting string to int")
    };

    // Both `guessInt` and `secret` are now integers,
    // so we can compare them with comparison operators.

    // `==` checks if the two values are equal.
    if guessInt == secret
        uni.print("You guessed it!\n")

    // `<` checks if the left side is smaller than the right side.
    else if guessInt < secret
        uni.print("Too low!\n")

    // If neither of the previous conditions matched,
    // the guess must be larger than the secret number.
    else
        uni.print("Too high!\n");

    // Curly braces are optional when an if/else body contains
    // only a single statement. If you need multiple statements,
    // surround them with braces to create a block.

    // Returning 0 tells the operating system that the program
    // completed successfully.
    return 0;
};
```

## Loops
This game is a bit too hard. We cannot expect to guess the number in just one try. We need to keep track of how many guesses the user has made. We can do this by using a loop.

```js
.needs <std>

import uni_string from "uni_string";
import {read_string, print} from "uni_string" under uni;

fn main() -> int {
    // As before, we'll store the secret number in an integer.
    int secret = 42;

    // A `for` loop repeats a block of code.
    //
    // It has three parts:
    //   1. Initialization   -> runs once before the loop starts.
    //   2. Condition        -> checked before every iteration.
    //   3. Iterator         -> runs after each iteration.
    //
    // This loop gives the player up to 10 guesses.
    for mutable int i = 0; i < 10; i = i + 1 {

        // `i` changes every time through the loop, so it must be marked
        // `mutable`. After each guess, `i = i + 1` increments the counter.
        let guess = uni.read_string("Enter your guess: ");

        // Convert the user's input into an integer.
        // (See the previous tutorial for a detailed explanation of `match`.)
        let guessInt = match guess.to_int() {
            Ok(value) => value,
            Err(err) => panic("Error converting string to int")
        };

        if guessInt == secret {
            uni.print("You guessed it!\n");

            // `break` immediately exits the nearest loop.
            // Without it, the player would continue guessing even
            // after finding the correct answer.
            break;
        } else if guessInt < secret {
            uni.print("Too low!\n");
        } else {
            uni.print("Too high!\n");
        }
    };

    // Backticks create an interpolated string.
    // Anything inside `{}` is evaluated and inserted into the text.
    uni.print(`The secret number was {secret}\n`);

    return 0;
};
```

The `for` loop looks like this:

```js
for mutable int i = 0; i < 10; i = i + 1 {
	// do something
};
```

The first section creates a variable named `i` and sets it to 0. The second section is the condition, which is checked before each iteration to determine whether the loop should continue. The third section is the increment, which runs after each iteration to increase `i`. The loop continues until the condition is false.

## Random numbers
This game gets boring quickly. We need to make sure that the program doesn't always use the same number. We can do this by using a random number generator.

```js
.needs <std>

import uni_string from "uni_string";
import {read_string, print} from "uni_string" under uni;

// Import the Random class from the math module.
import Random from "math";

fn main() -> int {

    // Create a Random object and ask it for a random integer
    // between 0 (inclusive) and 100 (exclusive).
    //
    // Random() constructs a new Random object.
    // The '.' operator calls one of the object's methods.
    // The number inside the parentheses is an argument passed
    // to the method.
    int secret = Random().nextInt(100);

    for mutable int i = 0; i < 10; i = i + 1 {
        let guess = uni.read_string("Enter your guess: ");

        let guessInt = match guess.to_int() {
            Ok(value) => value,
            Err(err) => panic("Error converting string to int")
        };

        if guessInt == secret {
            uni.print("You guessed it!\n");
            break;
        } else if guessInt < secret {
            uni.print("Too low!\n");
        } else {
            uni.print("Too high!\n");
        }
    };

    // Since the number is now random, reveal it after the game ends
    // so the player knows what the correct answer was.
    uni.print(`The secret number was {secret}\n`);

    return 0;
};
```

Now we have imported the `Random` class from the math library. Its `nextInt(int n)` function returns a random number from 0, inclusive, to `n`, exclusive.


# Classes

A **class** is a blueprint for creating objects. It describes what data an object stores and what functions (called **methods**) belong to it.

Classes are declared with the `class` keyword followed by the class name and a block of code enclosed in curly braces.

```js
class <ClassName> {
    // fields and methods go here
};
```

A class can contain two kinds of members:

* **Fields** - Variables that store the object's data.
* **Methods** - Functions that belong to the class and operate on that data.

Let's create a simple `Student` class.

```js
class Student {

    // Fields that belong to every Student object.
    // Fields that can be modified after the object is created
    // must be marked as `mutable`.

    mutable int id;
    mutable uni_string name = "";
    mutable int average;
};
```

Now let's create a `Student` object and use it.

```js
.needs <std>

import uni_string from "uni_string";
import {print} from "uni_string" under uni;

class Student {
    mutable int id;
    mutable uni_string name = "";
    mutable int average;
};

fn main() -> int {

    // Create a new Student object.
    // Since AFlat can infer the type, we can use `let`.
    let s = Student();

    // Access fields using the '.' operator.
    s.id = 1;
    s.name = "John";
    s.average = 90;

    // Read the fields back and print them.
    uni.print(`Student: {s.name} id: {s.id} average: {s.average}\n`);

    return 0;
};
```

The output should look similar to this:

```text
Student: John id: 1 average: 90
```

Notice that each field is accessed with the **`.` operator**. The same operator is also used to call methods, which we'll learn about next.

## Struct Literals

AFlat also allows objects to be created using a **struct literal**.

Instead of assigning each field individually, you provide the values in the same order that the fields are declared in the class.

```js
.needs <std>

import uni_string from "uni_string";
import {print} from "uni_string" under uni;

class Student {
    // This example intentionally contains only primitive fields.
    // Struct literals should not be used with resource-owning types
    // such as `uni_string`, vectors, maps, or other classes that
    // manage memory.

    mutable int id;
    mutable int average;
};

fn main() -> int {

    // Values are assigned by their position in the class.
    // The first value initializes `id`.
    // The second value initializes `average`.
    Student s = {
        1,
        90
    };

    uni.print(`Student: id: {s.id} average: {s.average}\n`);

    return 0;
};
```

The output should look similar to this:

```text
Student: id: 1 average: 90
```

> **⚠️ Warning**
>
> Struct literals are an advanced feature and should be used sparingly.
>
> Values are assigned **by the order the fields are declared**, not by their names. If you later reorder the fields in the class, every struct literal for that class must also be updated or it will initialize the wrong fields.
>
> More importantly, **struct literals bypass constructors and the ownership system**. Because of this, they cannot safely initialize owning types such as `uni_string`, vectors, maps, or other classes that manage resources. Using them with these types can break ownership semantics and lead to undefined behavior.
>
> Struct literals are intended primarily for simple **Plain Old Data (POD)** classes containing only primitive fields, or for specialized low-level code where you fully understand the tradeoffs.
>
> **For normal AFlat code, always prefer constructors.** They are safer, easier to read, and continue to work correctly as your classes evolve. Unless you're intentionally doing something clever or performance-critical, **don't use struct literals.**

# Constructors and Methods

A **constructor** is a special method that runs automatically whenever a new object is created. Its job is to initialize the object's fields so that every object starts in a valid state.

In AFlat, the constructor must be named **`init`**.

> **Note**
>
> You may notice the `&&` and `$` operators being used with `uni_string`. These are part of AFlat's **ownership system**, which manages memory efficiently and safely. Don't worry about understanding them yet—we'll cover ownership in detail in a later chapter. For now, simply use them as shown in the examples.

```js
.needs <std>

import uni_string from "uni_string";
import {print} from "uni_string" under uni;

class Student {
    mutable int id;
    mutable uni_string name = "";
    mutable int average;

    // The constructor is called automatically whenever
    // a Student object is created.
    fn init(int id, uni_string &&name, int average) -> Student {

        // `my` refers to the object currently being created.
        // Use it to access the object's fields.
        my.id = id;
        my.name = $name;
        my.average = average;

        // Constructors should always return `my`.
        return my;
    };
};

fn main() -> int {

    // Calling Student(...) automatically invokes init().
    let s = Student(1, "John", 90);

    uni.print(`Student: {s.name} id: {s.id} average: {s.average}\n`);

    return 0;
};
```

Notice how much simpler object creation has become. Instead of creating a `Student` and assigning each field individually, we simply pass the values to the constructor.

Constructors become even more useful when creating many objects.

```js
.needs <std>

import uni_string from "uni_string";
import {print} from "uni_string" under uni;

class Student {
    mutable int id;
    mutable uni_string name = "";
    mutable int average;

    fn init(int id, uni_string &&name, int average) -> Student {
        my.id = id;
        my.name = $name;
        my.average = average;
        return my;
    };
};

fn main() -> int {

    let johnDoe = Student(1, "John Doe", 90);
    let janeDoe = Student(2, "Jane Doe", 95);

    uni.print(`Student: {johnDoe.name} id: {johnDoe.id} average: {johnDoe.average}\n`);
    uni.print(`Student: {janeDoe.name} id: {janeDoe.id} average: {janeDoe.average}\n`);

    return 0;
};
```

The constructor removed the repetitive field assignments, but we still have another kind of repetition. The code that prints a student's information is duplicated for every student.

When you find yourself writing the same code over and over again, it's often a sign that the code belongs in a **method**.

A **method** is simply a function that belongs to a class. Like constructors, methods have access to the current object through the `my` variable.

```js
.needs <std>

import uni_string from "uni_string";
import {print} from "uni_string" under uni;

class Student {
    mutable int id;
    mutable uni_string name = "";
    mutable int average;

    fn init(int id, uni_string &&name, int average) -> Student {
        my.id = id;
        my.name = $name;
        my.average = average;
        return my;
    };

    // Print information about this Student.
    fn printInfo() -> void {
        uni.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
    };
};

fn main() -> int {

    let johnDoe = Student(1, "John Doe", 90);
    let janeDoe = Student(2, "Jane Doe", 95);

    // Call a method using the '.' operator.
    johnDoe.printInfo();
    janeDoe.printInfo();

    return 0;
};
```

The program produces the same output as before, but the code is much cleaner. Each `Student` object now knows how to print its own information, so that logic only needs to be written once.

This is one of the biggest advantages of classes: **the data and the functions that operate on that data are kept together.** As your programs grow, this makes your code easier to organize, reuse, and maintain.

# Collections with Vectors

Creating two students is easy, but imagine a classroom with 30 students. Creating a separate variable for every student would quickly become difficult to manage.

Instead, we can store all of our students in a **vector**. A vector is a resizable collection that can hold many objects of the same type.

```js
.needs <std>

import uni_string from "uni_string";
import {print} from "uni_string" under uni;
import vector from "Collections/Vector";

class Student {
    mutable int id;
    mutable uni_string name = "";
    mutable int average;

    fn init(int id, uni_string &&name, int average) -> Student {
        my.id = id;
        my.name = $name;
        my.average = average;
        return my;
    };

    fn printInfo() -> void {
        uni.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
    };
};

fn main() -> int {

    // Create a vector containing two Student objects.
    // The compiler automatically infers that this is a
    // vector of Students.
    let roster = [
        new Student(1, "John Doe", 90),
        new Student(2, "Jane Doe", 80)
    ];

    // `foreach` visits every element in the collection,
    // one at a time.
    foreach student in roster {
        student.printInfo();
    };

    return 0;
};
```

The output should look like this:

```text
Student: John Doe id: 1 average: 90
Student: Jane Doe id: 2 average: 80
```

Notice that we no longer need separate variables like `johnDoe` and `janeDoe`. Instead, all of our students live in a single collection.

The `foreach` loop automatically visits every student in the vector and stores the current element in the `student` variable. As the collection grows, the loop does not need to change.

Try adding a few more students to the vector. You'll notice that the `foreach` loop automatically prints every student without any additional code.


> **Note:** Even though we never explicitly write `vector`, the compiler recognizes the square bracket syntax and automatically creates a `vector<Student>` for us.

# Organizing Code with Modules

As programs grow, keeping every class in `main.af` quickly becomes difficult to manage. A better approach is to place related code into **modules**.

A module is simply another AFlat source file that contains code you want to reuse. By moving each class into its own module, your projects become easier to navigate and maintain.

To create a new module, run the following command from your project's root directory:

```bash
aflat module Student
```

This creates a new module at:

```text
src/
└── Student/
    └── mod.af
```

The package manager also updates your project configuration automatically so the new module will be compiled with your project.

Move the `Student` class from `main.af` into `src/Student/mod.af`.

```js
.needs <std>

import uni_string from "uni_string";
import {print} from "uni_string" under uni;

class Student {
    mutable int id;
    mutable uni_string name = "";
    mutable int average;

    fn init(int id, uni_string &&name, int average) -> Student {
        my.id = id;
        my.name = $name;
        my.average = average;
        return my;
    };

    fn printInfo() -> void {
        uni.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
    };
};
```

Now that the class lives in its own module, we can import it into our program just like any other class from the standard library.

```js
.needs <std>

import Student from "./Student";

fn main() -> int {

    let roster = [
        new Student(1, "John Doe", 90),
        new Student(2, "Jane Doe", 80)
    ];

    foreach student in roster {
        student.printInfo();
    };

    return 0;
};
```

Notice that the `Student` class is no longer defined in `main.af`, yet we can use it exactly as before. Importing a class makes it available to the current source file.

Our program produces the same output, but `main.af` is now much shorter and easier to read. As your projects grow, organizing your code into modules will become increasingly important. A common convention is to place each major class in its own module and keep `main.af` focused on the application's overall flow.

# Class Challenge

You've now learned how to:

* Create classes
* Write constructors
* Organize code into modules

Let's put those skills together!

## Your Challenge

Create a new module named **`Assignment`** by running:

```bash
aflat module Assignment
```

Inside `src/Assignment/mod.af`, create an `Assignment` class with the following fields:

* `uni_string name`
* `int grade`

Your class should also include a constructor that accepts both values and stores them in the object's fields.

When you're finished, try creating a few `Assignment` objects from `main.af` just like you did with the `Student` class.

> **Hint:** Your constructor should look very similar to the one you wrote for `Student`. Only the class name and field names need to change.


# Building Relationships Between Classes

So far, our `Student` class only stores information *about* a student. Real programs often need classes to contain other classes.

In this section, we'll give each `Student` a list of `Assignment` objects. Each assignment has its own name and grade, allowing a student to have any number of assignments.

First, import the `Assignment` module and add a vector to store the student's assignments.

```js
.needs <std>

import uni_string from "uni_string";
import {print} from "uni_string" under uni;
import vector from "Collections/Vector";
import Assignment from "../Assignment";

class Student {
    mutable int id;
    mutable uni_string name = "";
    mutable int average;

    // A Student can have many assignments.
    vector::<Assignment> assignments = new vector::<Assignment>();

    fn init(int id, uni_string &&name, int average) -> Student {
        my.id = id;
        my.name = $name;
        my.average = average;
        return my;
    };

    fn printInfo() -> void {
        uni.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
    };
};
```

Notice that `assignments` is a `vector` of `Assignment` objects. This means each `Student` owns a collection of assignments rather than just a single assignment.

Next, let's add a method that allows assignments to be added to the vector.

```js
fn addAssignment(Assignment &&assignment) -> void {
    my.assignments.push_back($assignment);
};
```

This method accepts an `Assignment` object and appends it to the student's assignment list.

> **Note**
>
> Once again you'll see the `&&` and `$` operators. These are part of AFlat's ownership system. We'll cover them in detail later. For now, simply use them as shown.

Adding assignments is useful, but we'd also like the student's average to stay up to date automatically. To do that, we'll keep track of the total number of points earned.

Add a new field to the `Student` class:

```js
mutable int totalPoints = 0;
```

Now update `addAssignment()` so that it recalculates the average every time a new assignment is added.

```js
fn addAssignment(Assignment &&assignment) -> void {

    my.totalPoints = my.totalPoints + assignment.grade;

    my.assignments.push_back($assignment);

    int count = my.assignments.count();

    my.average = my.totalPoints / count;
};
```

Now every time an assignment is added, the student's average is updated automatically. Code that uses the `Student` class never has to calculate the average itself.

Finally, let's update our program to use the new functionality.

```js
.needs <std>

import Student from "./Student";
import Assignment from "./Assignment";

fn main() -> int {

    let student = new Student(1, "John Doe", 0);

    student.addAssignment(new Assignment("Math Homework", 95));
    student.addAssignment(new Assignment("Science Project", 88));

    student.printInfo();

    return 0;
};
```

The output should show that the student's average has been updated based on the assignments that were added.

Try adding several more assignments with different grades. You should see the average change automatically each time.


# Encapsulation

Our `Student` class automatically updates the student's average whenever a new assignment is added. Because of this, **nothing outside the class should ever modify the average directly**.

Imagine if code elsewhere in the program could do this:

```js
student.average = 100;
```

The average would no longer match the student's assignments, leaving the object in an invalid state.

This is exactly what **encapsulation** is designed to prevent.

We can hide the `average` field by marking it as `private`.

```js
.needs <std>

import uni_string from "uni_string";
import {print} from "uni_string" under uni;
import vector from "Collections/Vector";
import Assignment from "../Assignment";

class Student {
    mutable int id;
    mutable uni_string name = "";

    // Only code inside the Student class can access this field.
    private mutable int average;

    mutable int totalPoints = 0;
    vector::<Assignment> assignments = new vector::<Assignment>();

    fn init(int id, uni_string &&name, int average) -> Student {
        my.id = id;
        my.name = $name;
        my.average = average;
        return my;
    };

    fn printInfo() -> void {
        uni.print(`Student: {my.name} id: {my.id} average: {my.average}\n`);
    };

    fn addAssignment(Assignment &&assignment) -> void {
        my.totalPoints = my.totalPoints + assignment.grade;

        my.assignments.push_back($assignment);

        int count = my.assignments.count();

        my.average = my.totalPoints / count;
    };
};
```

Now, if you try to access `average` from outside the class:

```js
student.average = 100;
```

the compiler will report an error because `average` is **private**.

This allows the `Student` class to control how its average is calculated, preventing other code from accidentally placing the object into an invalid state.

However, there is still a problem. Other code may need to **read** the student's average even though it cannot modify it.

The solution is to provide a **getter** method.

```js
fn getAverage() -> int {
    return my.average;
};
```

Now other code can safely read the student's average without being able to change it.

```js
uni.print(`Average: {student.getAverage()}\n`);
```

This is the main idea behind **encapsulation**:

* Keep an object's internal data private whenever possible.
* Expose methods that provide a safe way to interact with that data.
* Let the class itself maintain its own invariants, so objects cannot accidentally be placed into an invalid state.

As your programs grow, encapsulation makes your code easier to understand, easier to maintain, and much harder to misuse.

## Building a Student Roster System

Now that we have `Student` and `Assignment` classes, we can combine everything we have learned into a small interactive program.

Our roster system will allow the user to:

* View the current students
* Add a new student
* Add an assignment grade to a student
* Exit the program

Start by deleting the contents of `main.af` and adding the imports we will need.

```js
.needs <std>

import uni_string from "uni_string";
import {print, read_string} from "uni_string" under uni;
import vector from "Collections/Vector";
import Student from "./Student";
import Assignment from "./Assignment";
```

### Creating the Roster

Inside `main()`, create a vector that will hold our `Student` objects.

```js
fn main() -> int {
    let roster = new vector::<Student>();

    return 0;
};
```

Unlike the vector literal we used earlier, this creates an empty vector. Students can be added to it while the program is running.

### Creating the Main Loop

Our program should continue running until the user chooses to quit. We can do this with a `while true` loop.

```js
fn main() -> int {
    let roster = new vector::<Student>();

    // `true` never changes, so this loop continues until
    // it is stopped with a `break` statement.
    while true {

    };

    return 0;
};
```

Inside the loop, we can display every student currently in the roster.

```js
fn main() -> int {
    let roster = new vector::<Student>();

    while true {
        foreach student in roster {
            student.printInfo();
        };
    };

    return 0;
};
```

The loop currently runs forever without asking the user to do anything, so let's add a menu.

```js
fn main() -> int {
    let roster = new vector::<Student>();

    while true {
        uni.print("\n-- Student Roster --\n");

        foreach student in roster {
            student.printInfo();
        };

        uni.print("--------------------\n");
        uni.print("1. Add a student\n");
        uni.print("2. Add a grade\n");
        uni.print("Type 'quit' to exit.\n");

        let input = uni.read_string("Enter a command: ");
    };

    return 0;
};
```

Each time the loop runs, the program displays the current roster and waits for a command.

## Adding a Student

Rather than placing all of our logic inside `main()`, we can create a separate function for adding students.

Place this function below the imports and above `main()`.

```js
fn addStudent(vector::<Student> roster) -> void {
    let name = uni.read_string("Enter student name: ");

    // For this simple program, the next ID is based on
    // the number of students already in the roster.
    int id = roster.count() + 1;

    roster.push_back(new Student(id, $name, 0));
};
```

The student begins with an average of `0` because no assignments have been added yet.

> **Ownership note**
>
> The `$` operator transfers ownership of the student's name into the new `Student` object. Ownership will be explained in detail later. For now, use it as shown.

We can call `addStudent()` when the user enters `"1"`.

```js
fn main() -> int {
    let roster = new vector::<Student>();

    while true {
        uni.print("\n-- Student Roster --\n");

        foreach student in roster {
            student.printInfo();
        };

        uni.print("--------------------\n");
        uni.print("1. Add a student\n");
        uni.print("2. Add a grade\n");
        uni.print("Type 'quit' to exit.\n");

        let input = uni.read_string("Enter a command: ");

        if input == "1" {
            addStudent(roster);
        };
    };

    return 0;
};
```

## Adding a Grade

Adding a grade requires a few more steps:

1. Ask for a student ID.
2. Convert the ID from a string to an integer.
3. Find the matching student.
4. Ask for the assignment name and grade.
5. Create the assignment and add it to the student.

Add the following function above `main()`.

```js
fn addGrade(vector::<Student> roster) -> bool {
    let idInput = uni.read_string("Enter student id: ");

    // Convert the user's input into an integer.
    // Return false if the user did not enter a valid number.
    int id = match idInput.to_int() {
        Ok(value) => value,
        Err(err) => {
            uni.print("Invalid ID. Please enter a valid integer.\n");
            return false;
        }
    };

    /*
     * findFirst() searches the vector and returns an option.
     *
     * The anonymous function is called for each Student.
     * It returns true when the student's ID matches the ID
     * entered by the user.
     *
     * The ID is passed into the anonymous function by reference.
     * References will be covered in more detail later.
     */
    let student = match roster.findFirst(
        fn (Student current, int& wantedId) return current.id == wantedId,
        ?id
    ) {
        Some(value) => value,
        None() => {
            uni.print("Student not found.\n");
            return false;
        }
    };

    let assignmentName =
        uni.read_string("Enter assignment name: ");

    let gradeInput =
        uni.read_string("Enter assignment grade: ");

    match gradeInput.to_int() {
        Ok(grade) => {
            // Create the assignment, then transfer ownership
            // to the student's assignment vector.
            let assignment =
                new Assignment($assignmentName, grade);

            student.addAssignment($assignment);

            // Replace the existing student in the roster
            // with the updated Student object.
            roster.set(student.id - 1, $student);

            return true;
        },

        Err(err) => {
            uni.print(
                "Invalid grade. Please enter a valid integer.\n"
            );

            return false;
        }
    };
};
```

The function returns `true` when the assignment is added successfully and `false` if something goes wrong.

There are several newer concepts in this function, including references, anonymous functions, options, and ownership transfers. You do not need to understand every detail yet. For now, focus on the overall flow:

* Read input
* Validate it
* Find the student
* Create the assignment
* Update the roster

## Handling Every Menu Option

We can now complete the main loop by handling the remaining commands.

```js
fn main() -> int {
    let roster = new vector::<Student>();

    while true {
        uni.print("\n-- Student Roster --\n");

        foreach student in roster {
            student.printInfo();
        };

        uni.print("--------------------\n");
        uni.print("1. Add a student\n");
        uni.print("2. Add a grade\n");
        uni.print("Type 'quit' to exit.\n");

        let input = uni.read_string("Enter a command: ");

        if input == "1" {
            addStudent(roster);
        } else if input == "2" {
            if !addGrade(roster) {
                uni.print("Failed to add grade.\n");
            };
        } else if input == "quit" | input == "exit" {
            uni.print("Exiting program.\n");
            break;
        } else {
            uni.print("Invalid command. Please try again.\n");
        };
    };

    return 0;
};
```

The `break` statement exits the infinite loop, allowing the program to finish.

## Complete `main.af`

The complete main module should now look like this:

```js
.needs <std>

import uni_string from "uni_string";
import {print, read_string} from "uni_string" under uni;
import vector from "Collections/Vector";
import Student from "./Student";
import Assignment from "./Assignment";

fn addStudent(vector::<Student> roster) -> void {
    let name = uni.read_string("Enter student name: ");

    int id = roster.count() + 1;

    roster.push_back(new Student(id, $name, 0));
};

fn addGrade(vector::<Student> roster) -> bool {
    let idInput = uni.read_string("Enter student id: ");

    int id = match idInput.to_int() {
        Ok(value) => value,
        Err(err) => {
            uni.print(
                "Invalid ID. Please enter a valid integer.\n"
            );

            return false;
        }
    };

    let student = match roster.findFirst(
        fn (Student current, int& wantedId)
            return current.id == wantedId,
        ?id
    ) {
        Some(value) => value,
        None() => {
            uni.print("Student not found.\n");
            return false;
        }
    };

    let assignmentName =
        uni.read_string("Enter assignment name: ");

    let gradeInput =
        uni.read_string("Enter assignment grade: ");

    match gradeInput.to_int() {
        Ok(grade) => {
            let assignment =
                new Assignment($assignmentName, grade);

            student.addAssignment($assignment);

            roster.set(student.id - 1, $student);

            return true;
        },

        Err(err) => {
            uni.print(
                "Invalid grade. Please enter a valid integer.\n"
            );

            return false;
        }
    };
};

fn main() -> int {
    let roster = new vector::<Student>();

    while true {
        uni.print("\n-- Student Roster --\n");

        foreach student in roster {
            student.printInfo();
        };

        uni.print("--------------------\n");
        uni.print("1. Add a student\n");
        uni.print("2. Add a grade\n");
        uni.print("Type 'quit' to exit.\n");

        let input = uni.read_string("Enter a command: ");

        if input == "1" {
            addStudent(roster);
        } else if input == "2" {
            if !addGrade(roster) {
                uni.print("Failed to add grade.\n");
            };
        } else if input == "quit" | input == "exit" {
            uni.print("Exiting program.\n");
            break;
        } else {
            uni.print("Invalid command. Please try again.\n");
        };
    };

    return 0;
};
```

Run the program and experiment with adding students and assignment grades.

## Challenge

Add a new menu option that allows the user to view every assignment belonging to a particular student.

You may need to:

* Add a method to the `Student` class
* Loop through the student's `assignments` vector
* Print the name and grade of each assignment
* Add another command to the main menu

## Error Handling in AFlat

Some operations can fail.

For example:

* A user may enter text when a number is expected.
* A student ID may not exist.
* A program may attempt to divide by zero.

AFlat represents operations like these with the `result` union.

A result has two possible states:

* `Ok(value)` contains a successful value.
* `Err(error)` contains information about a failure.

The standard `result` union replaces the older `Result` class. The older class stored its value as `any` and relied on reference counting, which required additional allocations and casts. The newer union keeps its success value typed, allowing the compiler to check how it is used.

### Returning a Result

Import `result` and its helper functions from `Utils/result`.

* `accept` creates a successful result.
* `reject` creates an error result.
* `resultWrapper` automatically wraps a plain return value in `Ok`.

A function that may fail can return `result::<T>`. AFlat also provides the shorthand `T!`.

For example:

```js
.needs <std>

import Render from "Utils/Error/Render";
import Error from "Utils/Error";
import result from "Utils/result";
import {accept, reject} from "Utils/result" under res;
import {resultWrapper} from "Utils/result" under result;

fn divide(int a, int b) -> int! {
    if b == 0
        return res.reject::<int>(
            new Error("Cannot divide by zero")
        );

    // Returning a plain int from a function with an `int!`
    // return type automatically wraps the value in `Ok`.
    return a / b;
};
```

The return type `int!` is shorthand for:

```js
result::<int>
```

When the division succeeds, the returned integer is automatically wrapped in `Ok`. When the division fails, `reject` creates an `Err` result containing an `Error`.

### Handling a Result

Because a result can contain either a successful value or an error, we use `match` to handle both possibilities. Keep the preceding imports and `divide()` function in `main.af`, then add the `uni_string` imports and `main()` function below them.

```js
import uni_string from "uni_string";
import {print} from "uni_string" under uni;

fn main() -> int {
    match divide(10, 0) {
        Ok(value) => {
            uni.print(`Value: {value}\n`);
        },

        Err(err) => {
            uni.print(`{err}\n`);
        }
    };

    return 0;
};
```

Unlike an exception system, returning an error does not unwind the stack. An error is simply another value that is returned to the caller and handled explicitly.

## The Bubble Operator

AFlat provides the **bubble operator**, `!`, for functions that return results.

Appending `!` to a result-producing expression does one of two things:

* If the result is `Ok`, it extracts the successful value.
* If the result is `Err`, it immediately returns that error from the current function.

For example:

```js
int value = input.to_int()!;
```

This is similar to writing:

```js
int value = match input.to_int() {
    Ok(number) => number,
    Err(err) => return err
};
```

The bubble operator reduces repetitive error-handling code while preserving explicit result-based control flow.

> **Important**
>
> The bubble operator can only be used inside a function that also returns a compatible result type. The current function must be able to return the error being propagated.

## Adding Error Handling to the Student Roster

Our current `addGrade()` function returns a `bool`. That tells the caller whether the operation succeeded, but it does not explain why it failed.

We can improve it by returning `bool!`, which is shorthand for `result::<bool>`.

The function can then:

* Return `true` when the grade is added.
* Propagate conversion errors with `!`.
* Return a custom error when the student cannot be found.

First, make sure the main module imports `Error` and the result helpers.

```js
import Render from "Utils/Error/Render";
import Error from "Utils/Error";
import result from "Utils/result";
import {accept, reject} from "Utils/result" under res;
import {resultWrapper} from "Utils/result" under result;
```

Now update `addGrade()`:

```js
fn addGrade(vector::<Student> roster) -> bool! {
    let idInput =
        uni.read_string("Enter student id: ");

    // to_int() returns a result.
    // `!` extracts the integer or immediately returns
    // the conversion error to the caller.
    int id = idInput.to_int()!;

    let student = match roster.findFirst(
        fn (Student current, int& wantedId)
            return current.id == wantedId,
        ?id
    ) {
        Some(value) => value,

        // A missing student is not a conversion error, so
        // we create our own error with `reject`.
        None() => {
            return res.reject::<bool>(
                new Error("Student not found")
            );
        }
    };

    let assignmentName =
        uni.read_string("Enter assignment name: ");

    let gradeInput =
        uni.read_string("Enter assignment grade: ");

    // This conversion can also fail, so we propagate its error.
    int grade = gradeInput.to_int()!;

    let assignment =
        new Assignment($assignmentName, grade);

    student.addAssignment($assignment);

    // In this tutorial, student IDs correspond to their
    // positions in the vector.
    roster.set(student.id - 1, $student);

    // Because this function returns `bool!`, the plain bool
    // is automatically wrapped in `Ok`.
    return true;
};
```

Notice that we no longer print errors inside `addGrade()`. The function only performs the operation and reports its result. The caller decides how the error should be shown.

This separation makes the function easier to reuse. A graphical application, for example, could display the same error in a dialog instead of printing it to the terminal.

## Handling the Result in `main()`

Because `addGrade()` now returns a result, `main()` must handle both the success and error cases.

```js
fn main() -> int {
    let roster = new vector::<Student>();

    while true {
        uni.print("\n-- Student Roster --\n");

        foreach student in roster {
            student.printInfo();
        };

        uni.print("--------------------\n");
        uni.print("1. Add a student\n");
        uni.print("2. Add a grade\n");
        uni.print("Type 'quit' to exit.\n");

        let input =
            uni.read_string("Enter a command: ");

        if input == "1" {
            addStudent(roster);

        } else if input == "2" {
            match addGrade(roster) {
                Ok(success) => {
                    uni.print(
                        "Grade added successfully.\n"
                    );
                },

                Err(err) => {
                    uni.print(`{err}\n`);
                }
            };

        } else if input == "quit" | input == "exit" {
            uni.print("Exiting program.\n");
            break;

        } else {
            uni.print(
                "Invalid command. Please try again.\n"
            );
        };
    };

    return 0;
};
```

The `success` value is not especially important here because a successful call always returns `true`. However, `Ok` still contains the value returned by `addGrade()`, so its pattern must bind that value.

The important difference is that the caller now receives useful information when the operation fails.

For example, the error may report that:

* The student ID was not a valid integer.
* No student exists with that ID.
* The assignment grade was not a valid integer.

## Why Use Results?

Returning a `bool` only tells the caller that something failed:

```js
false
```

Returning a result can explain what happened:

```text
Error: Student not found
```

Results allow errors to remain part of normal program flow. They make failure visible in the function's return type and require the caller either to handle the error or deliberately propagate it.

Use results when a function can fail in a meaningful way and the caller may need to know why.

# AFlat Ownership Model

AFlat uses an explicit ownership model to help manage dynamically allocated memory.

The goal is to balance:

* Performance
* Safety
* Control
* AFlat’s philosophy of trusting the programmer

Ownership determines which part of a program is responsible for the lifetime of an object and when that responsibility is transferred elsewhere.

> **Important**
>
> This section is only an introduction to AFlat ownership. It explains enough to understand the examples used throughout this tutorial, but it does not cover every ownership rule, borrowing behavior, invalidation case, or memory-management concern.
>
> For the complete and most current behavior, refer to the AFlat language documentation.

## Which Types Use Ownership?

AFlat’s ownership model applies to non-value types, including user-defined classes and dynamically allocated objects.

Primitive value types are copied normally and are not tracked by the ownership system.

Examples of value types include:

```aflat
int
short
long
float
bool
```

For example:

```aflat
int first = 10;
int second = first;
```

Both variables contain their own copy of the integer.

Classes and other dynamically allocated objects behave differently because they refer to memory that must eventually be released.

## Core Ownership Ideas

There are three important ideas to understand:

1. An owned object has a responsible owner.
2. Ownership can be transferred from one part of the program to another.
3. Only owned values can be transferred or returned as owned values.

AFlat performs many of these checks at compile time, but the programmer is still responsible for using ownership correctly.

## Creating an Owned Object

The `new` keyword dynamically allocates an object and returns an owned value.

```aflat
let student = new Student(1, "John Doe", 90);
```

Because `new Student(...)` creates a new object, the result is owned.

The variable `student` is now responsible for that object until ownership is transferred or the object is otherwise released.

## Transferring Ownership with `$`

The `$` operator transfers ownership of a named variable.

```aflat
let student = new Student(1, "John Doe", 90);

roster.push_back($student);
```

After `$student` is passed to `push_back`, ownership of the object is transferred to the vector.

This operation is sometimes described as **moving** or **selling** the value.

The original variable should no longer be treated as the owner of the object.

```aflat
let assignment = new Assignment("Homework", 95);

student.addAssignment($assignment);
```

Here, ownership of `assignment` is transferred to the `Student` object.

## Ownership Parameters with `&&`

A parameter marked with `&&` accepts ownership of its argument.

```aflat
fn addAssignment(Assignment &&assignment) -> void {
    my.assignments.push_back($assignment);
};
```

The `addAssignment` method takes ownership of the `Assignment` passed into it.

When passing a named owned variable, use `$`:

```aflat
let assignment = new Assignment("Homework", 95);

student.addAssignment($assignment);
```

When passing a newly created object directly, `$` is not required:

```aflat
student.addAssignment(
    new Assignment("Homework", 95)
);
```

The result of `new Assignment(...)` is already an owned temporary value, so it can be passed directly into an ownership-taking parameter.

## Owned Values and Rvalues

The `$` operator is generally required when transferring ownership from a named variable.

```aflat
let assignment = new Assignment("Homework", 95);

student.addAssignment($assignment);
```

It is not required for a newly created temporary value:

```aflat
student.addAssignment(
    new Assignment("Homework", 95)
);
```

Compare the following calls:

```aflat
takeIt(new Foo()); // valid: new Foo() is an owned temporary
takeIt($foo);      // valid: ownership of foo is transferred
takeIt(foo);       // error: ownership was not transferred
```

## Only Owned Values Can Be Transferred

AFlat does not allow `$` to be used on a value that is not owned.

```aflat
fn inspect(Foo value) -> void {
    consume($value); // error: value is not owned
};
```

A regular parameter does not transfer ownership into the function. It is treated as a non-owning value.

To accept ownership, the parameter must use `&&`:

```aflat
fn consume(Foo &&value) -> void {
    // This function owns value.
};
```

## Only Owned Values Can Be Returned as Owned Values

A function cannot safely return ownership of an object it does not own.

For example, returning a regular borrowed parameter as though it were owned would create multiple parts of the program that believe they own the same object.

```aflat
fn invalid(Foo value) -> Foo {
    return value; // error: value is not owned
};
```

A function may return an object it creates:

```aflat
fn createFoo() -> Foo {
    return new Foo();
};
```

The newly created object is owned, so ownership can be transferred to the caller.

The return value of a function that produces an owned object is also treated as an owned temporary:

```aflat
let foo = createFoo();
```

## Ownership and `uni_string`

Earlier examples used syntax like this:

```aflat
fn init(uni_string &&name) -> Student {
    my.name = $name;
    return my;
};
```

The constructor takes ownership of `name`, and then transfers that ownership into the `name` field.

That is why calls such as this are valid:

```aflat
let student =
    new Student(1, "John Doe", 90);
```

The string passed into the constructor becomes owned by the new `Student`.

Ownership is also why struct literals were discouraged earlier. Struct literals bypass constructors and do not correctly participate in the normal ownership-transfer process.

## Moving Fields

If you own an object, AFlat currently allows you to transfer ownership out of one of its fields.

```aflat
let container = new Container();

let value = $container.item;
```

Because `container` is owned, its field may also be treated as movable.

However, this behavior must be used carefully.

> **Warning**
>
> AFlat does not currently perform complete move invalidation tracking. After moving a field, the parent object may still appear to contain that field even though ownership has been transferred.
>
> This can produce double ownership, use-after-move bugs, or multiple attempts to release the same resource.

The programmer is responsible for ensuring that moved values are not used again incorrectly.

This is one of the areas where the current ownership system provides control without attempting to prevent every possible mistake.

## Ownership Summary

| Expression                           |            Owned? | Requires `$` to transfer? | Notes                                      |
| ------------------------------------ | ----------------: | ------------------------: | ------------------------------------------ |
| `new Foo()`                          |               Yes |                        No | A newly allocated owned temporary          |
| `let foo = new Foo();`               |               Yes |                       Yes | A named owned variable                     |
| A function returning an owned object |               Yes |                        No | The returned temporary is owned            |
| A regular function parameter         |                No |               Not allowed | Does not transfer ownership                |
| A parameter marked `&&`              |               Yes |                   Depends | Use `$variable` or pass an owned temporary |
| A primitive value                    |       Not tracked |                        No | Primitive values are copied                |
| A field of an owned object           | Currently movable |                       Yes | Must be used carefully                     |
| A field of a borrowed object         |                No |               Not allowed | Cannot move from a borrowed object         |
| A non-owned return value             |                No |               Not allowed | Cannot return ownership you do not have    |

## Ownership in the Student Roster

The roster program contains several examples of ownership transfers.

### Creating and storing a student

```aflat
roster.push_back(
    new Student(id, $name, 0)
);
```

In this expression:

1. Ownership of `name` is transferred into the `Student`.
2. `new Student(...)` creates an owned student.
3. The new student is passed directly into the vector.
4. The vector becomes responsible for the student.

### Creating and storing an assignment

```aflat
let assignment =
    new Assignment($assignmentName, grade);

student.addAssignment($assignment);
```

In this expression:

1. Ownership of `assignmentName` is transferred into the assignment.
2. The local variable `assignment` owns the new object.
3. `$assignment` transfers ownership into the student.
4. The student's assignment vector becomes responsible for the assignment.

### Replacing an object in the roster

```aflat
roster.set(student.id - 1, $student);
```

The updated student is transferred back into the roster vector.

These ownership transfers may look unusual at first, but they make responsibility explicit. The code shows exactly where an object changes owners.

## Benefits of Explicit Ownership

AFlat’s ownership model provides several benefits:

* Ownership transfers are visible in the source code.
* Many invalid transfers are caught at compile time.
* Ownership checking has no runtime tracing overhead.
* Heap allocation and movement remain under programmer control.
* The model can be extended with stronger borrowing and invalidation checks in the future.

At the same time, ownership does not make unsafe memory behavior impossible. AFlat intentionally gives the programmer substantial control, and some mistakes remain the programmer’s responsibility.

## Where to Learn More

This tutorial has only introduced the ownership syntax needed for the examples:

```aflat
new Foo()
Foo &&value
$value
```

The full ownership model also interacts with:

* Object lifetimes
* Destructors
* Borrowed references
* Immutable values
* Moving fields
* Returning objects
* Containers
* Optional and result values
* Safe and unique classes
* Manual memory management

Refer to the official AFlat documentation for the complete and current ownership rules.

## Done So Soon?

Not quite.

You now know enough AFlat to:

* Create variables and control program flow
* Build classes and constructors
* Organize code with modules
* Store objects in vectors
* Use encapsulation
* Handle errors with results
* Work with basic ownership transfers
* Build a small interactive application

There is still much more to explore, including additional collections, generics, unions, pattern matching, memory management, transforms, concurrency, and other advanced language features.

More tutorials are coming.
