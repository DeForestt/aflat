#include "Parser/AST/Statements/DecAssign.hpp"
#include "Parser/Parser.hpp"

namespace ast {
    DecAssign::DecAssign(Declare* declare, const bool mute, links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) : declare(declare), mute(mute) {
        this->expr = parser.parseExpr(tokens);
        this->logicalLine = this->expr->logicalLine;
    }
} // namespace ast