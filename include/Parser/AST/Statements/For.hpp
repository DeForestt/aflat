#pragma once
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"

namespace ast {
/*
 * @class For
 * @brief this class will represent a for loop
 */
class For : public Statement {
 public:
  Statement *declare;
  Expr *expr;
  Statement *increment;
  Statement *Run;
  For() = default;
  For(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};

}  // namespace ast
