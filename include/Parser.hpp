#ifndef PARSE
#define PARSE


#include <string>
#include "AST.hpp"
#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "Scanner.hpp"

namespace parse{
    class Parser{
        public:
        AST::Statment Output;
        AST::Statment* parseStmt(links::LinkedList<lex::Token *> &tokens, bool singleStmt = false);
        Parser();
        
        private:
        links::SLinkedList<AST::Type, std::string> typeList;
        AST::Expr* parseExpr(links::LinkedList<lex::Token *> &tokens);
        AST::Statment* parseArgs(links::LinkedList<lex::Token *> &tokens, char delimn, char close);
        AST::ConditionalExpr* parseCondition(links::LinkedList<lex::Token *> &tokens);
    };
};

#endif
