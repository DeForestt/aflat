#ifndef LINK
#define LINK

#include "LinkedList.hpp"

namespace links {
template <typename T, typename Z>
class SLinkedList {
 public:
  bool (*foo)(T, Z);
  int count;

  Node<T> *head;
  SLinkedList();

  /*Push a new value to the top of the list*/
  void push(T value);

  void operator<<(T value);

  /*Reverses the direction of the list*/
  void invert();

  /*attaches the head of another linked list to the tail of this one*/
  void stitch(links::SLinkedList<T, Z> l);

  /*stitches the head of this linked list to the tail of another one and sets
   * the head of the other as the new head*/
  void istitch(links::SLinkedList<T, Z> l);

  /*clears the list and sets the head to nullptr*/
  void clear();

  /*Pop removes the top element for the list
  Behaves like a stack*/
  T pop();

  /*Observes the first node on the list and returns its value without
  popping it.*/
  T peek();

  T *operator[](Z input);

  T *at(int i);
};
}  // namespace links

template <typename T, typename Z>
T *links::SLinkedList<T, Z>::at(int i) {
  if (i < 0 || i >= this->count) return nullptr;
  links::Node<T> *pointer = this->head;
  for (int j = 0; j < i; j++) {
    pointer = pointer->next;
  }
  return &pointer->data;
}

template <typename T, typename Z>
T *links::SLinkedList<T, Z>::operator[](Z input) {
  links::Node<T> *pointer = this->head;
  if (this->head == nullptr) return nullptr;
  if ((*foo)(this->head->data, input)) return &this->head->data;
  while (pointer->next != nullptr) {
    pointer = pointer->next;
    if (foo(pointer->data, input)) {
      return &pointer->data;
    }
  }
  return nullptr;
}

template <typename T, typename Z>
links::SLinkedList<T, Z>::SLinkedList() {
  this->count = 0;
  this->foo = nullptr;
  head = nullptr;
}

template <typename T, typename Z>
void links::SLinkedList<T, Z>::clear() {
  if (this->count > 0) {
    // this->head = nullptr;
    while (this->count > 0) {
      this->pop();
    }
  }
}

template <typename T, typename Z>
void links::SLinkedList<T, Z>::push(T value) {
  this->count += 1;
  Node<T> *push = new Node<T>();
  push->next = this->head;
  push->data = value;
  this->head = push;
}

template <typename T, typename Z>
void links::SLinkedList<T, Z>::operator<<(T value) {
  this->count += 1;
  Node<T> *push = new Node<T>();
  push->next = this->head;
  push->data = value;
  this->head = push;
}

template <typename T, typename Z>
void links::SLinkedList<T, Z>::invert() {
  Node<T> *prev = nullptr;
  Node<T> *curr = this->head;
  Node<T> *next = nullptr;

  while (curr != nullptr) {
    next = curr->next;
    curr->next = prev;
    prev = curr;
    curr = next;
  }
  this->head = prev;
}

template <typename T, typename Z>
T links::SLinkedList<T, Z>::pop() {
  this->count -= 1;
  T data = this->head->data;
  Node<T> *popper = this->head;
  this->head = this->head->next;
  delete popper;
  return data;
}

template <typename T, typename Z>
T links::SLinkedList<T, Z>::peek() {
  return this->head->data;
}

template <typename T, typename Z>
void links::SLinkedList<T, Z>::stitch(SLinkedList<T, Z> l) {
  links::Node<T> *pointer = head;
  this->count += l.count;
  if (pointer == nullptr) {
    this->head = l.head;
  } else {
    while (pointer->next != nullptr) {
      pointer = pointer->next;
    }
    pointer->next = l.head;
  }
}

template <typename T, typename Z>
void links::SLinkedList<T, Z>::istitch(SLinkedList<T, Z> l) {
  links::Node<T> *pointer = l.head;
  this->count += l.count;
  if (pointer == nullptr) {
    this->head = l.head;
  } else {
    while (pointer->next != nullptr) {
      pointer = pointer->next;
    }
    pointer->next = head;
    this->head = l.head;
  }
}

#endif