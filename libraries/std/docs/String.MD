# String.af Documentation

## Overview
`String.af` is a core component of the Aflat standard library, focused on string manipulation and processing. This file defines both the `String` and `string` classes. The `String` class is deprecated and should be used with caution.

## Class: `String` (Deprecated)
The `String` class is the original string handling class in Aflat. It is now deprecated in favor of the `string` class.

### Methods
- `init(adr value)`: Initializes a string with the given address value.
- `concat(String s)`: Concatenates the current string with another string.
- `stitch(adr s)`: Stitches the current string with an address.
- `copy(String s)`: Copies the given string.
- `replace(char o, char n)`: Replaces a character in the string with another character.
- `split(* char c)`: Splits the string based on a delimiter character.
- `toLower()`: Converts the string to lowercase.
- `toUpper()`: Converts the string to uppercase.
- `trim(* char ch)`: Trims characters from the string.
- `at(int i)`: Returns the character at a specified index.
- `getPrimitive()`: Gets the primitive address of the string.
- `subString(int start, int end)`: Extracts a substring from the string.
- `loop(adr foo, * adr args)`: Loops through the string.
- `compare(String s)`: Compares the current string with another string.
- `equal(adr s)`: Checks if the current string is equal to another string.
- `isNumeric()`: Checks if the string is numeric.
- `indexOf(char c)`: Finds the index of a character in the string.
- `toInt()`: Converts the string to an integer.
- `len()`: Returns the length of the string.
- `del()`: Deletes the string.

## Class: `string`
The `string` class is a dynamic and safe class for handling strings in Aflat. It offers a variety of methods for string manipulation.

### Constructor
- `string init(const adr head)`: Initializes a string with the given address.

### Methods
- `string seal()`: Seals the string, making it permanent.
- `int del()`: Deletes the string, managing memory appropriately.
- `bool compare(const string other)`: Compares the current string with another string.
- `string get()`: Increments the reference count and returns the string.
- `string cpy(const string other)`: Copies the string from another string object.
- `char at(const int index)`: Returns the character at the specified index.
- `adr cstr()`: Returns the C-style string (character array).
- `int copyTo(const adr buff)`: Copies the string to the provided buffer.
- `bool input(const adr prompt)`: Takes user input after displaying a prompt.
- `int getRefcount()`: Returns the reference count of the string.
- `string concat(const string other)`: Concatenates the current string with another string.
- `List split(const char delim)`: Splits the string by the specified delimiter.
- `int forEach(const adr func, * const adr args)`: Applies a function to each character of the string.
- `int len()`: Returns the length of the string.
- `string copy()`: Creates a copy of the string.
- `bool isNumeric()`: Checks if the string is numeric.
- `Result toInt()`: Converts the string to an integer, returning a `Result`.
- `string toUpper()`: Converts the string to uppercase.
- `string toLower()`: Converts the string to lowercase.
- `string title()`: Converts the first character of each word to uppercase and the rest to lowercase.
- `string replaceChar(const char old, const char n)`: Replaces a specified character with another character.
- `int find(const string sub, const int start)`: Finds a substring starting from a specified index.
- `string replace(const adr substr, const adr ns)`: Replaces a substring with another string.
- `string subString(const int start, const int end)`: Extracts a substring from the string.
- `string trim(* char ch)`: Trims specified characters from the string.
- `int indexOf(const char ch)`: Finds the index of a character in the string.
- `string push(const char ch)`: Appends a character to the string.
- `string insert(const int i, const string n)`: Inserts a string at a specified index.
- `string sort()`: Sorts the characters in the string.
- `int endScope()`: Decrements the reference count and deletes the string if necessary.

### Exported Functions
- `string create_string(const adr head)`: Creates a new string from an address.
- `int print(const string obj)`: Prints the string object.
- `string readString(*const string prompt)`: Reads a string with an optional prompt.
- `string floatToString(const float f)`: Converts a float to a string representation.
- `string fString(const string fmt, * adr args)`: Formats a string based on a format specifier and arguments.
- `int readInt(const string prompt, * int tries)`: Reads an integer with a prompt and specified number of tries.
- `string itos(const int i)`: Converts an integer to a string.

## Usage
The `string` class in `String.af` is essential for handling and manipulating string data in the Aflat programming language. It provides a robust set of methods for common string operations, making it a fundamental tool for developers working with text in Aflat. The `String` class, being deprecated, should be used with caution and is recommended to be replaced with the `string` class in new developments.
