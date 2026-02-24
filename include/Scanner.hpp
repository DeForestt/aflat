#ifndef LEX
#define LEX

#include <string>

#include "LinkedList.hpp"
#include <memory>

using links::LinkedList;
using std::string;

namespace lex {

class Token {
public:
  int lineCount;
  virtual string toString() const {
    return "Token at line " + std::to_string(lineCount);
  }
  virtual ~Token() = default;
};

class Symbol : public Token {
public:
  string meta;
  string toString() const override {
    return "Symbol: " + meta + " at line " + std::to_string(lineCount);
  }
};

class LObj : public Token {
public:
  string meta;
  string toString() const override {
    return "LObj: " + meta + " at line " + std::to_string(lineCount);
  }
};

class Ref : public Token {
public:
  string toString() const override {
    return "Ref at line " + std::to_string(lineCount);
  }
};

class StringObj : public Token {
public:
  string value;
  string toString() const override {
    return "StringObj: " + value + " at line " + std::to_string(lineCount);
  }
};

class FStringObj : public Token {
public:
  string value;
  string toString() const override {
    return "FStringObj: " + value + " at line " + std::to_string(lineCount);
  }
};

class CharObj : public Token {
public:
  char value;
  string toString() const override {
    return "CharObj: " + std::string(1, value) + " at line " +
           std::to_string(lineCount);
  }
};

class OpSym : public Token {
public:
  char Sym;
  string toString() const override {
    return "OpSym: " + std::string(1, Sym) + " at line " +
           std::to_string(lineCount);
  }
};

class INT : public Token {
public:
  string value;
  string toString() const override {
    return "INT: " + value + " at line " + std::to_string(lineCount);
  }
};

class Long : public Token {
public:
  string value;
  string toString() const override {
    return "Long: " + value + " at line " + std::to_string(lineCount);
  }
};

class FloatLit : public Token {
public:
  string value;
  string toString() const override {
    return "FloatLit: " + value + " at line " + std::to_string(lineCount);
  }
};

/*This will take apart the string and breake it down into "Words"
that the parser will be able to understand*/
class Lexer {
public:
  LinkedList<std::shared_ptr<Token>> Scan(string input, int startLine = 1);
};

template <typename T>
links::LinkedList<T *>
toRawList(const links::LinkedList<std::shared_ptr<T>> &list) {
  links::LinkedList<T *> out;
  for (const auto &ptr : list) {
    out << ptr.get();
  }
  return out;
}

}; // namespace lex
#endif