#include <cctype>

#include "CodeGenerator/Utils.hpp"
#include "Parser/AST.hpp"
#include "Utils.hpp"
#include "catch.hpp"

TEST_CASE("remove_char removes specific character", "[utils]") {
  REQUIRE(utils::remove_char("hello", 'l') == "heo");
  REQUIRE(utils::remove_char("", 'x') == "");
}

TEST_CASE("op_to_string returns correct representation", "[utils]") {
  using ast::Op;
  REQUIRE(utils::op_to_string(Op::Plus) == "+");
  REQUIRE(utils::op_to_string(Op::Minus) == "-");
  REQUIRE(utils::op_to_string(Op::Mul) == "*");
  REQUIRE(utils::op_to_string(Op::Div) == "/");
  REQUIRE(utils::op_to_string(Op::CompEqu) == "===");
}

TEST_CASE("sizeToInt returns byte counts", "[cgutils]") {
  REQUIRE(gen::utils::sizeToInt(asmc::Size::Byte) == 1);
  REQUIRE(gen::utils::sizeToInt(asmc::Size::Word) == 2);
  REQUIRE(gen::utils::sizeToInt(asmc::Size::DWord) == 4);
  REQUIRE(gen::utils::sizeToInt(asmc::Size::QWord) == 8);
  REQUIRE(gen::utils::sizeToInt(asmc::Size::AUTO) == 0);
}

TEST_CASE("searchSymbol compares symbol names", "[cgutils]") {
  gen::Symbol s;
  s.symbol = "foo";
  REQUIRE(gen::utils::searchSymbol(s, "foo"));
  REQUIRE_FALSE(gen::utils::searchSymbol(s, "bar"));
}

TEST_CASE("compareFunc matches function identifier", "[cgutils]") {
  ast::Function f;
  f.ident.ident = "main";
  REQUIRE(gen::utils::compareFunc(f, "main"));
  REQUIRE_FALSE(gen::utils::compareFunc(f, "other"));
}

TEST_CASE("format utility replaces braces", "[cgutils]") {
  REQUIRE(gen::utils::format("{} {}", "hello", "world") == "hello world");
  REQUIRE(gen::utils::format("{}+{}={}", 1, 2, 3) == "1+2=3");
}

TEST_CASE("generateUUID returns 16 digits", "[cgutils]") {
  auto id = gen::utils::generateUUID();
  REQUIRE(id.size() == 16);
  for (char c : id) {
    REQUIRE(std::isdigit(c));
  }
}
