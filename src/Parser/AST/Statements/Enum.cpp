#include "Parser/AST/Statements/Enum.hpp"

namespace ast
{
    /**
     * @brief Construct a new Enum object
     * 
     * @param tokens 
     * @param parser 
     */
    Enum::Enum(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
        this->logicalLine = tokens.peek()->lineCount;
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
            auto ident = *dynamic_cast<lex::LObj *>(tokens.pop());
            this->Ident = ident.meta;
        } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " enum needs Ident");

        lex::OpSym *op = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (!op || op->Sym != '{')
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unopened Enum");
            
            tokens.pop();

        while (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
            auto ident = *dynamic_cast<lex::LObj *>(tokens.pop());
            this->values.push_back(ident.meta);
            op = dynamic_cast<lex::OpSym *>(tokens.peek());
            if (op && op->Sym == ',')
            tokens.pop();
        };

        op = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (!op || op->Sym != '}')
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unclosed Enum");
        tokens.pop();
        parser.typeList << ast::Type(this->Ident, asmc::DWord);
    }
} // namespace ast
