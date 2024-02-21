#include "Parser/AST/Statements/Struct.hpp"

#include "Parser/Parser.hpp"

namespace ast {
Struct::Struct(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->logicalLine = tokens.peek()->lineCount;
  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto ident = *dynamic_cast<lex::LObj *>(tokens.pop());
    this->ident.ident = ident.meta;
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " struct needs Ident");
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto op = *dynamic_cast<lex::OpSym *>(tokens.pop());
    if (op.Sym != '{')
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Unopened UDeffType");
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Unopened UDeffType");
  this->statement = parser.parseStmt(tokens);
  parser.addType(this->ident.ident, asmc::Hard, asmc::QWord);
}
}  // namespace ast