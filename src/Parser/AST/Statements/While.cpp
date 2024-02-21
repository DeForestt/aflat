#include "Parser/AST/Statements/While.hpp"
#include "Parser/Parser.hpp"

namespace ast
{
    /*
     * @brief This will parse a while statement
     * @param tokens The tokens to parse
     * @param parser The parser
     */
    While::While(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
      this->expr = parser.parseExpr(tokens);
      this->logicalLine = this->expr->logicalLine;
      auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym != nullptr) {
        if (sym->Sym == '{') {
          tokens.pop();
          this->stmt = parser.parseStmt(tokens);
        } else
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Unopened loop");
      } else
        this->stmt = parser.parseStmt(tokens, true);
    }
} // namespace ast