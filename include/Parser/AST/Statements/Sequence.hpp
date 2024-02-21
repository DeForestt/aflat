#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Sequence
 * @brief This class is used to represent a sequence of statements
 */
class Sequence : public Statement {
 public:
  Statement *Statement1;
  Statement *Statement2;
  Sequence() = default;
};
};  // namespace ast