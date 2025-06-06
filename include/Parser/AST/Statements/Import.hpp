#pragma once
#include "Parser/AST.hpp"

namespace ast {
/*
 * @class Import
 * @brief This class is used to represent an import statement
 */
class Import : public Statement {
 public:
  std::vector<std::string> imports;
  std::string path;
  std::string nameSpace;
  bool hasClasses = false;
  bool hasFunctions = false;
  Import() = default;
  Import(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
  gen::GenerationResult const generate(gen::CodeGenerator &generator);
};
};  // namespace ast