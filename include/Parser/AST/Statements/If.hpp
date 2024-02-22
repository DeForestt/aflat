#pragma once
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"
#include "LinkedList.hpp"

namespace ast {
    /*
    * @class If
    * @brief This class will be used to represent an if statement
    */
    class If : public Statement {
        public:
        Expr* expr;
        Statement* statement;
        Statement* elseStatement;
        Statement* elseIf;
        If() = default;
        If(links::LinkedList<lex::Token*> &tokens, parse::Parser &parser);
    };
} // namespace ast