#pragma once
#include "Parser/AST.hpp"

namespace ast {
/*
 * @class Class
 * @brief This class will represent a class
 */
class Class : public Statement {
 public:
  Ident ident;
  std::string base;
  std::vector<std::string> genericTypes = std::vector<std::string>();
  std::vector<parse::Annotation> annotations;
  Statement *contract;
  Statement *statement;
  bool safeType = false;
  bool dynamic = false;
  bool pedantic = false;
  bool includer = false;
  bool hidden = false;
  Class() = default;
  Class(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser,
        bool safeType, bool dynamic, bool pedantic,
        std::vector<parse::Annotation> &annotations,
        std::vector<std::string> &genericTypes);
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
};  // namespace ast
