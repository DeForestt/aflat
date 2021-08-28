#ifndef GEN
#define GEN

#include "AST.hpp"
#include "LinkedList.hpp"
#include "ASM.hpp"



namespace gen{
    class Symbol{
        public:
        std::string symbol;
        int byteMod;
    };
    class CodeGenerator{
        private:
        links::LinkedList<Symbol> SymbolTable;
        std::string intArgs[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        int intArgsCounter;
        std::string GenExpr(AST::Expr * expr);
        
        public:
        ASMC::File GenSTMT(AST::Statment * stmt);
    };
}

#endif