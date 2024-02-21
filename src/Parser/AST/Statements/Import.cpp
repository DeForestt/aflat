#include "Parser/AST/Statements/Import.hpp"
#include "Parser.hpp"

namespace ast
{
    Import::Import(links::LinkedList<lex::Token *> tokens, parse::Parser &parser)
    {
        this->logicalLine = tokens.peek()->lineCount;
        auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (sym != nullptr) {
            this->classes = false;
            if (sym->Sym == '{') {
            do {
                tokens.pop();
                auto importObj = dynamic_cast<lex::LObj *>(tokens.pop());
                if (importObj != nullptr) {
                this->imports.push_back(importObj->meta);
                } else {
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) +
                    " Expected Ident");
                }
            } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                    dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '}') {
                tokens.pop();
            } else {
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) +
                    " Expected }");
            }
            } else if (sym->Sym == '*') {
            tokens.pop();
            this->imports.push_back("*");
            } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unexpected Token");
        } else {
            bool first = true;
            this->classes = true;
            do {
            if (!first)
                tokens.pop();
            else
                first = false;

            auto nt = dynamic_cast<lex::LObj *>(tokens.peek());
            if (nt != nullptr) {
                this->imports.push_back(nt->meta);
                auto t = ast::Type();
                t.size = asmc::QWord;
                t.typeName = nt->meta;
                t.opType = asmc::Hard;
                parser.typeList << t;
                tokens.pop();
            } else {
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) +
                    " Expected Ident");
            }
            } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                    dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
        }

        // check from from keyword
        lex::LObj *from = dynamic_cast<lex::LObj *>(tokens.pop());
        if (from == nullptr || from->meta != "from") {
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected from");
        };

        ast::StringLiteral *str =
            dynamic_cast<ast::StringLiteral *>(parser.parseExpr(tokens));
        if (str != nullptr) {
            this->path = str->val;
        } else {
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected StringLiteral");
        }

        // check for under keyword
        auto under = dynamic_cast<lex::LObj *>(tokens.peek());
        if (under != nullptr) {
            if (under->meta == "under") {
            tokens.pop();
            auto ident = dynamic_cast<lex::LObj *>(tokens.peek());
            if (ident != nullptr) {
                this->nameSpace = ident->meta;
                tokens.pop();
            } else {
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) +
                    " Expected Ident");
            }
            }
        } else {
            std::string id = this->path.substr(this->path.find_last_of('/') + 1,
                                            this->path.find_last_of('.'));
            this->nameSpace = id;
        }
    }
} // namespace ast
