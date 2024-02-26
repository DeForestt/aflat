#pragma once
#include "Parser/Parser.hpp"
#include "Scanner.hpp"

namespace ast {
/**
 * @class Enum
 * @brief This class will represent an enum
 */
class Enum : public Statement {
 public:
  std::string Ident;
  std::vector<std::string> values;
  Enum() = default;
  Enum(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
}  // namespace ast
