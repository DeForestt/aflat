#include "Parser/AST.hpp"

#include <string>

#include "Parser/Parser.hpp"

using namespace ast;

bool Type::compare(const ast::Type &t, const std::string &name) {
  if (t.typeName == name)
    return true;
  return false;
}
