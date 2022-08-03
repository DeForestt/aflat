#ifndef PARSE
#define PARSE

#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "Parser/AST.hpp"
#include "Scanner.hpp"
#include <string>
#include <vector>

namespace parse {
class Parser {
public:
  ast::Statment Output;
  ast::Statment *parseStmt(links::LinkedList<lex::Token *> &tokens,
                           bool singleStmt = false);
  Parser(int mutability = 0);

private:
  int mutability;
  links::SLinkedList<ast::Type, std::string> typeList;
  ast::Expr *parseExpr(links::LinkedList<lex::Token *> &tokens);
  ast::Statment *parseArgs(links::LinkedList<lex::Token *> &tokens, char delimn,
                           char close, std::vector<ast::Type> &types,
                           int &requiered);
  ast::ConditionalExpr *parseCondition(links::LinkedList<lex::Token *> &tokens);
};
}; // namespace parse

#endif
