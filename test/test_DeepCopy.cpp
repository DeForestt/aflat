#include "Parser/AST.hpp"
#include "Parser/AST/Expressions/List.hpp"
#include "Parser/AST/Statements/Match.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

TEST_CASE("deepCopy clones statements", "[deepcopy]") {
  lex::Lexer l;
  parse::Parser p;
  auto tokens = l.Scan("return 1;");
  tokens.invert();
  ast::Statement *stmt = p.parseStmt(tokens, true);
  auto *ret = dynamic_cast<ast::Return *>(stmt);
  REQUIRE(ret != nullptr);

  ast::Statement *copyStmt = ast::deepCopy(stmt);
  auto *retCopy = dynamic_cast<ast::Return *>(copyStmt);
  REQUIRE(retCopy != nullptr);
  REQUIRE(retCopy != ret);
  auto *origInt = dynamic_cast<ast::IntLiteral *>(ret->expr);
  auto *copyInt = dynamic_cast<ast::IntLiteral *>(retCopy->expr);
  REQUIRE(origInt != nullptr);
  REQUIRE(copyInt != nullptr);
  REQUIRE(copyInt->val == origInt->val);
  copyInt->val = 2;
  CHECK(origInt->val == 1);
}

TEST_CASE("deepCopy clones foreach statements", "[deepcopy]") {
  lex::Lexer l;
  parse::Parser p;
  auto tokens = l.Scan("foreach v in items { print(v); };");
  tokens.invert();
  ast::Statement *stmt = p.parseStmt(tokens, true);
  auto *foreachStmt = dynamic_cast<ast::ForEach *>(stmt);
  REQUIRE(foreachStmt != nullptr);

  ast::Statement *copyStmt = ast::deepCopy(stmt);
  auto *foreachCopy = dynamic_cast<ast::ForEach *>(copyStmt);
  REQUIRE(foreachCopy != nullptr);
  REQUIRE(foreachCopy != foreachStmt);
  REQUIRE(foreachCopy->binding_identifier == foreachStmt->binding_identifier);
  REQUIRE(foreachCopy->implementation != nullptr);
  REQUIRE(foreachCopy->implementation != foreachStmt->implementation);
  REQUIRE(foreachCopy->iterator != nullptr);
  REQUIRE(foreachCopy->iterator != foreachStmt->iterator);
}

TEST_CASE("deepCopy clones list literals in call extensions", "[deepcopy]") {
  lex::Lexer l;
  parse::Parser p;
  auto tokens = l.Scan("new Builder().with_items([1, 2]);");
  tokens.invert();
  ast::Expr *expr = p.parseExpr(tokens);
  auto *newExpr = dynamic_cast<ast::NewExpr *>(expr);
  REQUIRE(newExpr != nullptr);
  auto *extension = dynamic_cast<ast::CallExpr *>(newExpr->extention);
  REQUIRE(extension != nullptr);
  REQUIRE(extension->call->Args.count == 1);

  ast::Statement *copyStmt = ast::deepCopy(extension);
  auto *copy = dynamic_cast<ast::CallExpr *>(copyStmt);
  REQUIRE(copy != nullptr);
  REQUIRE(copy != extension);
  REQUIRE(copy->call != nullptr);
  REQUIRE(copy->call != extension->call);
  REQUIRE(copy->call->Args.count == 1);
  auto *copiedList = dynamic_cast<ast::List *>(copy->call->Args.get(0));
  REQUIRE(copiedList != nullptr);
  REQUIRE(copiedList != extension->call->Args.get(0));
  REQUIRE(copiedList->items.size() == 2);
}

TEST_CASE("replaceTypes descends into match case bodies", "[generics][match]") {
  auto *constructor = new ast::UnionConstructor(
      ast::Type("result", asmc::QWord), "Ok", new ast::Var(), true, {"T"});
  auto *returnStmt = new ast::Return();
  returnStmt->expr = constructor;

  ast::Match match;
  match.returns = ast::Type("result<T>", asmc::QWord);
  match.cases.emplace_back(ast::Match::Pattern("Some", "value"), returnStmt);

  match.replaceTypes({{"T", "JSON"}});

  CHECK(match.returns.typeName == "result<JSON>");
  REQUIRE(constructor->templateTypes.size() == 1);
  CHECK(constructor->templateTypes[0] == "JSON");
}
