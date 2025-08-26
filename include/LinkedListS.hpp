#ifndef LINK
#define LINK

#include "LinkedList.hpp"

namespace links {
template <typename T, typename Z> class SLinkedList {
public:
  bool (*foo)(T, Z);
  int count;

  Node<T> *head;
  SLinkedList();
  ~SLinkedList();
  SLinkedList(const SLinkedList &other);
  SLinkedList &operator=(const SLinkedList &other);
  SLinkedList(SLinkedList &&other) noexcept;
  SLinkedList &operator=(SLinkedList &&other) noexcept;

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
} // namespace links

template <typename T, typename Z> T *links::SLinkedList<T, Z>::at(int i) {
  if (i < 0 || i >= this->count)
    return nullptr;
  links::Node<T> *pointer = this->head;
  for (int j = 0; j < i; j++) {
    pointer = pointer->next;
  }
  return &pointer->data;
}

template <typename T, typename Z>
T *links::SLinkedList<T, Z>::operator[](Z input) {
  links::Node<T> *pointer = this->head;
  if (this->head == nullptr)
    return nullptr;
  if ((*foo)(this->head->data, input))
    return &this->head->data;
  while (pointer->next != nullptr) {
    pointer = pointer->next;
    if (foo(pointer->data, input)) {
      return &pointer->data;
    }
  }
  return nullptr;
}

template <typename T, typename Z> links::SLinkedList<T, Z>::SLinkedList() {
  this->count = 0;
  this->foo = nullptr;
  head = nullptr;
}

template <typename T, typename Z> links::SLinkedList<T, Z>::~SLinkedList() {
  clear();
}

template <typename T, typename Z>
links::SLinkedList<T, Z>::SLinkedList(const SLinkedList &other)
    : SLinkedList() {
  foo = other.foo;
  Node<T> **tail = &head;
  for (const auto &v : other) {
    Node<T> *n = new Node<T>(v);
    *tail = n;
    tail = &n->next;
    ++count;
  }
}

template <typename T, typename Z>
links::SLinkedList<T, Z> &
links::SLinkedList<T, Z>::operator=(const SLinkedList &other) {
  if (this != &other) {
    clear();
    foo = other.foo;
    Node<T> **tail = &head;
    for (const auto &v : other) {
      Node<T> *n = new Node<T>(v);
      *tail = n;
      tail = &n->next;
      ++count;
    }
  }
  return *this;
}

template <typename T, typename Z>
links::SLinkedList<T, Z>::SLinkedList(SLinkedList &&other) noexcept {
  foo = other.foo;
  count = other.count;
  head = other.head;
  other.head = nullptr;
  other.count = 0;
  other.foo = nullptr;
}

template <typename T, typename Z>
links::SLinkedList<T, Z> &
links::SLinkedList<T, Z>::operator=(SLinkedList &&other) noexcept {
  if (this != &other) {
    clear();
    foo = other.foo;
    count = other.count;
    head = other.head;
    other.head = nullptr;
    other.count = 0;
    other.foo = nullptr;
  }
  return *this;
}

template <typename T, typename Z> void links::SLinkedList<T, Z>::clear() {
  if (this->count > 0) {
    // this->head = nullptr;
    while (this->count > 0) {
      this->pop();
    }
  }
  this->head = nullptr;
  this->count = 0;
}

template <typename T, typename Z> void links::SLinkedList<T, Z>::push(T value) {
  this->count += 1;
  Node<T> *push = new Node<T>(std::move(value));
  push->next = this->head;
  this->head = push;
}

template <typename T, typename Z>
void links::SLinkedList<T, Z>::operator<<(T value) {
  this->count += 1;
  Node<T> *push = new Node<T>(std::move(value));
  push->next = this->head;
  this->head = push;
}

template <typename T, typename Z> void links::SLinkedList<T, Z>::invert() {
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

template <typename T, typename Z> T links::SLinkedList<T, Z>::pop() {
  this->count -= 1;
  T data = this->head->data;
  Node<T> *popper = this->head;
  this->head = this->head->next;
  delete popper;
  if (this->count == 0)
    head = nullptr;
  return data;
}

template <typename T, typename Z> T links::SLinkedList<T, Z>::peek() {
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
  l.head = nullptr;
  l.count = 0;
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
  l.head = nullptr;
  l.count = 0;
}

#endif
