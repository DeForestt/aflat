#ifndef PARSE
#define PARSE


#include <string>
#include "AST.hpp"
#include "LinkedList.hpp"
#include "Scanner.hpp"

namespace parse{
    class Parser{
        public:
        AST::Statment Output;
        AST::Statment* parseStmt(links::LinkedList<lex::Token *> &tokens);
        AST::Statment* parseArgs(links::LinkedList<lex::Token *> &tokens);
        AST::Statment* parseExpr(links::LinkedList<lex::Token *> &tokens);
    };
};

#endif
