#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class DecAssign
 * @brief Represents a declaration and assignment statement.
 */
class DecAssignArr : public Statement {
 public:
  DecArr *declare;
  bool mute = true;
  Expr *expr;
  asmc::File const generate(gen::CodeGenerator &generator) override;
};
}  // namespace ast