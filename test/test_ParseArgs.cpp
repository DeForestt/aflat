#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

TEST_CASE("parseArgs records immutable arguments", "[parser]") {
  lex::Lexer l;
  PreProcessor pp;
  auto code = pp.PreProcess("fn foo(immutable int& x) -> void {}", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  auto *stmt = p.parseStmt(tokens);
  auto *func = dynamic_cast<ast::Function *>(stmt);
  REQUIRE(func != nullptr);
  REQUIRE(func->readOnly.size() == 1);
  CHECK(func->readOnly[0] == true);
}
