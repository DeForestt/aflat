#pragma once
#include <unordered_map>

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
  std::string cwd;
  Import() = default;
  Import(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
  gen::GenerationResult const generate(gen::CodeGenerator &generator);
  gen::GenerationResult const generateClasses(gen::CodeGenerator &generator);
};
void collectImportNamespaces(ast::Statement *stmt,
                             std::unordered_map<std::string, std::string> &map);
};  // namespace ast
