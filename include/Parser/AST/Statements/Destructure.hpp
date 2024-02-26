#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Destructure
 * @brief Node for destructuring assignment
 * let {a, b} = Identifier; or let {a, b} = {1, 2};
 */
class Destructure : public Statement {
 public:
  std::vector<std::string> identifiers;
  ast::Expr *expr;
  Destructure() = default;
  Destructure(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
};
};  // namespace ast