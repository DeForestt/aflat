#include "Parser/AST/Statements/For.hpp"

namespace ast {
For::For(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->declare = parser.parseStmt(tokens, true);
  this->logicalLine = this->declare->logicalLine;
  auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());

  if (sym == nullptr)
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Unterminated for loop initializer");
  if (sym->Sym != ';')
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         "unterminated for loop initializer");

  tokens.pop();

  this->expr = parser.parseExpr(tokens);

  sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym == nullptr)
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Unterminated for loop condition");
  if (sym->Sym != ';')
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         "unterminated for loop condition");
  tokens.pop();

  this->increment = parser.parseStmt(tokens, true);

  sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym != nullptr) {
    if (sym->Sym == ';') {
      tokens.pop();
    };
    sym = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (sym != nullptr) {
      if (sym->Sym == '{') {
        tokens.pop();
        this->Run = parser.parseStmt(tokens);
      } else
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unopened for loop body");
    }
  } else
    this->Run = parser.parseStmt(tokens, true);
}
}  // namespace ast
