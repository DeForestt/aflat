#ifndef PARSE
#define PARSE

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"
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
  Parser(int mutability = 0);
  ~Parser();
  Parser(Parser &&) noexcept;
  Parser &operator=(Parser &&) noexcept;
  Parser(const Parser &) = delete;
  Parser &operator=(const Parser &) = delete;

  ast::Statement *parseStmt(links::LinkedList<lex::Token *> &tokens,
                            bool singleStmt = false);
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
  ast::Type parseFPointerType(links::LinkedList<lex::Token *> &tokens,
                              const std::string typeName);
  ast::ConditionalExpr *parseCondition(links::LinkedList<lex::Token *> &tokens);
  std::vector<std::string>
  parseTemplateTypeList(links::LinkedList<lex::Token *> &tokens, int lineCount);
  ast::When parseWhenClause(links::LinkedList<lex::Token *> &tokens,
                            int lineCount);
  links::LinkedList<ast::Expr *>
  parseCallArgsList(links::LinkedList<lex::Token *> &tokens);

  links::SLinkedList<ast::Type, std::string> &getTypeList();
  const links::SLinkedList<ast::Type, std::string> &getTypeList() const;
  ast::Statement &getOutput();
  const ast::Statement &getOutput() const;
  int getMutability() const;
  void setMutability(int value);

private:
  struct Impl;
  std::unique_ptr<Impl> impl;
};
}; // namespace parse

#endif
