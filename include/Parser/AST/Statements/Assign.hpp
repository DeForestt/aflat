#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Assign
 * @brief This class is used to represent an assignment statement
 */
class Assign : public Statement {
 public:
  std::string Ident;
  bool reference = false;
  bool override = false;
  Expr *expr;
  links::LinkedList<std::string> modList;
  links::LinkedList<Expr *> indices;
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  Assign() = default;
  Assign(const std::string &ident, const links::LinkedList<Expr *> &indices,
         links::LinkedList<std::string> modList,
         links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
};
}  // namespace ast