#include "Parser/AST/Statements/Return.hpp"

#include "Parser/Parser.hpp"

namespace ast {
/*
 * @brief This will parse a return statement
 * @param tokens The tokens to parse
 * @param pa
 */
Return::Return(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  if (!(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
        dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ';')) {
    this->expr = parser.parseExpr(tokens);
    this->logicalLine = this->expr->logicalLine;
  } else {
    auto nu = new ast::Var();
    nu->Ident = "**void_type**";
    nu->logicalLine = tokens.peek()->lineCount;
    this->expr = nu;
  };
}
}  // namespace ast