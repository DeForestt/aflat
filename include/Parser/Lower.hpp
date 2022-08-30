#pragma once

#include "Parser/AST.hpp"

namespace parse {
namespace lower {
class Lowerer {
public:
  Lowerer(ast::Statment *root);

private:
  ast::Statment *root;
  ast::Statment *curr;
  bool inclass = false;
  std::string className = "";

  ast::Statment *lower(ast::Statment *stmt);
  ast::Statment *lowerFunction(ast::Function *func);
  ast::Function *findFunction(ast::Statment *stmt, std::string ident,
                              bool &fromClass);
  ast::Class *findClass(ast::Statment *stmt, std::string ident);
};
} // namespace lower
} // namespace parse
