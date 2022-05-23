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