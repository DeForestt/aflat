#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Dec
 * @brief This class is used to represent an increment statement
 */
class Dec : public Statement {
public:
  std::string ident;
  Dec() = default;
  Dec(const std::string &ident, links::LinkedList<lex::Token *> &tokens);
  gen::GenerationResult const generate(gen::CodeGenerator &generator);
};

} // namespace ast