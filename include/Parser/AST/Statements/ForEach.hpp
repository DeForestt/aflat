#pragma once
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"

namespace ast {
/*
 * @class ForEach
 * @brief this class will represent a for each loop
 */
class ForEach : public Statement {
public:
  Statement *implementation = nullptr;
  std::string binding_identifier;
  Expr *iterator;
  ForEach() = default;
  ForEach(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
} // namespace ast
