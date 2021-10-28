#ifndef GEN
#define GEN

#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "ASM.hpp"
#include "AST.hpp"

namespace gen{
   
    enum OpType{
        Float,
        Hard,
    };

    class Expr{
        public:
        gen::OpType op;
        std::string access;
        ASMC::Size size;
    };
    
    class Symbol{
        public:
        std::string symbol;
        AST::Type type;
        bool mask;
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

        #pragma region State Variables
            gen::Class * scope;
            AST::Type returnType;
            int scopePop = 0;
            int lablecount = 0; 
            int intArgsCounter;
            int selectReg = 0;
            bool globalScope = true;
        #pragma endregion

        #pragma region Item Lists
            links::SLinkedList<gen::Type *, std::string> typeList;
            links::SLinkedList<ASMC::Register, std::string> registers;
            links::LinkedList<Symbol> SymbolTable;
            links::LinkedList<Symbol> GlobalSymbolTable;
            links::SLinkedList<AST::Function, std::string> nameTable;
            links::SLinkedList<AST::Type, std::string> TypeList;
        #pragma endregion

        int getBytes(ASMC::Size size);
        
        ASMC::Register intArgs[6] = { ASMC::Register("rdi", "edi", "di", "dil"),
                                    ASMC::Register("rsi", "esi", "si", "sil"),
                                    ASMC::Register("rdx", "edx", "dx", "dl"),
                                    ASMC::Register("rcx", "ecx", "cx", "cl"),
                                    ASMC::Register("r8", "r8d", "r8w", "r8b"),
                                    ASMC::Register("r9", "r9d", "r9w", "r9b")};

        void GenArgs(AST::Statment * STMT, ASMC::File &OutputFile);
        AST::Function GenCall(AST::Call * call, ASMC::File &OutputFile);
        Expr GenExpr(AST::Expr * expr, ASMC::File &OutputFile);

        public:
        ASMC::File GenSTMT(AST::Statment * stmt);
        links::LinkedList<gen::Symbol> GenTable(AST::Statment * STMT, links::LinkedList<gen::Symbol> &table);
        CodeGenerator();
    };
}

#endif