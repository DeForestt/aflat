#pragma once
#include "Parser/AST.hpp"
#include "Parser/AST/Statements/Return.hpp"

namespace ast {
class Expr;
class Return : public Statement {
 public:
  Expr *expr;

  Return() = default;
  Return(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
};
};  // namespace ast