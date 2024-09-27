# Documentation for Data Structures and Collections

## Introduction

This document provides an overview and detailed descriptions of several data structures and collections implemented in the provided code. The classes and functions included are:

- `ICollection`
- `ListNode`
- `LinkedList`
- `Array`
- `Keyed`
- `BinTree`
- `List`

Each class and function is explained with its purpose, methods, and usage examples.

---

## Table of Contents

- [ICollection Interface](#icollection-interface)
- [ListNode Class](#listnode-class)
- [LinkedList Class](#linkedlist-class)
  - [Methods](#linkedlist-methods)
- [Array Class](#array-class)
  - [Methods](#array-methods)
- [Keyed Interface](#keyed-interface)
- [BinTree Class](#bintree-class)
  - [Methods](#bintree-methods)
- [List Class](#list-class)
  - [Methods](#list-methods)
- [Helper Functions](#helper-functions)
- [Usage Examples](#usage-examples)
- [Notes](#notes)
- [Conclusion](#conclusion)

---

## ICollection Interface

The `ICollection` interface serves as a contract for collection types. It defines methods that any implementing collection should provide.

### Contract Methods

- **`forEach`**: An abstract method that, when called, throws a panic indicating it's not implemented. Collections should implement this method to iterate over their elements.

  ```pseudo
  adr forEach = [] => panic("Function (forEach) not implemented on this instance");
  ```

- **`where`**: An abstract method that, when called, throws a panic indicating it's not implemented. Collections should implement this method to filter their elements based on a condition.

  ```pseudo
  adr where = [] => panic("Function (where) not implemented on this instance");
  ```

---

## ListNode Class

The `ListNode` class represents a node in a linked list.

### Fields

- **`adr next`**: A reference to the next node in the list.
- **`adr data`**: The data held by the node.

---

## LinkedList Class

The `LinkedList` class implements a singly linked list data structure.

### Fields

- **`public ListNode head`**: The first node in the linked list.
- **`public adr pos`**: A reference to the current position in the list, initialized to `head`.
- **`private adr delFunc`**: A function used to delete nodes' data.
- **`bool cleanUp`**: A flag indicating whether to clean up nodes' data upon deletion.

### LinkedList Methods

#### Initialization

- **`init(adr delFunc, bool cleanUp)`**: Initializes the linked list with an optional delete function and cleanup flag.

  ```pseudo
  LinkedList init(* adr delFunc, * bool cleanUp);
  ```

#### Modification Methods

- **`append(adr data)`**: Adds a new element to the end of the list.

  ```pseudo
  int append(adr data);
  ```

- **`insert(adr data, int index)`**: Inserts a new element at the specified index.

  ```pseudo
  int insert(adr data, int index);
  ```

- **`drop(int index)`**: Removes the element at the specified index.

  ```pseudo
  int drop(int index);
  ```

#### Access Methods

- **`get(int index)`**: Retrieves the element at the specified index.

  ```pseudo
  generic get(int index);
  ```

- **`size()`**: Returns the number of elements in the list.

  ```pseudo
  int size();
  ```

#### Utility Methods

- **`stitch(LinkedList New)`**: Concatenates another linked list to the end of this list.

  ```pseudo
  adr stitch(LinkedList New);
  ```

- **`reverse()`**: Reverses the linked list in place.

  ```pseudo
  LinkedList reverse();
  ```

- **`sort(adr compare, adr args)`**: Sorts the linked list based on the comparison function `compare`.

  ```pseudo
  LinkedList sort(adr compare, * adr args);
  ```

- **`copy(LinkedList list)`**: Copies another linked list into this one.

  ```pseudo
  public LinkedList copy<<=>>(adr list);
  ```

#### Functional Methods

- **`where(adr foo, adr args)`**: Filters the list based on a condition provided by `foo`.

  ```pseudo
  LinkedList where(adr foo, * adr args);
  ```

- **`forEach(adr foo, adr args)`**: Applies a function `foo` to each element in the list.

  ```pseudo
  int forEach(adr foo, * adr args);
  ```

- **`reduce(adr foo, adr args)`**: Reduces the list to a single value using the function `foo`.

  ```pseudo
  adr reduce(adr foo, * adr args);
  ```

- **`find(adr foo, adr args)`**: Finds the index of the first element satisfying the condition in `foo`.

  ```pseudo
  int find(adr foo, * adr args);
  ```

- **`map(adr foo, adr args)`**: Creates a new list with elements transformed by `foo`.

  ```pseudo
  adr map(adr foo, * adr args);
  ```

- **`erace()`**: Removes all elements from the list and frees memory.

  ```pseudo
  int erace();
  ```

#### Destructor

- **`del()`**: Cleans up the list by freeing memory and deleting nodes.

  ```pseudo
  int del();
  ```

---

## Array Class

The `Array` class represents a dynamic array and implements the `ICollection` interface.

### Fields

- **`public int typeSize`**: The size of each element in the array.
- **`public adr head`**: The starting address of the array.
- **`int size`**: The current capacity of the array.

### Array Methods

#### Initialization

- **`init(int typeSize, int size)`**: Initializes the array with element size and initial capacity.

  ```pseudo
  Array init(int typeSize, int size);
  ```

#### Access Methods

- **`at(int index)`**: Returns a reference to the element at the given index.

  ```pseudo
  public adr at(int index);
  ```

#### Modification Methods

- **`del(int index)`**: Deletes the element at the given index.

  ```pseudo
  public int del(int index);
  ```

- **`extend(int size)`**: Extends the array capacity by the given size.

  ```pseudo
  public int extend(int size);
  ```

---

## Keyed Interface

The `Keyed` interface requires implementing classes to have an integer key.

### Contract

- **`public int key`**: An integer key property that must be implemented.

  ```pseudo
  contract {
      public int key;
  };
  ```

---

## BinTree Class

The `BinTree` class represents a binary search tree for elements that implement the `Keyed` interface.

### Fields

- **`const Keyed data`**: The data held by the node, which must have a key.
- **`private BinTree left`**: Reference to the left child node.
- **`private BinTree right`**: Reference to the right child node.

### BinTree Methods

#### Initialization

- **`init(const Keyed data)`**: Initializes the tree with the root data.

  ```pseudo
  public BinTree init(const Keyed data);
  ```

#### Modification Methods

- **`insert(const Keyed data)`**: Inserts a new node into the tree based on the key.

  ```pseudo
  public bool insert(const Keyed data);
  ```

#### Access Methods

- **`find(const int key)`**: Searches for a node with the specified key.

  ```pseudo
  public adr find(const int key);
  ```

#### Utility Methods

- **`traverse()`**: Returns a `LinkedList` of the tree's elements in traversal order.

  ```pseudo
  public LinkedList traverse();
  ```

- **`normalize()`**: Adjusts the tree to a normalized form.

  ```pseudo
  public int normalize();
  ```

---

## List Class

The `List` class represents a dynamic array with additional functionalities like automatic resizing and element manipulation.

### Fields

- **`private adr head`**: Pointer to the array data.
- **`private int typeSize`**: Size of each element.
- **`private int size`**: Current capacity of the array.
- **`private int count`**: Number of elements currently stored.
- **`private adr delFunc`**: Function to delete an element.
- **`private adr ini`**: Function to initialize an element.
- **`public adr stringFunc`**: Function to convert an element to a string.

### List Methods

#### Initialization

- **`init(int typeSize, int size, adr delFunc, adr ini)`**: Initializes the list.

  ```pseudo
  List init(const int typeSize, *const int size, *const adr delFunc, *const adr ini);
  ```

#### Modification Methods

- **`pushBack(adr value)`**: Adds a new element to the end of the list.

  ```pseudo
  int pushBack(const adr value);
  ```

- **`pushValue(any value)`**: Adds a value to the list by value (only for small types).

  ```pseudo
  void pushValue(const any value);
  ```

- **`set(int index, adr value)`**: Sets the element at the specified index.

  ```pseudo
  int set(const int index, const adr value);
  ```

- **`insert(int index, adr value)`**: Inserts an element at the specified index.

  ```pseudo
  int insert(const int index, const adr value);
  ```

- **`drop(int index)`**: Removes the element at the specified index.

  ```pseudo
  int drop(const int index);
  ```

#### Access Methods

- **`get(int index)`**: Retrieves the element at the specified index, wrapped in a `Result`.

  ```pseudo
  Result get(const int index);
  ```

- **`_call(int index)`**: Alias for `get`, allowing the list to be called like a function.

  ```pseudo
  Result _call(const int index);
  ```

- **`popBack()`**: Removes and returns the last element, wrapped in an `Option`.

  ```pseudo
  Option popBack();
  ```

- **`back()`**: Returns the last element without removing it, wrapped in an `Option`.

  ```pseudo
  Option back();
  ```

- **`getCount()`**: Returns the number of elements in the list.

  ```pseudo
  int getCount();
  ```

#### Functional Methods

- **`forEach(any func, adr args)`**: Applies a function to each element.

  ```pseudo
  int forEach(const any<any, *int, *any> func, * const adr args);
  ```

- **`findFirst(bool func, adr args)`**: Finds the first element satisfying the condition.

  ```pseudo
  Option findFirst(const bool<any, *any> func, * const adr args);
  ```

- **`map(int newTypeSize, adr func, adr args)`**: Transforms the list into a new list with elements processed by `func`.

  ```pseudo
  List map(const int newTypeSize ,const adr func, * const adr args);
  ```

- **`where(adr func, adr args)`**: Filters the list based on the condition provided by `func`.

  ```pseudo
  List where(const adr func, * const adr args);
  ```

- **`reduce(int func, adr args)`**: Reduces the list to a single value using `func`.

  ```pseudo
  int reduce(const int<int, any, *int, *any> func, * const adr args);
  ```

- **`stitch(List other)`**: Appends another list to this one.

  ```pseudo
  void stitch(const List other);
  ```

#### Utility Methods

- **`toString()`**: Converts the list to a string representation.

  ```pseudo
  string toString();
  ```

- **`scroll()`**: Returns a `Scroller` for the list.

  ```pseudo
  Scroller scroll();
  ```

#### Destructor

- **`del()`**: Cleans up the list by freeing memory and deleting elements.

  ```pseudo
  int del();
  ```

---

## Helper Functions

### reduce@LinkedList

An implementation of the `reduce` function specific to `LinkedList`.

- **Signature**:

  ```pseudo
  adr reduce@LinkedList(const adr foo, * const adr args);
  ```

- **Parameters**:
  - `foo`: Function to apply for reduction.
  - `args`: Additional arguments.

- **Description**: Starts with the first element and applies the function `foo` cumulatively to reduce the list to a single value.

### forEach@LinkedList

An implementation of the `forEach` function specific to `LinkedList`.

- **Signature**:

  ```pseudo
  int forEach@LinkedList(const adr foo, * const adr arg);
  ```

- **Parameters**:
  - `foo`: Function to apply to each element.
  - `arg`: Additional arguments.

- **Description**: Iterates over each element and applies `foo`. If `foo` returns `1`, the iteration stops early.

### where@LinkedList

An implementation of the `where` function specific to `LinkedList`.

- **Signature**:

  ```pseudo
  LinkedList where@LinkedList(const adr foo, *const adr arg);
  ```

- **Parameters**:
  - `foo`: Predicate function to filter elements.
  - `arg`: Additional arguments.

- **Description**: Creates a new `LinkedList` containing elements for which `foo` returns `true`.

### Additional Helper Functions

Similar implementations exist for methods like `sort`, `map`, `find`, etc., each tailored for `LinkedList` or `Array` types.

---

## Usage Examples

### Creating and Using a LinkedList

```pseudo
// Initialize a new linked list
const LinkedList myList = new LinkedList();
myList.init(NULL, false);

// Append elements to the list
myList.append(data1);
myList.append(data2);

// Get the size of the list
int listSize = myList.size();

// Retrieve an element
generic element = myList.get(0);

// Iterate over the list
myList.forEach(printFunction, NULL);

// Filter the list
const LinkedList filteredList = myList.where(predicateFunction, NULL);
```

### Working with an Array

```pseudo
// Initialize an array with element size and capacity
const Array myArray = new Array();
myArray.init(sizeof(int), 10);

// Access elements
adr element = myArray.at(5);

// Extend the array
myArray.extend(5);

// Delete an element
myArray.del(3);
```

### Using the List Class

```pseudo
// Initialize a list
const List myDynamicList = new List();
myDynamicList.init(sizeof(int), 10, NULL, NULL);

// Add elements to the list
int value = 42;
myDynamicList.pushBack(&value);

// Retrieve an element
Result result = myDynamicList.get(0);
if result.isOk() {
    int* valPtr = result.unwrap();
}

// Iterate over the list
myDynamicList.forEach(processElementFunction, NULL);

// Map the list to a new list
List newList = myDynamicList.map(sizeof(float), convertFunction, NULL);

// Convert list to string
string listStr = myDynamicList.toString();
```

---

## Notes

- **Memory Management**: It's crucial to manage memory correctly. Always call the `del()` method to free resources when a data structure is no longer needed.

- **Function Pointers**: Many methods accept function pointers (e.g., `foo`, `compare`, `delFunc`). Ensure that these functions match the expected signatures and behavior.

- **Error Handling**: Methods often return `Result` or `Option` types to handle errors or optional values. Check these results before proceeding.

- **Contracts and Interfaces**: Classes like `ICollection` and `Keyed` define contracts that implementing classes must fulfill.

- **Type Sizes**: When working with dynamic structures like `List` and `Array`, ensure that `typeSize` matches the actual size of the data types used.

---

## Conclusion

This document provides a comprehensive overview of the data structures and collections implemented in the code. Each class and method is designed to offer flexibility and efficiency for various data manipulation tasks. Users should further explore and test these implementations according to their specific use cases and requirements.

---
