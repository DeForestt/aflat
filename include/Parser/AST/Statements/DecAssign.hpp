#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class DecAssign
 * @brief Represents a declaration and assignment statement.
 */
class DecAssign : public Statement {
 public:
  std::vector<parse::Annotation> annotations;

  Declare *declare;
  bool mute = true;
  Expr *expr;
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  DecAssign() = default;
  DecAssign(Declare *declare, const bool mute,
            links::LinkedList<lex::Token *> &tokens, parse::Parser &parser,
            const std::vector<parse::Annotation> &annotations);

  DecAssign(const DecAssign &other)
      : declare(other.declare), mute(other.mute), expr(other.expr) {
    this->logicalLine = other.logicalLine;
  }
  void debug();
};
}  // namespace ast