#include "Scanner.hpp"
#include "catch.hpp"

TEST_CASE("Lexer scans identifiers and ints", "[scanner]") {
  lex::Lexer l;
  auto tokens = l.Scan("foo 123");
  REQUIRE(tokens.size() >= 3);  // includes trailing semicolon token
  auto *first = dynamic_cast<lex::LObj *>(tokens.get(2));
  auto *second = dynamic_cast<lex::INT *>(tokens.get(1));
  REQUIRE(first != nullptr);
  REQUIRE(first->meta == "foo");
  REQUIRE(first->columnStart == 0);
  REQUIRE(first->columnEnd == 2);
  REQUIRE(second != nullptr);
  REQUIRE(second->value == "123");
  REQUIRE(second->columnStart == 4);
  REQUIRE(second->columnEnd == 6);
}
