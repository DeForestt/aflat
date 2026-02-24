#ifndef PARSE
#define PARSE

#include <string>
#include <unordered_map>
#include <vector>

#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"
#include "Scan/Scan.hpp"
#include "Scanner.hpp"

namespace parse {

static std::unordered_map<std::string, int> PRIMITIVE_TYPES = {
    {"int", 4},  {"short", 2}, {"char", 1}, {"long", 8},    {"adr", 8},
    {"byte", 1}, {"float", 4}, {"bool", 1}, {"generic", 8}, {"number", 8}};

struct Annotation {
  std::string name;
  std::vector<std::string> args;
};

class Parser {
public:
  ast::Statement Output;

  Parser(int mutability = 0);
  Parser(aflat::scan::Scanner &scanner, int mutability = 0);

  ast::Statement *parseStmt(links::LinkedList<lex::Token *> &tokens,
                            bool singleStmt = false);
  ast::Statement *parseStmt(bool singleStmt = false);
  ast::Expr *parseExpr(links::LinkedList<lex::Token *> &tokens);

  ast::Statement *parseArgs(links::LinkedList<lex::Token *> &tokens,
                            char delimn, char close,
                            std::vector<ast::Type> &types, int &requiered,
                            std::vector<bool> &mutability,
                            std::vector<int> &optConvertionIndices,
                            std::vector<bool> &readOnly, bool forEach);

  ast::Statement *parseArgs(links::LinkedList<lex::Token *> &tokens,
                            char delimn, char close,
                            std::vector<ast::Type> &types, int &requiered,
                            std::vector<bool> &mutability,
                            std::vector<int> &optConvertionIndices,
                            std::vector<bool> &readOnly);
  void addType(std::string name, asmc::OpType opType, asmc::Size size,
               bool unique = false);
  void addType(std::string name, asmc::OpType opType, asmc::Size size,
               bool isGeneric, bool unique = false);
  int mutability;
  links::SLinkedList<ast::Type, std::string> typeList;
  ast::Type parseFPointerType(links::LinkedList<lex::Token *> &tokens,
                              const std::string typeName);
  ast::ConditionalExpr *parseCondition(links::LinkedList<lex::Token *> &tokens);
  std::vector<std::string>
  parseTemplateTypeList(links::LinkedList<lex::Token *> &tokens, int lineCount);
  ast::When parseWhenClause(links::LinkedList<lex::Token *> &tokens,
                            int lineCount);
  links::LinkedList<ast::Expr *>
  parseCallArgsList(links::LinkedList<lex::Token *> &tokens);

private:
  aflat::scan::Scanner *scanner;
};
}; // namespace parse

#endif
