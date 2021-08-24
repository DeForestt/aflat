#ifndef GEN
#define Gen

#include "AST.hpp"
#include "LinkedList.hpp"
#include "ASM.hpp"


namespace Gen{
    class CodeGenerator{
        public:
        links::LinkedList<ASM::Instruction> Generate();
    };
}

#endif