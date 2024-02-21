#include "Parser/AST/Statements/Function.hpp"

namespace ast
{
    Function::Function(const string &ident, const ScopeMod &scope, const Type &type, const Op op, const std::string &scopeName, links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) : scope(scope), type(type), op(op), scopeName(scopeName)
    {
        this->ident.ident = ident;
        this->args =
        parser.parseArgs(tokens, ',', ')', this->argTypes, this->req);
        
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            auto sym = *dynamic_cast<lex::OpSym *>(tokens.peek());

            // Check for a ':'
            if (sym.Sym == ':') {
            // make the next word a decorator.
            tokens.pop();
            auto *decor = dynamic_cast<lex::LObj *>(tokens.pop());
            if (decor == nullptr)
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) +
                    "Expected Identifier after ':'");
            this->decorator = decor->meta;

            if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount));
            sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
            if (sym.Sym == '.') {
                tokens.pop();
                auto *lob = dynamic_cast<lex::LObj *>(tokens.pop());
                if (lob == nullptr)
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) +
                    "Expected Identifier after '.'");
                this->decNSP = this->decorator;
                this->decorator = lob->meta;
                if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount));
                sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
            };
            if (sym.Sym == '(') {
                tokens.pop();
                if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                    dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ')') {
                    auto symp = dynamic_cast<lex::OpSym *>(tokens.pop());
                    if (symp->Sym != ')')
                    throw err::Exception("Expected closed parenthesis got " + symp->Sym);
                } else {
                    bool pop = false;
                    do {
                    if (pop)
                        tokens.pop();
                    this->decoratorArgs.push(parser.parseExpr(tokens));
                    pop = true;
                    } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                            dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
                    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
                    auto symp = dynamic_cast<lex::OpSym *>(tokens.pop());
                    if (symp->Sym != ')')
                        throw err::Exception("Expected closed parenthesis got " +
                                            symp->Sym);
                    }
                }
            };
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
                throw err::Exception(
                    "Line: " + std::to_string(tokens.peek()->lineCount) + "Expected a symbol");
                sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
            }

            if (sym.Sym == '{') {
                tokens.pop();
                this->statement = parser.parseStmt(tokens);
                this->logicalLine = tokens.peek()->lineCount;
            } else {
                this->statement = nullptr;
                this->logicalLine = tokens.peek()->lineCount;
            }
        } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Need terminating symbol or open symbol");
    }
} // namespace ast
