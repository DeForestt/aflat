#pragma once
#include "LinkedList.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"

namespace ast {
/*
 * @class If
 * @brief This class will be used to represent an if statement
 */
class If : public Statement {
 public:
  Expr *expr;
  Statement *statement;
  Statement *elseStatement;
  Statement *elseIf;
  If() = default;
  If(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
}  // namespace ast