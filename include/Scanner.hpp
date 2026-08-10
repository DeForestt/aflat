#ifndef LEX
#define LEX

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

#include "LinkedList.hpp"

using links::LinkedList;
using std::string;

namespace lex {

class TokenAllocationScope {
public:
  TokenAllocationScope();
  ~TokenAllocationScope();
  TokenAllocationScope(const TokenAllocationScope &) = delete;
  TokenAllocationScope &operator=(const TokenAllocationScope &) = delete;

private:
  std::size_t checkpoint = 0;
};

class Token {
public:
  static constexpr std::uint32_t UntrackedAllocation =
      static_cast<std::uint32_t>(-1);
  Token();
  Token(const Token &other);
  Token &operator=(const Token &other);
  int lineCount;
  bool generated = false;
  int column = 1;
  int length = 1;
  virtual string toString() const {
    return "Token at line " + std::to_string(lineCount);
  }
  virtual ~Token();
  static void *operator new(std::size_t size);
  static void operator delete(void *ptr) noexcept;
  static void operator delete(void *ptr, std::size_t) noexcept;
  std::uint32_t allocationIndex = UntrackedAllocation;
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
  bool uniqueLiteral = false;
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
  Lexer();
  ~Lexer();
  Lexer(Lexer &&) noexcept;
  Lexer &operator=(Lexer &&) noexcept;
  Lexer(const Lexer &) = delete;
  Lexer &operator=(const Lexer &) = delete;

  LinkedList<Token *> Scan(string input, int startLine = 1);

private:
  struct Impl;
  std::unique_ptr<Impl> impl;
};

}; // namespace lex
#endif
