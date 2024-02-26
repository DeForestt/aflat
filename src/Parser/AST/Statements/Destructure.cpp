#include "Parser/AST/Statements/Destructure.hpp"

#include <iostream>

#include "Parser/Parser.hpp"

namespace ast {
Destructure::Destructure(links::LinkedList<lex::Token *> &tokens,
                         parse::Parser &parser) {
  auto identifier = dynamic_cast<lex::LObj *>(tokens.pop());
  if (!identifier)
    throw err::Exception("On line " + std::to_string(tokens.peek()->lineCount) +
                         " need at least one identifier for destructuring");

  while (identifier) {
    identifiers.push_back(identifier->meta);
    auto symbol = dynamic_cast<lex::OpSym *>(tokens.pop());
    if (!symbol)
      throw err::Exception(
          "On line " + std::to_string(tokens.peek()->lineCount) +
          " expected ',' or '}' after identifier in destructuring");
    if (symbol->Sym == '}') break;
    if (symbol->Sym != ',')
      throw err::Exception(
          "On line " + std::to_string(tokens.peek()->lineCount) +
          " expected ',' or '}' after identifier in destructuring");
    identifier = dynamic_cast<lex::LObj *>(tokens.pop());
    if (!identifier)
      throw err::Exception("On line " +
                           std::to_string(tokens.peek()->lineCount) +
                           " expected identifier after ',' in destructuring");
  }

  auto eq = dynamic_cast<lex::OpSym *>(tokens.pop());
  if (!eq || eq->Sym != '=')
    throw err::Exception("On line " + std::to_string(tokens.peek()->lineCount) +
                         " expected '=' after identifiers in destructuring");

  this->expr = parser.parseExpr(tokens);

  std::string identList = "";
  for (auto &ident : identifiers) identList += ident + ", ";

  std::cout << "created Destructure " << identList << " = " << expr->toString()
            << std::endl;
}
}  // namespace ast
