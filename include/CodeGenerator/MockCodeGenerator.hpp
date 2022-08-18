#include "CodeGenerator/CodeGenerator.hpp"

namespace test {
    namespace mockGen {
        class CodeGenerator : public gen::CodeGenerator {
            public:
            CodeGenerator(std::string moduleId);
            
            bool canAssign(ast::Type type, std::string typeName, bool strict = false);
            bool addType(gen::Type * type);
        };
    }
}