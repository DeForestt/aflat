#pragma once
#include "Parser/AST.hpp"

namespace ast {
/*
 * @class Struct
 * @brief This class will represent a struct
 */
class Struct : public Member, public Statement {
public:
  Ident ident;
  Statement *statement;
  bool uniqueType = false;
  Struct() = default;
  Struct(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser,
         bool uniqueType);
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
} // namespace ast
