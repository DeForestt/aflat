#pragma once
#include "Parser/AST.hpp"

namespace ast {
class Note : public Statement {
 public:
  std::string message;
  Note() = default;
  explicit Note(const std::string &msg) : message(msg) {}
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
}  // namespace ast
