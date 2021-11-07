#ifndef GEN
#define GEN

#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "ASM.hpp"
#include "AST.hpp"

namespace gen{

    class Expr{
        public:
        ASMC::OpType op = ASMC::Hard;
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
        
        // The public Symbol table will hold all public symbols
        // This is to be used externally
        links::LinkedList<Symbol> publicSymbols;

        // The private Symbol table will hold all public and private symbols
        // This is used internally
        links::LinkedList<Symbol> SymbolTable;
        static bool compair(Type * t, std::string ident);
        virtual int poly(){return 0;};
        int size;
    };

    class Class : public Type{
        public:
            // The public Name table will hold all public functions
            // This is to be used externally
            links::SLinkedList<AST::Function, std::string> publicNameTable;
            // The private Name table will hold all public and private functions
            // This is used internally
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
        void prepareCompound(AST::Compound compound, ASMC::File &OutputFile, bool isDiv = false);

        public:
        ASMC::File GenSTMT(AST::Statment * stmt);
        links::LinkedList<gen::Symbol> GenTable(AST::Statment * STMT, links::LinkedList<gen::Symbol> &table);
        CodeGenerator();
    };
}

#endif