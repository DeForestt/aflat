#pragma once
#include "Parser/AST.hpp"

namespace ast {
class Expr;
class Return : public Statement {
 public:
  Expr *expr;
  bool empty = false;
  bool implicit = false;
  bool resolver =
      false;  // if this return is a `resolve` it does not actually return
              // anything. All it does is stick the expression into eax (this is
              // really meant for match statements)

  gen::GenerationResult const generate(gen::CodeGenerator &generateor) override;
  Return() = default;
  Return(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
};
};  // namespace ast
