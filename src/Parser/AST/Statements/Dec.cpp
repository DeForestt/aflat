#include "Parser/AST/Statements/Dec.hpp"

namespace ast {
    Dec::Dec(const std::string &ident, links::LinkedList<lex::Token *> &tokens) : ident(ident) {
        if (tokens.count < 1) {
            throw err::Exception("EOF while parsing increment statement");
        }
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr || dynamic_cast<lex::OpSym *>(tokens.peek())->Sym != '-') {
            throw err::Exception("Expected another '-' in the increment statment on line " + std::to_string(tokens.peek()->lineCount));
        }
        this->logicalLine = tokens.pop()->lineCount;
    }
} // namespace ast