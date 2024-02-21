#include "Parser/AST/Statements/Class.hpp"
#include "Parser/Parser.hpp"
#include "Parser/AST.hpp"

namespace ast
{
    Class::Class(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser, bool safe, bool dynamic) {
        this->logicalLine = tokens.peek()->lineCount;
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
            auto ident = *dynamic_cast<lex::LObj *>(tokens.pop());
            this->ident.ident = ident.meta;
        } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " class needs Ident");

        // check for the word signs
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
            auto sig = *dynamic_cast<lex::LObj *>(tokens.pop());
            if (sig.meta == "signs") {
            auto *ident = dynamic_cast<lex::LObj *>(tokens.pop());
            if (ident != nullptr) {
                if (parser.typeList[ident->meta] == nullptr)
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) +
                    " Type " + ident->meta + " not found");
                this->base = ident->meta;
            } else
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) +
                    " signs needs Ident");
            }
        } else {
            this->base = "";
        };

        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            auto op = *dynamic_cast<lex::OpSym *>(tokens.pop());
            if (op.Sym != '{')
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unopened UDeffType");
        } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unopened UDeffType");

        // check if there is a contract
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
            auto t = ast::Type();
            t.size = asmc::QWord;
            t.typeName = this->ident.ident;
            // Check if the class is in the typeList
            if (parser.typeList[this->ident.ident] != nullptr)
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) + " Class " +
                this->ident.ident + " already exists");
            parser.typeList << t;

            auto contract = *dynamic_cast<lex::LObj *>(tokens.peek());
            if (contract.meta == "contract") {
            tokens.pop();
            auto sy = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (sy == nullptr)
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) +
                    " Unopened Contract");
            if (sy->Sym != '{')
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) +
                    " Unopened Contract");
            this->contract = parser.parseStmt(tokens);
            } else
            this->contract = nullptr;
        } else
            this->contract = nullptr;
        this->safeType = safe;
        this->dynamic = dynamic;
        this->statement = parser.parseStmt(tokens);
    };
} // namespace ast
