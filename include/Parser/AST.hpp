#ifndef STRUCT
#define STRUCT

#include <string>
#include <vector>

#include "ASM.hpp"
#include "LinkedList.hpp"
#include "Scanner.hpp"

namespace parse {
  class Parser;
};

namespace ast {
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
  Expr * extention = nullptr;
  std::string typeCast = "";
};

class ConditionalExpr {
 public:
  Expr* expr1;
  Op op;
  Expr* expr2;
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
  ast::Type * typeHint;

  struct FPointerArgs {
    ast::Type * returnType = nullptr;
    int requiredArgs = 0;
    std::vector<ast::Type> argTypes;
    std::string id;
    asmc::Size size;
    bool isFPointer = false;
  };

  FPointerArgs fPointerArgs;

  Type() = default;
  Type(const std::string &typeName, const asmc::Size &size) : typeName(typeName), size(size) {};
};

class Arg {
 public:
  virtual std::string toString() { return ""; };
};

class Program {
 public:
  ProgramMember members;
};

class Function : public Member, public Statement {
 public:
  ast::ScopeMod scope;
  Type type;
  std::string scopeName = "";
  std::string decorator = "";
  std::string decNSP = "";
  Ident ident;
  Statement* args;
  Statement* statement;
  ast::Op op;
  int req;
  std::vector<ast::Type> argTypes;
  links::LinkedList<Expr*> decoratorArgs;
  bool isLambda = false;
  bool flex = false;
  bool mask;
  bool has_return = false;
};

class Declare : public Arg, public Statement {
 public:
  ast::ScopeMod scope;
  std::string Ident;
  std::string TypeName;
  bool mut = true;
  bool mask;
  Type type;
};

class Argument : public Arg, public Statement {
 public:
  std::string Ident;
  Type type;
};

class ParenExpr : public Expr {
 public:
  Expr* expr;
};

class CharLiteral : public Expr {
 public:
  char value;
};

class DecAssign : public Statement {
 public:
  Declare* declare;
  bool mute = true;
  Expr* expr;
};

class Sequence : public Statement {
 public:
  Statement* Statement1;
  Statement* Statement2;
};

class Iflush : public Statement {};

class CWrite : public Statement {
 public:
  Expr* expr;
};

class Push : public Statement {
 public:
  Expr* expr;
};

class Pull : public Statement {
 public:
  Expr* expr;
};

class DecArr : public Statement {
 public:
  std::string ident;
  ast::Type type;
  int count;
  bool mut = true;
  ast::ScopeMod scope;
  links::LinkedList<Expr*> indices;
};

class DecAssignArr : public Statement {
 public:
  DecArr* declare;
  bool mute = true;
  Expr* expr;
};

class Inc : public Statement {
 public:
  std::string ident;
};

class Dec : public Statement {
 public:
  std::string ident;
};

class Var : public Expr {
 public:
  std::string Ident;
  links::LinkedList<ast::Expr*> indices;
  links::LinkedList<std::string> modList;
  bool internal = false;
};

class StringLiteral : public Expr {
 public:
  std::string val;
};

class FStringLiteral : public Expr {
 public:
  std::string val;
  std::vector<Expr*> args;
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
  Expr* expr1;
  Op op;
  Expr* expr2;
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
  ast::Type type;
};

class CallExpr : public Expr {
 public:
  ast::Call* call;
};

class parenExpr : public Expr {
 public:
  Expr* expr;
};

class Lambda : public Expr {
 public:
  ast::Function* function;
};

class NewExpr : public Expr {
 public:
  ast::Type type;
  links::LinkedList<Expr*> args;
};

class StructList : public Expr {
 public:
  links::LinkedList<ast::Expr*> args;
};

class Not : public Expr {
 public:
  Expr* expr;
};

class IfExpr : public Expr {
  public:
  Expr* expr;
  Expr* trueExpr;
  Expr* falseExpr;
};

}  // namespace ast

#endif