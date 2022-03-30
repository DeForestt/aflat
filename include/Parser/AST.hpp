#ifndef STRUCT
#define STRUCT

#include "ASM.hpp"
#include "LinkedList.hpp"
#include <string>
#include <vector>

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

class Expr {
public:
  virtual std::string toString() { return ""; };
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
  static bool compair(Type t, std::string name);
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
  Statment *contract;
  Statment *statment;
};

class Function : public Member, public Statment {
public:
  ast::ScopeMod scope;
  Type type;
  std::string scopeName = "";
  Ident ident;
  Statment *args;
  Statment *statment;
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
  Statment *statment;
};

class Declare : public Arg, public Statment {
public:
  ast::ScopeMod scope;
  std::string Ident;
  std::string TypeName;
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
  Expr *expr;
};

class CharLiteral : public Expr {
public:
  char value;
};

class If : public Statment {
public:
  Expr *expr;
  Statment *statment;
  Statment *elseStatment;
  Statment *elseIf;
};

class DecAssign : public Statment {
public:
  Declare *declare;
  Expr *expr;
};

class Assign : public Statment {
public:
  std::string Ident;
  bool refrence = false;
  Expr *expr;
  links::LinkedList<std::string> modList;
};

class While : public Statment {
public:
  Expr *expr;
  Statment *stmt;
};

class For : public Statment {
public:
  Statment *declare;
  ast::Expr *expr;
  Statment *increment;
  Statment *Run;
};

class Call : public Statment {
public:
  std::string ident;
  links::LinkedList<Expr *> Args;
  links::LinkedList<std::string> modList;
  std::string publify = "";
};

class Sequence : public Statment {
public:
  Statment *Statment1;
  Statment *Statment2;
};

class Iflush : public Statment {};

class CWrite : public Statment {
public:
  Expr *expr;
};

class Push : public Statment {
public:
  Expr *expr;
};

class Pull : public Statment {
public:
  Expr *expr;
};

class DecArr : public Statment {
public:
  std::string ident;
  ast::Type type;
  int count;
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
  Expr *expr;
};

class Import : public Statment {
  public:
    std::vector<std::string> imports;
    std::string path;
    std::string nameSpace;
};

class Var : public Expr {
public:
  std::string Ident;
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
  Expr *expr1;
  Op op;
  Expr *expr2;
};

class Refrence : public Expr {
public:
  std::string Ident;
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
  ast::Call *call;
};

class parenExpr : public Expr {
public:
  Expr *expr;
};

class Lambda : public Expr {
public:
  ast::Function *function;
};

class NewExpr : public Expr {
public:
  ast::Type type;
  links::LinkedList<Expr *> args;
};

class Not : public Expr {
public:
  Expr *expr;
};
} // namespace ast

#endif