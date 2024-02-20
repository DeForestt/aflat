#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include <string>

bool ast::Type::compare(ast::Type t, std::string name) {
  if (t.typeName == name) return true;
  return false;
}

ast::Return::Return(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
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