#pragma once

#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"

namespace parse {
namespace lower {
class Lowerer {
 public:
  Lowerer(ast::Statement *root);

 private:
  ast::Statement *root;
  ast::Statement *curr;
  bool inclass = false;
  std::string className = "";

  ast::Statement *lower(ast::Statement *stmt);
  ast::Statement *lowerFunction(ast::Function *func);
  ast::Function *findFunction(ast::Statement *stmt, std::string ident,
                              bool &fromClass);
  ast::Class *findClass(ast::Statement *stmt, std::string ident);
};
}  // namespace lower
}  // namespace parse
