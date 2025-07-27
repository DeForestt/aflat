#include "Parser/Parser.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

TEST_CASE("Parser parses when clauses", "[parser][when]") {
  lex::Lexer l;
  auto tokens =
      l.Scan("when (T is dynamic and T has toString) fn foo() -> int {};", 1);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *func = dynamic_cast<ast::Function *>(seq->Statement1);
  REQUIRE(func != nullptr);
  REQUIRE(func->when.has_value());
  REQUIRE(func->when->predicates.size() == 2);
  CHECK(func->when->predicates[0].typeName == "T");
  CHECK(func->when->predicates[0].op == ast::WhenOperator::IS);
  CHECK(func->when->predicates[0].ident == "dynamic");
  CHECK(func->when->predicates[0].negated == false);
  CHECK(func->when->predicates[1].typeName == "T");
  CHECK(func->when->predicates[1].op == ast::WhenOperator::HAS);
  CHECK(func->when->predicates[1].ident == "toString");
  CHECK(func->when->predicates[1].negated == false);
}
