#ifndef LINKS
#define LINKS

#include <iterator>
#include <utility>

#include "Exceptions.hpp"

namespace links {

template <typename T>
class Node {
 public:
  T data;
  Node *next = nullptr;

  Node() = default;
  explicit Node(const T &val) : data(val) {}
  explicit Node(T &&val) : data(std::move(val)) {}
};

template <typename T>
class LinkedList {
 public:
  bool (*foo)(T, T);
  int count;

  Node<T> *head;
  Node<T> *pos;
  LinkedList();
  ~LinkedList();
  LinkedList(const LinkedList &other);
  LinkedList &operator=(const LinkedList &other);
  LinkedList(LinkedList &&other) noexcept;
  LinkedList &operator=(LinkedList &&other) noexcept;

  class iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;

    iterator(Node<T> *n) : node(n) {}
    reference operator*() const { return node->data; }
    pointer operator->() const { return &node->data; }
    iterator &operator++() {
      node = node ? node->next : nullptr;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp(*this);
      ++(*this);
      return tmp;
    }
    friend bool operator==(const iterator &a, const iterator &b) {
      return a.node == b.node;
    }
    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.node != b.node;
    }

   private:
    Node<T> *node;
  };

  class const_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = const T *;
    using reference = const T &;

    const_iterator(const Node<T> *n) : node(n) {}
    reference operator*() const { return node->data; }
    pointer operator->() const { return &node->data; }
    const_iterator &operator++() {
      node = node ? node->next : nullptr;
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator tmp(*this);
      ++(*this);
      return tmp;
    }
    friend bool operator==(const const_iterator &a, const const_iterator &b) {
      return a.node == b.node;
    }
    friend bool operator!=(const const_iterator &a, const const_iterator &b) {
      return a.node != b.node;
    }

   private:
    const Node<T> *node;
  };

  iterator begin() { return iterator(head); }
  iterator end() { return iterator(nullptr); }
  const_iterator begin() const { return const_iterator(head); }
  const_iterator end() const { return const_iterator(nullptr); }

  /*Push a new value to the top of the list*/
  void push(T value);

  /*push with operator*/
  void operator<<(T value);

  /*Insert at an index*/
  void insert(T value, int index);

  /*void insert from top*/
  void insert_top(T value, int index);
  /*Reverses the direction of the list*/
  void invert();

  /*attaches the head of another linked list to the tail of this one*/
  void stitch(links::LinkedList<T> l);

  /*stitches the head of this linked list to the tail of another one and sets
   * the head of the other as the new head*/
  void istitch(links::LinkedList<T> l);

  /*clears the list and sets the head to nullptr*/
  void clear();

  /*returns the value at the index*/
  T get(int index);

  /*returns the number of nodes in the list*/
  int size();

  /*like size but from the pos*/
  int trail();

  /*set the nth items next to nullptr*/
  void terminate(int n);

  /*Pop removes the top element for the list
  Behaves like a stack*/
  T pop();

  /*Observes the first node on the list and returns its value without
  popping it.*/
  T peek();

  /*search with passed in logic*/
  template <typename Z>
  T *search(bool (*foo)(T, Z), Z input);

  /*search for a value*/
  T *operator[](T input);

  /*appends a node to the end of the list*/
  T *append(T input);

  /*A list of functions to use without mutating the List */

  /*Return the element pointed to by the pos Pointer and shifts the pos pointer
   * by one*/
  T shift();

  /*Return the element pointed to by the pos pointer without shifting*/
  T touch();

  /*insert at the position of the pos pointer*/
  void place(T);

  void reset();
};
}  // namespace links

template <typename T>
int links::LinkedList<T>::size() {
  int count = 0;
  Node<T> *temp = head;
  while (temp != nullptr) {
    count++;
    temp = temp->next;
  }
  return count;
}

template <typename T>
int links::LinkedList<T>::trail() {
  int count = 0;
  Node<T> *temp = pos;
  while (temp != nullptr) {
    count++;
    temp = temp->next;
  }
  return count;
}

template <typename T>
void links::LinkedList<T>::terminate(int n) {
  Node<T> *temp = head;
  for (int i = 0; i < n; i++) {
    temp = temp->next;
  }
  temp->next = nullptr;
}

template <typename T>
T links::LinkedList<T>::get(int index) {
  if (index < 0 || index > this->size()) {
    throw err::Exception("Index Out of Bounds");
    ;
  }
  Node<T> *current = head;
  for (int i = 0; i < index; i++) {
    current = current->next;
  }
  return current->data;
}

template <typename T>
T *links::LinkedList<T>::append(T input) {
  Node<T> *temp = new Node<T>(std::move(input));
  temp->next = nullptr;
  if (head == nullptr) {
    head = temp;
  } else {
    Node<T> *current = head;
    while (current->next != nullptr) {
      current = current->next;
    }
    current->next = temp;
  }
  count++;
  return &temp->data;
}

