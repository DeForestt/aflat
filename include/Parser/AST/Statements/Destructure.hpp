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
  bool mute;
  Destructure() = default;
  Destructure(const bool mute, links::LinkedList<lex::Token *> &tokens,
              parse::Parser &parser);
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
};  // namespace ast