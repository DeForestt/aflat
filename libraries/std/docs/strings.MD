# strings.af Documentation

## Overview
`strings.af` is a part of the Aflat standard library, providing a collection of functions for string manipulation and character handling.

## Functions

### `ascii`
Converts an integer to its ASCII character representation.

- **Signature**: `char ascii(const int input)`
- **Parameters**:
  - `input`: Integer to convert.
- **Returns**: The ASCII character.

### `len`
Calculates the length of a string.

- **Signature**: `int len(adr input)`
- **Parameters**:
  - `input`: Address of the string.
- **Returns**: Length of the string.

### `str_comp`
Compares two strings for equality.

- **Signature**: `int str_comp(adr str1, adr str2)`
- **Parameters**:
  - `str1`: Address of the first string.
  - `str2`: Address of the second string.
- **Returns**: 1 if equal, 0 otherwise.

### `str_concat`
Concatenates two strings.

- **Signature**: `int str_concat(adr str1, adr str2, adr pointer)`
- **Parameters**:
  - `str1`: Address of the first string.
  - `str2`: Address of the second string.
  - `pointer`: Address where the concatenated string will be stored.
- **Returns**: 0 on success.

### `str_toInt`
Converts a string to an integer.

- **Signature**: `int str_toInt(adr input)`
- **Parameters**:
  - `input`: Address of the string.
- **Returns**: The integer value.

### `int_toString`
Converts an integer to a string.

- **Signature**: `int int_toString(int input, adr buffer)`
- **Parameters**:
  - `input`: Integer to convert.
  - `buffer`: Address where the string representation will be stored.
- **Returns**: 0 on success.

### `int_toStringL0`
Converts an integer to a string with leading zeros.

- **Signature**: `int int_toStringL0(int input, adr buffer, const int size)`
- **Parameters**:
  - `input`: Integer to convert.
  - `buffer`: Address where the string representation will be stored.
  - `size`: Total size of the output string including leading zeros.
- **Returns**: 0 on success.

### `str_find`
Finds the first occurrence of a character in a string.

- **Signature**: `int str_find(adr in, const char find)`
- **Parameters**:
  - `in`: Address of the string.
  - `find`: Character to find.
- **Returns**: Index of the character, or -1 if not found.

### `str_split`
Splits a string at the first occurrence of a separator character.

- **Signature**: `int str_split(adr str, const char sep, adr buff1, const adr buff2)`
- **Parameters**:
  - `str`: Address of the string to split.
  - `sep`: Separator character.
  - `buff1`: Address where the first part of the split string will be stored.
  - `buff2`: Address where the second part of the split string will be stored.
- **Returns**: 0 on success, -1 if the separator is not found.

### `charToFloat`
Converts a character to a float.

- **Signature**: `float charToFloat(const char a)`
- **Parameters**:
  - `a`: Character to convert.
- **Returns**: The float value.

### `str_subString`
Extracts a substring from a string.

- **Signature**: `adr str_subString(const int start, const int length, adr str, const adr buff)`
- **Parameters**:
  - `start`: Starting index.
  - `length`: Length of the substring.
  - `str`: Address of the original string.
  - `buff`: Address where the substring will be stored.
- **Returns**: Address of the substring.

### `str_reverse`
Reverses a string.

- **Signature**: `adr str_reverse(const adr in, adr buff)`
- **Parameters**:
  - `in`: Address of the string to reverse.
  - `buff`: Address where the reversed string will be stored.
- **Returns**: Address of the reversed string.

### `str_at`
Returns the character at a specified index in a string.

- **Signature**: `char str_at(adr str, const int i)`
- **Parameters**:
  - `str`: Address of the string.
  - `i`: Index of the character.
- **Returns**: The character at the specified index.

### `str_forEach`
Applies a function to each character of a string.

- **Signature**: `int str_forEach(const adr s, const adr foo, const adr args)`
- **Parameters**:
  - `s`: Address of the string.
  - `foo`: Function to apply.
  - `args`: Additional arguments for the function.
- **Returns**: 0 on completion.

### `char_isDigit`
Checks if a character is a digit.

- **Signature**: `bool char_isDigit(const char c)`
- **Parameters**:
  - `c`: Character to check.
- **Returns**: `true` if the character is a digit, `false` otherwise.

### `hex_toInt`
Converts a hexadecimal string to an integer.

- **Signature**: `int hex_toInt(adr hex)`
- **Parameters**:
  - `hex`: Address of the hexadecimal string.
- **Returns**: The integer value.

## Usage
The functions in `strings.af` are essential for string handling and character manipulation in the Aflat programming language. They provide a wide range of operations for string processing, making them invaluable for text-based applications.
