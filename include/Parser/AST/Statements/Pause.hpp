#pragma once

#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"

namespace ast {
class Pause : public Statement {
public:
  Statement *body = nullptr;
  bool hasBlock = false;

  Pause() = default;
  Pause(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);

  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
} // namespace ast
