#pragma once
#include "Parser/AST.hpp"

namespace ast {
    /*
     * @class Class
     * @brief This class will represent a class
     */
    class Class : public Statement {
    public:
        Ident ident;
        std::string base;
        Statement* contract;
        Statement* statement;
        bool safeType = false;
        bool dynamic = false;
        Class() = default;
        Class(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser, bool safeType, bool dynamic);
    };
};