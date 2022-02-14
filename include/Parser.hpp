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
        ast::Statment Output;
        ast::Statment* parseStmt(links::LinkedList<lex::Token *> &tokens, bool singleStmt = false);
        Parser();
        
        private:
        links::SLinkedList<ast::Type, std::string> typeList;
        ast::Expr* parseExpr(links::LinkedList<lex::Token *> &tokens);
        ast::Statment* parseArgs(links::LinkedList<lex::Token *> &tokens, char delimn, char close);
        ast::ConditionalExpr* parseCondition(links::LinkedList<lex::Token *> &tokens);
    };
};

#endif
