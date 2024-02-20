#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include <string>

bool ast::Type::compare(ast::Type t, std::string name) {
  if (t.typeName == name) return true;
  return false;
}