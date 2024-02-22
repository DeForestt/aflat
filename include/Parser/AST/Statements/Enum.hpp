#pragma once
#include "Scanner.hpp"
#include "Parser/Parser.hpp"

namespace ast
{
    /**
     * @class Enum
     * @brief This class will represent an enum
     */
    class Enum : public Statement {
        public:
            std::string Ident;
            std::vector<std::string> values;
            Enum() = default;
            Enum(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
    };
} // namespace ast
