#include "CodeGenerator/MockCodeGenerator.hpp"

namespace test {
namespace mockGen {
bool mockGen::CodeGenerator::canAssign(ast::Type type, std::string typeName,
                                       std::string fmt, bool strict) {
  return gen::CodeGenerator::canAssign(type, typeName, fmt, strict);
}

CodeGenerator::CodeGenerator(std::string moduleId, parse::Parser &parser,
                             const std::string &source, const std::string &cwd)
    : gen::CodeGenerator(moduleId, parser, source, cwd), parser(parser) {}

bool CodeGenerator::addType(gen::Type *type) {
  typeList().push(type);
  return true;
}
} // namespace mockGen
} // namespace test
