#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Continue
 * @brief This class is used to represent a continue statement
 */
class Continue : public Statement {
 public:
  int level = 1;
  Continue() = default;
  explicit Continue(int level) : level(level){};
  // copy constructor
  Continue(const Continue &other) : level(other.level){};
  Continue(links::LinkedList<lex::Token *> &tokens);
};
}  // namespace ast