# String
The 'String' class is a wrapper around a string pointer which provides memory managment and other useful features.

## Constructor
The constructor takes a string literal or a pointer to a character array and returns a 'String' object.

```js
String s = new String("Hello World!");
```

It is suggested to dynamically allocate the string using the 'new' keyword so that the delete operator can be used to free the memory.  Keep in mind that the constructor copies the string, so if the string is modified after the constructor is called, the 'String' object will not reflect the changes.

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