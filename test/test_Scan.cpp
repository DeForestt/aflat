#include "Scan/Scan.hpp"
#include "catch.hpp"

TEST_CASE("Scanner scans identifiers and ints", "[scanner]") {
  auto input = std::istringstream("foo 123");
  aflat::scan::Scanner scanner(input, 1);
  auto token1 = scanner.next();
  REQUIRE(token1.has_value());
  REQUIRE(aflat::scan::token::isIdentifier(token1.value()));
  REQUIRE(*aflat::scan::token::asIdentifier(token1.value()) == "foo");
  REQUIRE(token1.value().range.start.line == 1);
  REQUIRE(token1.value().range.start.column == 1);
  REQUIRE(token1.value().range.end.line == 1);
  REQUIRE(token1.value().range.end.column == 3);

  auto token2 = scanner.next();
  REQUIRE(token2.has_value());
  REQUIRE(aflat::scan::token::isInteger(token2.value()));
  REQUIRE(*aflat::scan::token::asInteger(token2.value()) == 123);
  REQUIRE(token2.value().range.start.line == 1);
  REQUIRE(token2.value().range.start.column == 5);
  REQUIRE(token2.value().range.end.line == 1);
  REQUIRE(token2.value().range.end.column == 7);
}

TEST_CASE("Scanner scans keywords", "[scanner]") {
  auto input = std::istringstream("if else while");
  aflat::scan::Scanner scanner(input, 1);
  auto token1 = scanner.next();
  REQUIRE(token1.has_value());
  REQUIRE(aflat::scan::token::isKeyword(token1.value()));
  REQUIRE(*aflat::scan::token::asKeyword(token1.value()) ==
          aflat::scan::token::Keyword::Type::If);
  REQUIRE(token1.value().range.start.line == 1);
  REQUIRE(token1.value().range.start.column == 1);
  REQUIRE(token1.value().range.end.line == 1);
  REQUIRE(token1.value().range.end.column == 2);

  auto token2 = scanner.next();
  REQUIRE(token2.has_value());
  REQUIRE(aflat::scan::token::isKeyword(token2.value()));
  REQUIRE(*aflat::scan::token::asKeyword(token2.value()) ==
          aflat::scan::token::Keyword::Type::Else);
  REQUIRE(token2.value().range.start.line == 1);
  REQUIRE(token2.value().range.start.column == 4);
  REQUIRE(token2.value().range.end.line == 1);
  REQUIRE(token2.value().range.end.column == 7);

  auto token3 = scanner.next();
  REQUIRE(token3.has_value());
  REQUIRE(aflat::scan::token::isKeyword(token3.value()));
  REQUIRE(*aflat::scan::token::asKeyword(token3.value()) ==
          aflat::scan::token::Keyword::Type::While);
  REQUIRE(token3.value().range.start.line == 1);
  REQUIRE(token3.value().range.start.column == 9);
  REQUIRE(token3.value().range.end.line == 1);
  REQUIRE(token3.value().range.end.column == 13);

  auto token4 = scanner.next();
  REQUIRE(token4.has_value());
  REQUIRE(aflat::scan::token::isEof(token4.value()));
  REQUIRE(token4.value().range.start.line == 1);
  REQUIRE(token4.value().range.start.column == 13);
  REQUIRE(token4.value().range.end.line == 1);
  REQUIRE(token4.value().range.end.column == 13);
}

TEST_CASE("Scanner scans full program", "[scanner]") {
  auto input = std::istringstream(R"(
        fn main() {
            let x = 10;
            let y = 20;
            let z = x + y;
            return z;
        }
    )");

  aflat::scan::Scanner scanner(input, 1);
  std::vector<aflat::scan::token::Token> tokens;
  while (true) {
    auto token = scanner.next();
    REQUIRE(token.has_value());
    tokens.push_back(token.value());
    if (aflat::scan::token::isEof(token.value())) {
      break;
    }
  }
}
