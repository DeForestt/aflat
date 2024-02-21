#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Inc
 * @brief This class is used to represent an increment statement
 */
class Inc : public Statement {
 public:
  std::string ident;
  Inc() = default;
  Inc(const std::string &ident, links::LinkedList<lex::Token *> &tokens);
};

}  // namespace ast