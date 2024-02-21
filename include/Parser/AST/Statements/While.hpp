#pragma once
#include "Parser/AST.hpp"

namespace ast
{
    /*
     * @class While
     * @brief This class will be used to represent a while statement
     */
    class While : public Statement {
        public:
        Expr* expr;
        Statement* stmt;
        While() = default;
        While(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
    };
} // namespace ast


