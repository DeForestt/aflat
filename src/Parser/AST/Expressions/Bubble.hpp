#pragma once
#include "Parser/AST.hpp"

namespace ast {
class Bubble : public Expr {
public:
  Expr *expr;
  Bubble(Expr *expr) : expr(expr) {}
  gen::GenerationResult const
  generateExpression(gen::CodeGenerator &generator, asmc::Size size,
                     std::string typeHint = "") override;
};
} // namespace ast
