#ifndef GEN
#define GEN

#include "AST.hpp"
#include "LinkedList.hpp"
#include "ASM.hpp"



namespace gen{
    class Symbol{
        public:
        std::string symbol;
        AST::Type type;
        int byteMod;
    };
    class CodeGenerator{
        private:
        links::LinkedList<Symbol> SymbolTable;
        ASMC::Register intArgs[6] = { ASMC::Register("rdi", "edi", "di", "dil"),
                                    ASMC::Register("rsi", "esi", "si", "sil"),
                                    ASMC::Register("rdx", "edx", "dx", "dl"),
                                    ASMC::Register("rcx", "ecx", "cx", "cl"),
                                    ASMC::Register("r8", "r8d", "r8w", "r8b"),
                                    ASMC::Register("r9", "r9d", "r9w", "r9b")};
        int intArgsCounter;
        std::string GenExpr(AST::Expr * expr, ASMC::File &OutputFile);
        public:
        ASMC::File GenSTMT(AST::Statment * stmt);
        CodeGenerator();
    };
}

#endif