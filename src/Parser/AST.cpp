#include "Parser/AST.hpp"

#include <string>

#include "Parser/Parser.hpp"

bool ast::Type::compare(ast::Type t, std::string name) {
  if (t.typeName == name) return true;
  return false;
}