#include "Parser/AST/Statements/Assign.hpp"

#include "Parser/Parser.hpp"

namespace ast {
Assign::Assign(const std::string &ident,
               const links::LinkedList<ast::Expr *> &indices,
               links::LinkedList<std::string> modList,
               links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->logicalLine = tokens.peek()->lineCount;
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto s2 = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (s2->Sym == ':') {
      this->reference = true;
      tokens.pop();
    };
  };

  this->Ident = ident;
  this->modList = modList;
  this->expr = parser.parseExpr(tokens);
  this->indices = indices;
}
}  // namespace ast
