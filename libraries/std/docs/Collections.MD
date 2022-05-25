# ICollection -- Depricated
The ICollection interface that was meant to provide a common interface for all collections.  It is now only used in the case of the Array class.  It is not recommended to use this interface.

## Fields
The ICollection interface mandates no fields.

## Interface Methods
### ForEach
The forEach method should be used to iterate over the collection.
signature:
```js
adr forEach = [] => panic("Function (forEach) not implemented on this instance");
```
### Where
The where method should be used to filter the collection.
signature:
```js
adr where = [] => panic("Function (where) not implemented on this instance");
```

# ListNode
The ListNode class is used to create a node in a linked list.

## Fields
### next
The next field is a pointer to the next node in the list.

### Data
The data field is a reference to the data contained in the node.

# LinkedList
The LinkedList class is a dynamic linked list of refrences to any type. Please not that if a primitive type is used, the data will need to be boxed or a reference to the data will need to be passed.

## Fields
### head
This is a ListNode which is the first node in the list.

### pos
This is the current position of the linked list.

## Constructor
The constructor takes no arguments. and retruns a new LinkedList.

## Methods

### Append
The append method adds a new node to the end of the list with the given data.
signature:
```js
int append(adr data);
```
example:
```js
import LinkedList from "Collections";
//...

LinkedList list = new LinkedList();
int i = 0;
linkedList.append(?i); // append a reference to the integer i to the end of the linked list.
```

### insert
The insert method inserts a new node at the given position.
signature:
```js
int insert(adr data, int index);
```
example:
```js
import LinkedList from "Collections";
//...

LinkedList list = new LinkedList();
int i = 0;
linkedList.append(?i); // append a reference to the integer i to the end of the linked list.
int j = 1;
linkedList.insert(?j, 0); // insert a reference to the integer j at the beginning of the linked list.

// the linked list now contains: [?j, ?i]
```

### del
The del method deletes the node at the given position.
signature:
```js
int del(int index);
```
example:
```js
import LinkedList from "Collections";
//...

LinkedList list = new LinkedList();
int i = 0;
linkedList.append(?i); // append a reference to the integer i to the end of the linked list.
int j = 1;
linkedList.insert(?j, 0); // insert a reference to the integer j at the beginning of the linked list.
linkedList.del(0); // delete the node at index 0.

// the linked list now contains: [?i]
```