template <typename T>
void links::LinkedList<T>::reset() {
  this->pos = this->head;
  if (this->trail() > this->count) {
    this->terminate(this->count - 1);
  }
}

template <typename T>
template <typename Z>
T *links::LinkedList<T>::search(bool (*foo)(T, Z), Z input) {
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

template <typename T>
T *links::LinkedList<T>::operator[](T input) {
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

template <typename T>
links::LinkedList<T>::LinkedList() {
  this->count = 0;
  this->foo = nullptr;
  head = nullptr;
  pos = nullptr;
}

template <typename T>
links::LinkedList<T>::~LinkedList() {
  clear();
}

template <typename T>
links::LinkedList<T>::LinkedList(const LinkedList &other) : LinkedList() {
  foo = other.foo;
  for (const auto &v : other) append(v);
  pos = head;
}

template <typename T>
links::LinkedList<T> &links::LinkedList<T>::operator=(const LinkedList &other) {
  if (this != &other) {
    clear();
    foo = other.foo;
    for (const auto &v : other) append(v);
    pos = head;
  }
  return *this;
}

template <typename T>
links::LinkedList<T>::LinkedList(LinkedList &&other) noexcept {
  foo = other.foo;
  count = other.count;
  head = other.head;
  pos = head;
  other.head = nullptr;
  other.pos = nullptr;
  other.count = 0;
  other.foo = nullptr;
}

template <typename T>
links::LinkedList<T> &links::LinkedList<T>::operator=(
    LinkedList &&other) noexcept {
  if (this != &other) {
    clear();
    foo = other.foo;
    count = other.count;
    head = other.head;
    pos = head;
    other.head = nullptr;
    other.pos = nullptr;
    other.count = 0;
    other.foo = nullptr;
  }
  return *this;
}

template <typename T>
void links::LinkedList<T>::clear() {
  if (this->count > 0) {
    while (this->count > 0) {
      this->pop();
    }
  }
  this->count = 0;
  this->head = nullptr;
  this->pos = nullptr;
}

template <typename T>
void links::LinkedList<T>::push(T value) {
  this->count += 1;
  Node<T> *push = new Node<T>(std::move(value));
  push->next = this->head;
  this->head = push;
  this->pos = this->head;
}

template <typename T>
void links::LinkedList<T>::insert(T value, int index) {
  Node<T> *curr = this->head;
  int i = 0;
  int count = this->size();
  while (curr != nullptr) {
    if (i == count - index) {
      Node<T> *New = new Node<T>(std::move(value));
      New->next = curr->next;
      curr->next = New;
      this->count++;
      return;
    }
    curr = curr->next;
    i++;
  }
  throw err::Exception("Index Out of Range");
};

template <typename T>
void links::LinkedList<T>::insert_top(T value, int index) {
  Node<T> *curr = this->head;
  int i = 0;
  while (curr != nullptr) {
    if (i == index) {
      Node<T> *New = new Node<T>(std::move(value));
      New->next = curr;
      this->head = New;
      this->count++;
      this->pos = New;
      return;
    }
    curr = curr->next;
    i++;
  }
  throw err::Exception("Index Out of Range");
}

template <typename T>
void links::LinkedList<T>::operator<<(T value) {
  this->count += 1;
  Node<T> *push = new Node<T>(std::move(value));
  push->next = this->head;
  this->head = push;
}

template <typename T>
void links::LinkedList<T>::invert() {
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
  this->pos = this->head;
}

template <typename T>
T links::LinkedList<T>::pop() {
  this->count -= 1;
  T data = this->head->data;
  Node<T> *popper = this->head;
  this->head = this->head->next;
  this->pos = this->head;
  delete popper;
  if (this->count == 0) head = nullptr;
  return data;
}

template <typename T>
T links::LinkedList<T>::shift() {
  if (this->pos == nullptr) throw err::Exception("Position Pointer is null");

  T data = this->pos->data;
  this->pos = this->pos->next;
  return data;
}

template <typename T>
T links::LinkedList<T>::peek() {
  return this->head->data;
}

template <typename T>
T links::LinkedList<T>::touch() {
  if (this->pos == nullptr)
    throw err::Exception("Position Pointer is null cannot touch");
  return this->pos->data;
};

template <typename T>
void links::LinkedList<T>::place(T value) {
  // get the index of pos
  int index = 0;
  Node<T> *curr = this->head;
  while (curr != this->pos) {
    curr = curr->next;
    index++;
  }
  // insert at the index
  this->insert_top(value, index);
};

template <typename T>
void links::LinkedList<T>::stitch(LinkedList<T> l) {
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
  l.head = nullptr;
  l.count = 0;
}

template <typename T>
void links::LinkedList<T>::istitch(LinkedList<T> l) {
  links::Node<T> *pointer = l.head;
  this->count += l.count;
  if (pointer == nullptr) {
    return;
  } else {
    while ((pointer->next != nullptr)) {
      pointer = pointer->next;
    }
    pointer->next = head;
    this->head = l.head;
  }
  l.head = nullptr;
  l.count = 0;
}

#endif
