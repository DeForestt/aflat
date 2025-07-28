#pragma once
#include "Parser/AST.hpp"

namespace ast {
class Ellipsis : public Statement {
 public:
  Ellipsis() = default;
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
}  // namespace ast
