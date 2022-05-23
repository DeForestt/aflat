# String
The 'String' class is a wrapper around a string pointer which provides memory managment and other useful features.

## API Functions
### Matches
Matches returns true if the string matches the given pattern.
signature: 
```js
export bool matches(String input, adr pattern)
```

```js
import String from "String";
import { matches } from "String" under str;
String number = "111-111-1111";
bool result = matches(number, "ddd-ddd-dddd"); // true
```

### Consume
Consume gets string input that matches the given pattern.
signature: 
```js
export String consume(adr pattern, * adr message)
```


```js
import String from "String";
import { consume } from "String" under str;
String number = consume("ddd-ddd-dddd", "Please enter your phone number: "); // this will attempt to consume a phone number from the stdin
```
## Constructor
The constructor takes a string literal or a pointer to a character array and returns a 'String' object.

```js
String s = new String("Hello World!");
```

It is suggested to dynamically allocate the string using the 'new' keyword so that the delete operator can be used to free the memory.  Keep in mind that the constructor copies the string, so if the string is modified after the constructor is called, the 'String' object will not reflect the changes.


## Destructor
The destructor frees the memory allocated by the 'String' object and the internal char array.

```js
String s = new String("Hello World!");
delete s;
```
## Fields

The 'String' class does not provide any public fields.

## Methods

### Concat
The concat method returns a new 'String' object which is the concatenation of the current string and the parameter string. This method does not mutate either of the strings.

signature:
```js
String concat(String s)
```

```js
String s = new String("Hello ");
String s2 = new String("World!");
String s3 = s.concat(s2); // s3 = "Hello World!"
```
The concat method overloads the '&' operator.

```js
String s = new String("Hello ");
String s2 = new String("World!");
String s3 = s & s2; // s3 = "Hello World!"
```

### Stitch
The stitch method returns a new 'String' object which is a concatenation of the current String and the parameter character array. This method does not mutate either of the strings.
signature:
```js
String stitch(adr s);
```

```js
String s = new String("Hello ");
String s2 = s.stitch("World!"); // s2 = "Hello World!"
```
The stitch method overloads the '+' operator.

```js
String s = new String("Hello ");
String s2 = s + "World!"; // s2 = "Hello World!"
```

### Copy
The copy method copies the value of the argument String object into the current String object.  This method does not mutate the argument String object.
signiture:
```js
String copy<<=>>(String s);
```

```js
String s = new String("Hello ");
String s2 = new String("World!");
s.copy(s2); // s = "World!"
```
The copy method overloads the '=' operator.

```js
String s = new String("Hello ");
String s2 = new String("World!");
s = s2; // s = "World!"
```

### Replace
The replace returns a new 'String' object which is the result of replacing all occurrences of the parameter character with the replacement character.  This method does not mutate the current String object.
signature:
```js
String replace(char o, char n);
```

```js
String s = new String("Hello World!");
String s2 = s.replace('o', 'O'); // s2 = "HellO WOrld!"
```

### Split
The split method returns a LinkedList of new String objects which are the result of splitting the current String object at the parameter character.  This method does not mutate the current String object.  If the parameter character is not provided, the split method splits the current String object at every space character. The split method does not mutate the current String object.
signature:
```js
LinkedList split(* char c);
```

```js
String s = new String("This is a string to split");
LinkedList l = s.split(' '); // l = ["This", "is", "a", "string", "to", "split"] <= String objects not string literals
```

### ToLower
The toLower method returns a new 'String' object which is the result of converting the current String object to lower case.  This method does not mutate the current String object.
signature:
```js
String toLower();
```

```js
String s = new String("Hello World!");
String s2 = s.toLower(); // s2 = "hello world!"
```

### ToUpper
The toUpper method returns a new 'String' object which is the result of converting the current String object to upper case.  This method does not mutate the current String object.
signature:
```js
String toUpper();
```

```js
String s = new String("Hello World!");
String s2 = s.toUpper(); // s2 = "HELLO WORLD!"
```

### Trim
The trim method returns a new 'String' object which is the result of removing all leading and trailing occurrences of the parameter character from the current String object. If the parameter character is not given, it will assume ' '.  This method does not mutate the current String object.
signature:
```js
String trim(* char c);
```

```js
String s = new String("***Hello World!***");
String s2 = s.trim('*'); // s2 = "Hello World!"
```

### At
The at method returns the character at the specified index.  This method does not mutate the current String object.
signature:
```js
char at(int i);
```

```js
String s = new String("Hello World!");
char c = s.at(4); // c = 'o'
```

### GetPrimitive
The getPrimitive method returns a pointer to the underlying character array. It is suggested to use this method only when absolutely necessary.
signature:
```js
adr getPrimitive();
```

```js
String s = new String("Hello World!");
adr s2 = s.getPrimitive(); // s2 = "Hello World!"
```

### SubString
The subString method returns a new 'String' object which is a substring of the current String object between the specified indices.  This method does not mutate the current String object.
signature:
```js
String subString(int start, int end);
```

```js
String s = new String("Hello World!");
String s2 = s.subString(4, 8); // s2 = "World"
```

### Loop
The loop method loops through the current String object and calls the parameter function for each character. The loop method can also pass an optional pointer parameter to the parameter function.  The loop method passes the current character, the current index, and the optional pointer to the parameter function to the parameter function.
signature:
```js
int loop(adr foo, * adr args)
```

```js
String s = new String("Hello World!");
adr arg = // optional pointer to pass to the parameter function
s.loop([char c, int index, adr args] {
    // do something with c, index, and args
}, arg);
```

### Compare
The compare method returns true if the current String object is equal to the parameter String Object.  This method does not mutate the current String object.
signature:
```js
bool compare(String s);
```

```js
String s = new String("Hello World!");
String s2 = new String("Hello World!");
bool b = s.compare(s2); // b = true
```

### Equal
The equal method returns true if the current String object is equal to the parameter char array pointer.  This method does not mutate the current String object. This method overloads the '==' operator.
signature:
```js
bool equal<<==>>(adr s);
```

```js
String s = new String("Hello World!");
bool b = s.equal("Hello World!"); // b = true
```

The equals method overloads the '==' operator.

```js
String s = new String("Hello World!");
bool b = (s == "Hello World!"); // b = true
```
### IsNumeric
The isNumeric method returns true if the current String object is a numeric value.  This method does not mutate the current String object.
signature:
```js
bool isNumeric();
```

```js
String s = new String("123");
bool b = s.isNumeric(); // b = true
```

### IndexOf
The indexOf method returns the index of the first occurrence of the parameter character in the current String object.  This method does not mutate the current String object.
signature:
```js
int indexOf(char c);
```

```js
String s = new String("Hello World!");
int i = s.indexOf('o'); // i = 4
```

### toInt
The toInt method returns the integer value of the current String object. Will panic if the current String is not a numeric value "Attempt to convert non-numeric string to int". This method does not mutate the current String object.
signature:
```js
int toInt();
```

```js
String s = new String("123");
int i = s.toInt(); // i = 123
```

Len
The length method returns the length of the current String object.  This method does not mutate the current String object.
signature:
```js
int len();
```

```js
String s = new String("Hello World!");
int i = s.len(); // i = 12
```