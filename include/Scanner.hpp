#ifndef LEX
#define LEX

#include <string>

#include "LinkedList.hpp"

using links::LinkedList;
using std::string;

namespace lex {

class Token {
 public:
  int lineCount;
  int column;
  Token(int line = 0, int col = 0) : lineCount(line), column(col) {}
  virtual ~Token() = default;
};

class Symbol : public Token {
 public:
  string meta;
  Symbol(string m = "", int line = 0, int col = 0)
      : Token(line, col), meta(std::move(m)) {}
};

class LObj : public Token {
 public:
  string meta;
  LObj(string m = "", int line = 0, int col = 0)
      : Token(line, col), meta(std::move(m)) {}
};

class Ref : public Token {
 public:
  explicit Ref(int line = 0, int col = 0) : Token(line, col) {}
};

class StringObj : public Token {
 public:
  string value;
  StringObj(string v = "", int line = 0, int col = 0)
      : Token(line, col), value(std::move(v)) {}
};

class FStringObj : public Token {
 public:
  string value;
  FStringObj(string v = "", int line = 0, int col = 0)
      : Token(line, col), value(std::move(v)) {}
};

class CharObj : public Token {
 public:
  char value;
  CharObj(char v = 0, int line = 0, int col = 0) : Token(line, col), value(v) {}
};

class OpSym : public Token {
 public:
  char Sym;
  OpSym(char s = 0, int line = 0, int col = 0) : Token(line, col), Sym(s) {}
};

class INT : public Token {
 public:
  string value;
  INT(string v = "", int line = 0, int col = 0)
      : Token(line, col), value(std::move(v)) {}
};

class Long : public Token {
 public:
  string value;
  Long(string v = "", int line = 0, int col = 0)
      : Token(line, col), value(std::move(v)) {}
};

class FloatLit : public Token {
 public:
  string value;
  FloatLit(string v = "", int line = 0, int col = 0)
      : Token(line, col), value(std::move(v)) {}
};

/*This will take apart the string and breake it down into "Words"
that the parser will be able to understand*/
class Lexer {
 public:
  LinkedList<Token *> Scan(string input, int startLine = 1);
};

};  // namespace lex
#endif