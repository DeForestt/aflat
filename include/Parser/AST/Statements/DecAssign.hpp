#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class DecAssign
 * @brief Represents a declaration and assignment statement.
 */
class DecAssign : public Statement {
 public:
  Declare *declare;
  bool mute = true;
  Expr *expr;
  DecAssign() = default;
  DecAssign(Declare *declare, const bool mute,
            links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
};
}  // namespace ast