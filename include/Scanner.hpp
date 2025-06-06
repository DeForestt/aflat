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
  int columnStart;
  int columnEnd;
  virtual ~Token() = default;
};

class Symbol : public Token {
 public:
  string meta;
};

class LObj : public Token {
 public:
  string meta;
};

class Ref : public Token {};

class StringObj : public Token {
 public:
  string value;
};

class FStringObj : public Token {
 public:
  string value;
};

class CharObj : public Token {
 public:
  char value;
};

class OpSym : public Token {
 public:
  char Sym;
};

class INT : public Token {
 public:
  string value;
};

class Long : public Token {
 public:
  string value;
};

class FloatLit : public Token {
 public:
  string value;
};

/*This will take apart the string and breake it down into "Words"
that the parser will be able to understand*/
class Lexer {
 public:
  LinkedList<Token *> Scan(string input, int startLine = 1);
};

};  // namespace lex
#endif