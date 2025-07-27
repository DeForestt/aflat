#ifndef STRUCT
#define STRUCT

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "ASM.hpp"
#include "CodeGenerator/GenerationResult.hpp"
#include "LinkedList.hpp"
#include "Scanner.hpp"

namespace parse {
class Parser;
struct Annotation;
};  // namespace parse

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

enum class WhenOperator {
  IS,
  HAS,
};

enum class WhenJoiner {
  AND,
  OR,
};

class WhenPredicat {
 public:
  bool negated = false;
  WhenOperator op;
  std::string typeName;
  std::string ident;
  WhenJoiner join = WhenJoiner::AND;  // relation to next predicate
};

class When {
 public:
  std::vector<WhenPredicat> predicates;
};

class Statement {
 public:
  std::optional<When> when;  // When clause for templates
  bool locked = false;
  int logicalLine = 0;
  virtual std::string toString() { return ""; };
  virtual void replaceTypes(std::unordered_map<std::string, std::string> map);
  virtual void namespaceSwap(std::unordered_map<std::string, std::string> map);
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
  bool selling = false;
  std::string typeCast = "";
  virtual gen::GenerationResult const generateExpression(
      gen::CodeGenerator &generator, asmc::Size size,
      std::string typeHint = "");
};

class ConditionalExpr {
 public:
  Expr *expr1;
  Op op;
  Expr *expr2;

  std::string toString() {
    return expr1->toString() + " " + " " + expr2->toString();
  }
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
  bool readOnly = false;
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
      : typeName(typeName), size(size) {};
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
  std::string toString() override;
};

class ParenExpr : public Expr {
 public:
  Expr *expr;
  std::string toString() override { return "(" + expr->toString() + ")"; }
};

class CharLiteral : public Expr {
 public:
  char value;
  std::string toString() override { return std::string(1, value); }
};

class Iflush : public Statement {};

class CWrite : public Statement {
 public:
  Expr *expr;
  std::string toString() override { return "cwrite(" + expr->toString() + ")"; }
};

class Push : public Statement {
 public:
  Expr *expr;
  std::string toString() override { return "push(" + expr->toString() + ")"; }
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

  std::string toString() override {
    std::string result = Ident;
    for (int i = 0; i < modList.count; i++) {
      result += "." + modList.get(i);
    }
    return result;
  }
};

class Buy : public Expr {
 public:
  Expr *expr;
  std::string toString() override { return "$" + expr->toString(); }
};

class StringLiteral : public Expr {
 public:
  std::string val;

  std::string toString() override { return "\"" + val + "\""; }
};

class FStringLiteral : public Expr {
 public:
  std::string original;
  std::string val;
  std::vector<Expr *> args;

  std::string toString() override { return "`" + original + "`"; }
};

class IntLiteral : public Expr {
 public:
  int val;
  std::string toString() override { return std::to_string(val); }
};

class FloatLiteral : public Expr {
 public:
  std::string val;
  std::string toString() override { return val; }
};

class LongLiteral : public Expr {
 public:
  int val;
  std::string toString() override { return std::to_string(val); }
};

class Compound : public Expr {
 public:
  Expr *expr1;
  Op op;
  Expr *expr2;
  std::string toString() override;
};

class Reference : public Expr {
 public:
  std::string Ident;
  links::LinkedList<std::string> modList;
  bool internal = false;

  std::string toString() override {
    std::string result = "?" + Ident;
    for (int i = 0; i < modList.count; i++) {
      result += "." + modList.get(i);
    }
    return result;
  }
};

class DeReference : public Expr {
 public:
  std::string Ident;
  links::LinkedList<std::string> modList;
  Type type;

  std::string toString() override {
    std::string result = Ident;
    for (int i = 0; i < modList.count; i++) {
      result += "." + modList.get(i);
    }

    result = result + "as" + type.typeName;
    return result;
  }
};

class CallExpr : public Expr {
 public:
  Call *call;

  std::vector<std::string> templateTypes;
  std::string toString() override;
};

class parenExpr : public Expr {
 public:
  Expr *expr;

  std::string toString() override { return "(" + expr->toString() + ")"; }
};

class Lambda : public Expr {
 public:
  Function *function;
};

class NewExpr : public Expr {
 public:
  Type type;
  links::LinkedList<Expr *> args;
  std::vector<std::string> templateTypes;
  bool castableMap = false;

  std::string toString() override {
    std::string result = "new " + type.typeName;
    if (args.count > 0) {
      result += "(";
      for (int i = 0; i < args.count; i++) {
        result += args.get(i)->toString();
        if (i != args.count - 1) {
          result += ", ";
        }
      }
      result += ")";
    }
    return result;
  }
};

class StructList : public Expr {
 public:
  links::LinkedList<Expr *> args;
};

class Not : public Expr {
 public:
  Expr *expr;
  std::string toString() override { return "!" + expr->toString(); }
};

class IfExpr : public Expr {
 public:
  Expr *expr;
  Expr *trueExpr;
  Expr *falseExpr;
  std::string toString() override {
    return "if" + expr->toString() + "" + trueExpr->toString() + "else" +
           falseExpr->toString();
  }
};

class UnionConstructor : public Expr {
 public:
  ast::Type unionType;
  std::string variantName;
  Expr *expr;
  bool dynamic;
  std::vector<std::string> templateTypes;
  std::string toString() override {
    return unionType.typeName + "->" + variantName + "(" + expr->toString() +
           ")";
  }
  UnionConstructor(Type unionType, std::string variantName, Expr *expr,
                   bool dynamic, std::vector<std::string> templateTypes)
      : unionType(unionType),
        variantName(variantName),
        expr(expr),
        dynamic(dynamic),
        templateTypes(std::move(templateTypes)) {}
  UnionConstructor() = default;

  gen::GenerationResult const generateExpression(
      gen::CodeGenerator &generator, asmc::Size size,
      std::string typeHint = "") override;

  gen::GenerationResult const getStaticExpr(gen::CodeGenerator &generator,
                                            asmc::Size size,
                                            std::string typeHint = "");

  gen::GenerationResult const getDynamicExpr(gen::CodeGenerator &generator,
                                             asmc::Size size,
                                             std::string typeHint = "");
};

Statement *deepCopy(const Statement *stmt);

}  // namespace ast

#endif
