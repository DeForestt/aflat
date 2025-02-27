#pragma once
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"

namespace ast {
/**
 * @class Function
 * @brief Represents a function declaration.
 */
class Function : public Member, public Statement {
 public:
  ScopeMod scope;
  Type type;
  Type useType = Type("", asmc::QWord);
  std::string scopeName = "global";
  std::string decorator = "";
  std::string decNSP = "";
  bool hidden = false;
  Ident ident;
  Statement *args;
  Statement *statement;
  Op op;
  int req = 0;
  std::vector<int> optConvertionIndices;  // indices of optional arguments that
                                          // need to be converted
  std::vector<Type> argTypes;
  links::LinkedList<Expr *> decoratorArgs;
  std::vector<bool> mutability;
  bool isLambda = false;
  bool flex = false;
  bool mask;
  bool has_return = false;
  bool optional = false;
  bool autoType = false;

  Function() = default;
  Function(const string &ident, const ScopeMod &scope, const Type &type,
           const Op op, const std::string &scopeName,
           links::LinkedList<lex::Token *> &tokens, parse::Parser &parser,
           bool optional);
  Function(const ScopeMod &scope, links::LinkedList<lex::Token *> &tokens,
           parse::Parser &parser);
  Function(const Function &Other, bool locked)
      : scope(Other.scope),
        type(Other.type),
        op(Other.op),
        scopeName(Other.scopeName),
        ident(Other.ident),
        args(Other.args),
        statement(Other.statement),
        decorator(Other.decorator),
        decoratorArgs(Other.decoratorArgs),
        argTypes(Other.argTypes),
        optConvertionIndices(Other.optConvertionIndices),
        mutability(Other.mutability),
        isLambda(Other.isLambda),
        mask(Other.mask),
        has_return(Other.has_return),
        optional(Other.optional),
        useType(Other.useType) {
    this->logicalLine = Other.logicalLine;
    this->locked = locked;
    this->hidden = this->hidden;
  }
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  gen::Expr toExpr(gen::CodeGenerator &generator);

 private:
  void parseFunctionBody(links::LinkedList<lex::Token *> &tokens,
                         parse::Parser &parser);
};
};  // namespace ast
