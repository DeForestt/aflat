#pragma once

#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"

namespace ast {
class Await : public Expr {
public:
  Expr *expr = nullptr;

  Await() = default;
  Await(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);

  gen::GenerationResult const
  generateExpression(gen::CodeGenerator &generator, asmc::Size size,
                     std::string typeHint = "") override;
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
} // namespace ast
