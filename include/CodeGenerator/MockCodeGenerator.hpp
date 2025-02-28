#include "CodeGenerator/CodeGenerator.hpp"

namespace test {
namespace mockGen {
class CodeGenerator : public gen::CodeGenerator {
  parse::Parser &parser;

 public:
  CodeGenerator(std::string moduleId, parse::Parser &parser);

  bool canAssign(ast::Type type, std::string typeName, std::string fmt,
                 bool strict = false);
  bool addType(gen::Type *type);
};
}  // namespace mockGen
}  // namespace test