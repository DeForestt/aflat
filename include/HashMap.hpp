#pragma once
#include <iostream>
#include <string>

template <typename Z> class HashMap {
public:
  HashMap();
  ~HashMap();
  HashMap(const HashMap &other);
  HashMap &operator=(const HashMap &other);
  HashMap(HashMap &&other) noexcept;
  HashMap &operator=(HashMap &&other) noexcept;
  void insert(std::string key, Z value);
  Z get(std::string key);
  void remove(std::string key);
  void clear();
  int size();
  bool contains(std::string key);

private:
  struct Node {
    std::string key;
    Z value;
    Node *next;
  };
  Node **table;
  int tableSize;
  int count;
  int hash(std::string key);
};

template <typename Z> HashMap<Z>::HashMap() {
  this->tableSize = 100;
  this->count = 0;
  this->table = new Node *[this->tableSize];
  for (int i = 0; i < this->tableSize; i++) {
    this->table[i] = nullptr;
  }
}

template <typename Z> HashMap<Z>::~HashMap() {
  clear();
  delete[] this->table;
  this->table = nullptr;
  this->tableSize = 0;
}

template <typename Z> HashMap<Z>::HashMap(const HashMap &other) : HashMap() {
  delete[] this->table;
  this->tableSize = other.tableSize;
  this->table = new Node *[this->tableSize];
  for (int i = 0; i < this->tableSize; ++i)
    this->table[i] = nullptr;
  for (int i = 0; i < other.tableSize; ++i) {
    Node *curr = other.table[i];
    Node **tail = &this->table[i];
    while (curr != nullptr) {
      Node *newNode = new Node();
      newNode->key = curr->key;
      newNode->value = curr->value;
      newNode->next = nullptr;
      *tail = newNode;
      tail = &newNode->next;
      curr = curr->next;
    }
  }
  this->count = other.count;
}

template <typename Z> HashMap<Z> &HashMap<Z>::operator=(const HashMap &other) {
  if (this != &other) {
    clear();
    delete[] this->table;
    this->tableSize = other.tableSize;
    this->table = new Node *[this->tableSize];
    for (int i = 0; i < this->tableSize; ++i)
      this->table[i] = nullptr;
    for (int i = 0; i < other.tableSize; ++i) {
      Node *curr = other.table[i];
      Node **tail = &this->table[i];
      while (curr != nullptr) {
        Node *newNode = new Node();
        newNode->key = curr->key;
        newNode->value = curr->value;
        newNode->next = nullptr;
        *tail = newNode;
        tail = &newNode->next;
        curr = curr->next;
      }
    }
    this->count = other.count;
  }
  return *this;
}

template <typename Z> HashMap<Z>::HashMap(HashMap &&other) noexcept {
  this->table = other.table;
  this->tableSize = other.tableSize;
  this->count = other.count;
  other.table = nullptr;
  other.tableSize = 0;
  other.count = 0;
}

template <typename Z>
HashMap<Z> &HashMap<Z>::operator=(HashMap &&other) noexcept {
  if (this != &other) {
    clear();
    delete[] this->table;
    this->table = other.table;
    this->tableSize = other.tableSize;
    this->count = other.count;
    other.table = nullptr;
    other.tableSize = 0;
    other.count = 0;
  }
  return *this;
}

template <typename Z> void HashMap<Z>::insert(std::string key, Z value) {
  int index = this->hash(key);
  Node *curr = this->table[index];
  while (curr != nullptr) {
    if (curr->key == key) {
      curr->value = value;
      return;
    }
    curr = curr->next;
  }
  Node *newNode = new Node();
  newNode->key = key;
  newNode->value = value;
  newNode->next = this->table[index];
  this->table[index] = newNode;
  this->count++;
}

template <typename Z> Z HashMap<Z>::get(std::string key) {
  int index = this->hash(key);
  Node *curr = this->table[index];
  while (curr != nullptr) {
    if (curr->key == key) {
      return curr->value;
    }
    curr = curr->next;
  }
  return nullptr;
}

template <typename Z> void HashMap<Z>::remove(std::string key) {
  int index = this->hash(key);
  Node *curr = this->table[index];
  Node *prev = nullptr;
  while (curr != nullptr) {
    if (curr->key == key) {
      if (prev == nullptr) {
        this->table[index] = curr->next;
      } else {
        prev->next = curr->next;
      }
      delete curr;
      this->count--;
      return;
    }
    prev = curr;
    curr = curr->next;
  }
}

template <typename Z> void HashMap<Z>::clear() {
  if (!this->table)
    return;
  for (int i = 0; i < this->tableSize; i++) {
    Node *curr = this->table[i];
    while (curr != nullptr) {
      Node *temp = curr;
      curr = curr->next;
      delete temp;
    }
    this->table[i] = nullptr;
  }
  this->count = 0;
}

template <typename Z> int HashMap<Z>::size() { return this->count; }

template <typename Z> bool HashMap<Z>::contains(std::string key) {
  int index = this->hash(key);
  if (index < 0 || index >= this->tableSize) {
    std::cout << "Warning: " << key << " returns " << index
              << " which is out of bounds." << std::endl;
    return false;
  };

  Node *curr = this->table[index];
  while (curr != nullptr) {
    if (curr->key == key) {
      return true;
    }
    curr = curr->next;
  }
  return false;
}

template <typename Z> int HashMap<Z>::hash(std::string key) {
  int hash = 0;
  for (int i = 0; i < key.length(); i++) {
    hash += key[i];
  }
  return hash % (this->tableSize - 1);
}