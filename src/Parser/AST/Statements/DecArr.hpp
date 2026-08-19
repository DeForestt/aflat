#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class DecArr
 * @brief Represents a declaration of an array.
 */
class DecArr : public Statement {
public:
  std::string ident;
  Type type;
  int count;
  bool mut = true;
  ScopeMod scope;
  links::LinkedList<Expr *> indices;
  DecArr() = default;
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};

} // namespace ast