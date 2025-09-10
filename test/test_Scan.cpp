#include "Scan/Scan.hpp"
#include "catch.hpp"

TEST_CASE("Scanner scans identifiers and ints", "[scanner]") {
  auto input = std::istringstream("foo 123");
  aflat::scan::Scanner scanner(input, 1);
  auto token1 = scanner.next();
  REQUIRE(token1.has_value());
  REQUIRE(aflat::scan::token::isIdentifier(token1.value()));
  REQUIRE(aflat::scan::token::asIdentifier(token1.value()) == "foo");
  REQUIRE(token1.value().range.start.line == 1);
  REQUIRE(token1.value().range.start.column == 1);
  REQUIRE(token1.value().range.end.line == 1);
  REQUIRE(token1.value().range.end.column == 3);

  auto token2 = scanner.next();
  REQUIRE(token2.has_value());
  REQUIRE(aflat::scan::token::isInteger(token2.value()));
  REQUIRE(aflat::scan::token::asInteger(token2.value()) == 123);
  REQUIRE(token2.value().range.start.line == 1);
  REQUIRE(token2.value().range.start.column == 5);
  REQUIRE(token2.value().range.end.line == 1);
  REQUIRE(token2.value().range.end.column == 7);
}
