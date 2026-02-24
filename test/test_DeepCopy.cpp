#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

TEST_CASE("deepCopy clones statements", "[deepcopy]") {
  lex::Lexer l;
  parse::Parser p;
  auto tokenPtrs = l.Scan("return 1;");
  tokenPtrs.invert();
  auto tokens = lex::toRawList(tokenPtrs);
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
