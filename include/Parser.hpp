#ifndef PARSE
#define PARSE


#include <string>
#include "AST.hpp"
#include "LinkedList.hpp"
#include "Scanner.hpp"

namespace parse{
    class Parser{
        public:
        AST::Stmt Output;
        AST::Stmt* parseStmt(links::LinkedList<lex::Token *> &tokens);
        AST::Stmt* parseArgs(links::LinkedList<lex::Token *> &tokens);
        AST::Expr* parseExpr(links::LinkedList<lex::Token *> &tokens);
    };
};

#endif
