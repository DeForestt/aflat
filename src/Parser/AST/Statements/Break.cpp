#include "Parser/AST/Statements/Break.hpp"

namespace ast {
    /**
     * @brief Construct a new Break object
    */
    Break::Break(links::LinkedList<lex::Token *> &tokens)
    {
        this->logicalLine = tokens.peek()->lineCount;
        if (dynamic_cast<lex::INT *>(tokens.peek()) != nullptr)
            this->level = std::stoi(dynamic_cast<lex::INT *>(tokens.pop())->value);
        else
            this->level = 1;

    }
};