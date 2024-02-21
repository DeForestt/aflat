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
  std::string scopeName = "";
  std::string decorator = "";
  std::string decNSP = "";
  Ident ident;
  Statement *args;
  Statement *statement;
  Op op;
  int req = 0;
  std::vector<Type> argTypes;
  links::LinkedList<Expr *> decoratorArgs;
  bool isLambda = false;
  bool flex = false;
  bool mask;
  bool has_return = false;

  Function() = default;
  Function(const string &ident, const ScopeMod &scope, const Type &type,
           const Op op, const std::string &scopeName,
           links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  gen::Expr toExpr(gen::CodeGenerator &generator);
};
};  // namespace ast