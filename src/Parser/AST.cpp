#include "Parser/AST.hpp"

#include <string>

bool ast::Type::compair(ast::Type t, std::string name) {
  if (t.typeName == name) return true;
  return false;
}