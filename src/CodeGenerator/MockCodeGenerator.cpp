#include "CodeGenerator/MockCodeGenerator.hpp"

namespace test
{
    namespace mockGen
    {
        bool mockGen::CodeGenerator::canAssign(ast::Type type, std::string typeName, bool strict = false) {
            return gen::CodeGenerator::canAssign(type, typeName, strict);
        }

        CodeGenerator::CodeGenerator(std::string moduleId) : gen::CodeGenerator(moduleId) {

        }

        bool CodeGenerator::addType(gen::Type* type) {
            this->typeList.push(type);
            return true;
        }
    }
}
    