#ifndef GEN
#define GEN

#include "AST.hpp"
#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "ASM.hpp"



namespace gen{
    class Expr{
        public:
        std::string access;
        ASMC::Size size;
    };
    class Symbol{
        public:
        std::string symbol;
        AST::Type type;
        int byteMod;
    };

    class Type{
        public:
        std::string Ident;
        links::LinkedList<Symbol> SymbolTable;
        static bool compair(Type * t, std::string ident);
        virtual int poly(){return 0;};
        int size;
    };

    class Class : public Type{
        public:
            links::SLinkedList<AST::Function, std::string> nameTable;
            static bool compair(Type * t, std::string ident);
    };

    class CodeGenerator{
        private:
        gen::Class * scope;
        int scopePop = 0;
        links::SLinkedList<gen::Type *, std::string> typeList;
        links::SLinkedList<ASMC::Register, std::string> registers;
        links::LinkedList<Symbol> SymbolTable;
        links::SLinkedList<AST::Function, std::string> nameTable;
        links::SLinkedList<AST::Type, std::string> TypeList;

        int getBytes(ASMC::Size size);
        
        ASMC::Register intArgs[6] = { ASMC::Register("rdi", "edi", "di", "dil"),
                                    ASMC::Register("rsi", "esi", "si", "sil"),
                                    ASMC::Register("rdx", "edx", "dx", "dl"),
                                    ASMC::Register("rcx", "ecx", "cx", "cl"),
                                    ASMC::Register("r8", "r8d", "r8w", "r8b"),
                                    ASMC::Register("r9", "r9d", "r9w", "r9b")};
        int lablecount = 0;
        int intArgsCounter;
        int selectReg = 0;
        AST::Type returnType;
        void GenArgs(AST::Statment * STMT ,ASMC::File &OutputFile);
        Expr GenExpr(AST::Expr * expr, ASMC::File &OutputFile);
        public:
        ASMC::File GenSTMT(AST::Statment * stmt);
        links::LinkedList<gen::Symbol> GenTable(AST::Statment * STMT, links::LinkedList<gen::Symbol> &table);
        CodeGenerator();
    };
}

#endif