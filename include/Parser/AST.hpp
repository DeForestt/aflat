#ifndef STRUCT
#define STRUCT

#include <string>
#include <vector>

#include "ASM.hpp"
#include "LinkedList.hpp"

namespace ast {
class ProgramMember {
 public:
  virtual std::string toString() { return ""; };
};

class Ident {
 public:
  std::string ident;
};

class Statment {
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

class Expr : public Statment {
 public:
  virtual std::string toString() { return ""; };
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
  static bool compair(Type t, std::string name);
  Type* typeHint = nullptr;
  links::LinkedList<int> indecies;
  bool isGeneric = false;
  Type() = default;
  Type(std::string typeName, asmc::Size size) : typeName(typeName), size(size) {};
};

class Arg {
 public:
  virtual std::string toString() { return ""; };
};

class Program {
 public:
  ProgramMember members;
};

class Class : public Statment {
 public:
  Ident ident;
  std::string base;
  Statment* contract;
  Statment* statment;
};

class Function : public Member, public Statment {
 public:
  ast::ScopeMod scope;
  Type type;
  std::string scopeName = "";
  std::string decorator = "";
  std::string decNSP = "";
  Ident ident;
  Statment* args;
  Statment* statment;
  ast::Op op;
  int req;
  std::vector<ast::Type> argTypes;
  bool isLambda = false;
  bool flex = false;
  bool mask;
};

class UDeffType : public Member, public Statment {
 public:
  Ident ident;
  Statment* statment;
};

class Declare : public Arg, public Statment {
 public:
  ast::ScopeMod scope;
  std::string Ident;
  std::string TypeName;
  bool mut = true;
  bool mask;
  Type type;
};

class Argument : public Arg, public Statment {
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

class If : public Statment {
 public:
  Expr* expr;
  Statment* statment;
  Statment* elseStatment;
  Statment* elseIf;
};

class DecAssign : public Statment {
 public:
  Declare* declare;
  bool mute = true;
  Expr* expr;
};

class Assign : public Statment {
 public:
  std::string Ident;
  bool refrence = false;
  bool override = false;
  Expr* expr;
  links::LinkedList<std::string> modList;
  links::LinkedList<ast::Expr*> indices;
};

class While : public Statment {
 public:
  Expr* expr;
  Statment* stmt;
};

class For : public Statment {
 public:
  Statment* declare;
  ast::Expr* expr;
  Statment* increment;
  Statment* Run;
};

class Call : public Statment {
 public:
  std::string ident;
  links::LinkedList<Expr*> Args;
  links::LinkedList<std::string> modList;
  std::string publify = "";
};

class Delete : public Statment {
 public:
  std::string ident;
  links::LinkedList<std::string> modList;
};

class Sequence : public Statment {
 public:
  Statment* Statment1;
  Statment* Statment2;
};

class Iflush : public Statment {};

class CWrite : public Statment {
 public:
  Expr* expr;
};

class Push : public Statment {
 public:
  Expr* expr;
};

class Pull : public Statment {
 public:
  Expr* expr;
};

class DecArr : public Statment {
 public:
  std::string ident;
  ast::Type type;
  int count;
  bool mut = true;
  ast::ScopeMod scope;
  links::LinkedList<Expr*> indices;
};

class DecAssignArr : public Statment {
 public:
  DecArr* declare;
  bool mute = true;
  Expr* expr;
};

class Inc : public Statment {
 public:
  std::string ident;
};

class Dec : public Statment {
 public:
  std::string ident;
};

class Return : public Statment {
 public:
  Expr* expr;
};

class Import : public Statment {
 public:
  std::vector<std::string> imports;
  std::string path;
  std::string nameSpace;
};

class Enum : public Statment {
  public:
  std::string Ident;
  std::vector<std::string> values;
};

class Var : public Expr {
 public:
  std::string Ident;
  links::LinkedList<ast::Expr*> indecies;
  links::LinkedList<std::string> modList;
};

class StringLiteral : public Expr {
 public:
  std::string val;
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

class Refrence : public Expr {
 public:
  std::string Ident;
  links::LinkedList<std::string> modList;
  bool internal = false;
};

class DeRefence : public Expr {
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
}  // namespace ast

#endif