#ifndef PARSE
#define PARSE

#include <string>
#include <vector>

#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"
#include "Scanner.hpp"

namespace parse {
struct Annotation {
  std::string name;
  std::vector<std::string> args;
};

class Parser {
 public:
  ast::Statement Output;
  ast::Statement *parseStmt(links::LinkedList<lex::Token *> &tokens,
                            bool singleStmt = false);
  Parser(int mutability = 0);
  ast::Expr *parseExpr(links::LinkedList<lex::Token *> &tokens);

  ast::Statement *parseArgs(links::LinkedList<lex::Token *> &tokens,
                            char delimn, char close,
                            std::vector<ast::Type> &types, int &requiered,
                            std::vector<bool> &mutability,
                            std::vector<int> &optConvertionIndices,
                            bool forEach);

  ast::Statement *parseArgs(links::LinkedList<lex::Token *> &tokens,
                            char delimn, char close,
                            std::vector<ast::Type> &types, int &requiered,
                            std::vector<bool> &mutability,
                            std::vector<int> &optConvertionIndices);
  void addType(std::string name, asmc::OpType opType, asmc::Size size);
  void addType(std::string name, asmc::OpType opType, asmc::Size size,
               bool isGeneric);
  int mutability;
  links::SLinkedList<ast::Type, std::string> typeList;
  ast::Type parseFPointerType(links::LinkedList<lex::Token *> &tokens,
                              const std::string typeName);
  ast::ConditionalExpr *parseCondition(links::LinkedList<lex::Token *> &tokens);
  std::vector<std::string> parseTemplateTypeList(
      links::LinkedList<lex::Token *> &tokens, int lineCount);
  links::LinkedList<ast::Expr *> parseCallArgsList(
      links::LinkedList<lex::Token *> &tokens);
};
};  // namespace parse

#endif
