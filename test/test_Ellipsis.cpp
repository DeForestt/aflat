#include "Parser/Parser.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

TEST_CASE("Parser handles ellipsis as a no-op statement", "[parser]") {
  lex::Lexer l;
  auto tokenPtrs = l.Scan("...;");
  tokenPtrs.invert();
  auto tokens = lex::toRawList(tokenPtrs);
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *ellipsis = dynamic_cast<ast::Ellipsis *>(seq->Statement1);
  REQUIRE(ellipsis != nullptr);
}
