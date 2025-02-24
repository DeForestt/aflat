#pragma once
#include "Parser/AST.hpp"
#include "Parser/AST/Statements/Return.hpp"

namespace ast {
class Expr;
class Return : public Statement {
 public:
  Expr *expr;
  bool empty = false;

  gen::GenerationResult const generate(gen::CodeGenerator &generateor) override;
  Return() = default;
  Return(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
};
};  // namespace ast
