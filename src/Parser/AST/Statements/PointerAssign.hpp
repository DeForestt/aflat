#pragma once

#include "Parser/AST.hpp"

namespace ast {
class PointerAssign : public Statement {
public:
  Expr *target = nullptr;
  Expr *expr = nullptr;

  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
} // namespace ast
