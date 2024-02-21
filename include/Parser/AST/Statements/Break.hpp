#pragma once
#include "Parser/AST.hpp"

namespace ast {
    /**
     * @class Break
     * @brief This class is used to represent a continue statement
     */
    class Break : public Statement {
        public:
            int level = 1;
            Break() = default;
            explicit Break(int level) : level(level) {};
            // copy constructor
            Break(const Break &other) : level(other.level) {};
            Break(links::LinkedList<lex::Token *> &tokens);
    };
}