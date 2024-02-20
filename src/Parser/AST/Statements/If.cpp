#include "Parser/AST/Statements/If.hpp"
namespace ast
{
    /*
     * @brief This will parse an if statement
     * @param tokens The tokens to parse\
     * @param parser The parser to use
     */
    If::If(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
      this->elseIf = nullptr;
      this->elseStatement = nullptr;
      this->expr = parser.parseExpr(tokens);
      this->logicalLine = this->expr->logicalLine;
      auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym != nullptr) {
        if (sym->Sym == '{') {
          tokens.pop();
          this->statement = parser.parseStmt(tokens);
        } else
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Unopened If");
      } else if (dynamic_cast<lex::LObj *>(tokens.peek())) {
        this->statement = parser.parseStmt(tokens, true);
      } else
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unopened If");
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        auto elseObj = *dynamic_cast<lex::LObj *>(tokens.peek());
        if (elseObj.meta == "else") {
          tokens.pop();
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            sym = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (sym->Sym == '{') {
              this->elseStatement = parser.parseStmt(tokens);
            } else
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Unopened Else");
          } else if (dynamic_cast<lex::LObj *>(tokens.peek())) {
            this->elseStatement = parser.parseStmt(tokens, true);
          } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unopened Else");
        } else
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Unclosed if");
      }
    }
} // namespace ast
