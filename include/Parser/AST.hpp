#ifndef STRUCT
#define STRUCT

#include <string>
#include <vector>

#include "ASM.hpp"
#include "CodeGenerator/GenerationResult.hpp"
#include "LinkedList.hpp"
#include "Scanner.hpp"

namespace parse {
class Parser;
};

namespace gen {
class CodeGenerator;
};  // namespace gen

namespace ast {

class Call;
class Declare;
class Function;
class DecArr;

class ProgramMember {
 public:
  virtual std::string toString() { return ""; };
};

class Ident {
 public:
  std::string ident;
};

class Statement {
 public:
  bool locked = false;
  int logicalLine = 0;
  virtual std::string toString() { return ""; };
  virtual gen::GenerationResult const generate(gen::CodeGenerator &generator) {
    asmc::File file;
    file.text << new asmc::nop();
    return {file, std::nullopt};
  };
};

class Member {
 public:
  virtual std::string toString() { return ""; };
};

enum Op {
  Plus,
  Minus,
  Carrot,
  Mod,
  Mul,
  Div,
  Equ,
  NotEqu,
  Less,
  Great,
  AndBit,
  AndBool,
  OrBit,
  LessCmp,
  GreatCmp,
  CompEqu,
  Leq,
  Geq,
  None,
};

// Enum to represent the scope of a variable
enum ScopeMod { Public, Private, Static, Export };

class Expr : public Statement {
 public:
  virtual std::string toString() override { return ""; };
  Expr *extention = nullptr;
  std::string typeCast = "";
};

class ConditionalExpr {
 public:
  Expr *expr1;
  Op op;
  Expr *expr2;
};

class Type {
 public:
  std::string typeName;
  asmc::Size size;
  asmc::OpType opType = asmc::Hard;
  int arraySize = 1;
  static bool compare(Type t, std::string name);
  links::LinkedList<int> indices;
  bool isGeneric = false;
  bool safeType = false;
  bool pedantic = false;
  Type *typeHint;
  bool isReference = false;
  bool isRvalue = false;
  asmc::Size refSize;

  struct FPointerArgs {
    Type *returnType = nullptr;
    int requiredArgs = 0;
    std::vector<int> optConvertionIndices;
    std::vector<Type> argTypes;
    std::string id;
    asmc::Size size;
    bool isFPointer = false;
  };

  FPointerArgs fPointerArgs;

  Type() = default;
  Type(const std::string &typeName, const asmc::Size &size)
      : typeName(typeName), size(size){};
};

class Arg {
 public:
  virtual std::string toString() { return ""; };
};

class Program {
 public:
  ProgramMember members;
};

class Argument : public Arg, public Statement {
 public:
  std::string Ident;
  Type type;
};

class ParenExpr : public Expr {
 public:
  Expr *expr;
};

class CharLiteral : public Expr {
 public:
  char value;
};

class Iflush : public Statement {};

class CWrite : public Statement {
 public:
  Expr *expr;
};

class Push : public Statement {
 public:
  Expr *expr;
};

class Pull : public Statement {
 public:
  Expr *expr;
};

class Var : public Expr {
 public:
  std::string Ident;
  links::LinkedList<Expr *> indices;
  links::LinkedList<std::string> modList;
  bool clean = false;
  bool internal = false;
};

class StringLiteral : public Expr {
 public:
  std::string val;
};

class FStringLiteral : public Expr {
 public:
  std::string val;
  std::vector<Expr *> args;
};

class IntLiteral : public Expr {
 public:
  int val;
};

class FloatLiteral : public Expr {
 public:
  std::string val;
};

class LongLiteral : public Expr {
 public:
  int val;
};

class Compound : public Expr {
 public:
  Expr *expr1;
  Op op;
  Expr *expr2;
};

class Reference : public Expr {
 public:
  std::string Ident;
  links::LinkedList<std::string> modList;
  bool internal = false;
};

class DeReference : public Expr {
 public:
  std::string Ident;
  links::LinkedList<std::string> modList;
  Type type;
};

class CallExpr : public Expr {
 public:
  Call *call;
};

class parenExpr : public Expr {
 public:
  Expr *expr;
};

class Lambda : public Expr {
 public:
  Function *function;
};

class NewExpr : public Expr {
 public:
  Type type;
  links::LinkedList<Expr *> args;
};

class StructList : public Expr {
 public:
  links::LinkedList<Expr *> args;
};

class Not : public Expr {
 public:
  Expr *expr;
};

class IfExpr : public Expr {
 public:
  Expr *expr;
  Expr *trueExpr;
  Expr *falseExpr;
};

}  // namespace ast

#endif