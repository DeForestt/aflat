#ifndef LINKS
#define LINKS

#include <algorithm>
#include <vector>

#include "Exceptions.hpp"

namespace links {

template <typename T>
class Node {
 public:
  T data;
  Node *next = nullptr;
};

template <typename T>
class LinkedList {
 public:
  bool (*foo)(T, T);
  int count;
  Node<T> *head;
  Node<T> *pos;

  LinkedList();

  void push(T value);
  void operator<<(T value) { push(value); }
  void insert(T value, int index);
  void insert_top(T value, int index);
  void invert();
  void stitch(LinkedList<T> l);
  void istitch(LinkedList<T> l);
  void clear();
  T get(int index);
  int size();
  int trail();
  void terminate(int n);
  T pop();
  T peek();
  template <typename Z>
  T *search(bool (*f)(T, Z), Z input);
  T *operator[](T input);
  T *append(T input);
  T shift();
  T touch();
  void place(T value);
  void reset();

 private:
  std::vector<Node<T>> nodes;
  void rebuild();
};

}  // namespace links

// Implementation

namespace links {

template <typename T>
LinkedList<T>::LinkedList()
    : foo(nullptr), count(0), head(nullptr), pos(nullptr) {}

template <typename T>
void LinkedList<T>::rebuild() {
  head = nodes.empty() ? nullptr : &nodes.front();
  for (size_t i = 0; i < nodes.size(); ++i) {
    nodes[i].next = (i + 1 < nodes.size()) ? &nodes[i + 1] : nullptr;
  }
  pos = head;
  count = static_cast<int>(nodes.size());
}

template <typename T>
void LinkedList<T>::push(T value) {
  nodes.insert(nodes.begin(), Node<T>{value, nullptr});
  rebuild();
}

template <typename T>
void LinkedList<T>::insert(T value, int index) {
  if (index < 1 || index > static_cast<int>(nodes.size()))
    throw err::Exception("Index Out of Range");
  nodes.insert(nodes.end() - index + 1, Node<T>{value, nullptr});
  rebuild();
}

template <typename T>
void LinkedList<T>::insert_top(T value, int index) {
  if (index < 0 || index > static_cast<int>(nodes.size()))
    throw err::Exception("Index Out of Range");
  nodes.insert(nodes.begin() + index, Node<T>{value, nullptr});
  rebuild();
}

template <typename T>
void LinkedList<T>::invert() {
  std::reverse(nodes.begin(), nodes.end());
  rebuild();
}

template <typename T>
void LinkedList<T>::stitch(LinkedList<T> l) {
  nodes.insert(nodes.end(), l.nodes.begin(), l.nodes.end());
  rebuild();
}

template <typename T>
void LinkedList<T>::istitch(LinkedList<T> l) {
  nodes.insert(nodes.begin(), l.nodes.begin(), l.nodes.end());
  rebuild();
}

template <typename T>
void LinkedList<T>::clear() {
  nodes.clear();
  rebuild();
}

template <typename T>
T LinkedList<T>::get(int index) {
  if (index < 0 || index >= static_cast<int>(nodes.size()))
    throw err::Exception("Index Out of Bounds");
  return nodes[index].data;
}

template <typename T>
int LinkedList<T>::size() {
  return static_cast<int>(nodes.size());
}

template <typename T>
int LinkedList<T>::trail() {
  int c = 0;
  Node<T> *tmp = pos;
  while (tmp) {
    ++c;
    tmp = tmp->next;
  }
  return c;
}

template <typename T>
void LinkedList<T>::terminate(int n) {
  if (n < 0 || n >= static_cast<int>(nodes.size())) return;
  if (n + 1 < static_cast<int>(nodes.size())) nodes[n].next = nullptr;
}

template <typename T>
T LinkedList<T>::pop() {
  if (nodes.empty()) throw err::Exception("Pop from empty list");
  T val = nodes.front().data;
  nodes.erase(nodes.begin());
  rebuild();
  return val;
}

template <typename T>
T LinkedList<T>::peek() {
  if (nodes.empty()) throw err::Exception("Peek from empty list");
  return nodes.front().data;
}

template <typename T>
T *LinkedList<T>::append(T input) {
  nodes.push_back(Node<T>{input, nullptr});
  rebuild();
  return &nodes.back().data;
}

template <typename T>
T *LinkedList<T>::operator[](T input) {
  if (!foo) return nullptr;
  for (auto &n : nodes) {
    if (foo(n.data, input)) return &n.data;
  }
  return nullptr;
}

template <typename T>
template <typename Z>
T *LinkedList<T>::search(bool (*f)(T, Z), Z input) {
  for (auto &n : nodes) {
    if (f(n.data, input)) return &n.data;
  }
  return nullptr;
}

template <typename T>
T LinkedList<T>::shift() {
  if (!pos) throw err::Exception("Position Pointer is null");
  T data = pos->data;
  pos = pos->next;
  return data;
}

template <typename T>
T LinkedList<T>::touch() {
  if (!pos) throw err::Exception("Position Pointer is null cannot touch");
  return pos->data;
}

template <typename T>
void LinkedList<T>::place(T value) {
  int index = 0;
  Node<T> *curr = head;
  while (curr && curr != pos) {
    curr = curr->next;
    ++index;
  }
  insert_top(value, index);
}

template <typename T>
void LinkedList<T>::reset() {
  rebuild();
}

}  // namespace links

#endif
