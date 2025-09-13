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

TEST_CASE("Scanner scans symbols", "[scanner]") {
  auto input = std::istringstream("+-*/==!=<=>==>");
  aflat::scan::Scanner scanner(input, 1);

  std::vector<aflat::scan::token::Symbol::Type> expected_symbols = {
      aflat::scan::token::Symbol::Type::Plus,         // +
      aflat::scan::token::Symbol::Type::Minus,        // -
      aflat::scan::token::Symbol::Type::Asterisk,     // *
      aflat::scan::token::Symbol::Type::Slash,        // /
      aflat::scan::token::Symbol::Type::DoubleEqual,  // ==
      aflat::scan::token::Symbol::Type::NotEqual,     // !=
      aflat::scan::token::Symbol::Type::LessEqual,    // <=
      aflat::scan::token::Symbol::Type::GreaterEqual, // >=
      aflat::scan::token::Symbol::Type::FatArrow      // =>
  };

  for (const auto &expected_symbol : expected_symbols) {
    auto token = scanner.next();
    REQUIRE(token.has_value());
    REQUIRE(aflat::scan::token::isSymbol(token.value()));
    REQUIRE(*aflat::scan::token::asSymbol(token.value()) == expected_symbol);
  }

  auto eof_token = scanner.next();
  REQUIRE(eof_token.has_value());
  REQUIRE(aflat::scan::token::isEof(eof_token.value()));
}

TEST_CASE("Scanner scans strings", "[scanner]") {
  auto input = std::istringstream(R"("hello" `template string`)");
  aflat::scan::Scanner scanner(input, 1);

  auto token1 = scanner.next();
  REQUIRE(token1.has_value());
  REQUIRE(aflat::scan::token::isString(token1.value()));
  REQUIRE(*aflat::scan::token::asString(token1.value()) == "hello");
  REQUIRE(token1.value().range.start.line == 1);
  REQUIRE(token1.value().range.start.column == 1);
  REQUIRE(token1.value().range.end.line == 1);
  REQUIRE(token1.value().range.end.column == 7);

  auto token2 = scanner.next();
  REQUIRE(token2.has_value());
  REQUIRE(aflat::scan::token::isTemplate(token2.value()));
  REQUIRE(*aflat::scan::token::asTemplate(token2.value()) == "template string");
  REQUIRE(token2.value().range.start.line == 1);
  REQUIRE(token2.value().range.start.column == 9);
  REQUIRE(token2.value().range.end.line == 1);
  REQUIRE(token2.value().range.end.column == 26);

  auto token3 = scanner.next();
  REQUIRE(token3.has_value());
  REQUIRE(aflat::scan::token::isEof(token3.value()));
  REQUIRE(token3.value().range.start.line == 1);
  REQUIRE(token3.value().range.start.column == 26);
  REQUIRE(token3.value().range.end.line == 1);
  REQUIRE(token3.value().range.end.column == 26);
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
    if (aflat::scan::token::isEof(token.value()))
      break;
  }

  // fn main() { let x = 10; let y = 20; let z = x + y; return z; }
  // Expect: 27 including EOF
  REQUIRE(tokens.size() == 27);

  using namespace aflat::scan::token;

  // 0: fn
  REQUIRE(isKeyword(tokens[0]));
  REQUIRE(*asKeyword(tokens[0]) == Keyword::Type::Fn);

  // 1: main
  REQUIRE(isIdentifier(tokens[1]));
  REQUIRE(*asIdentifier(tokens[1]) == "main");

  // 2: (
  REQUIRE(isSymbol(tokens[2]));
  REQUIRE(*asSymbol(tokens[2]) == Symbol::Type::LeftParen);

  // 3: )
  REQUIRE(isSymbol(tokens[3]));
  REQUIRE(*asSymbol(tokens[3]) == Symbol::Type::RightParen);

  // 4: {
  REQUIRE(isSymbol(tokens[4]));
  REQUIRE(*asSymbol(tokens[4]) == Symbol::Type::LeftBrace);

  // 5: let
  REQUIRE(isKeyword(tokens[5]));
  REQUIRE(*asKeyword(tokens[5]) == Keyword::Type::Let);

  // 6: x
  REQUIRE(isIdentifier(tokens[6]));
  REQUIRE(*asIdentifier(tokens[6]) == "x");

  // 7: =
  REQUIRE(isSymbol(tokens[7]));
  REQUIRE(*asSymbol(tokens[7]) == Symbol::Type::Equal);

  // 8: 10
  REQUIRE(isInteger(tokens[8]));
  REQUIRE(*asInteger(tokens[8]) == 10);

  // 9: ;
  REQUIRE(isSymbol(tokens[9]));
  REQUIRE(*asSymbol(tokens[9]) == Symbol::Type::Semicolon);

  // 10: let
  REQUIRE(isKeyword(tokens[10]));
  REQUIRE(*asKeyword(tokens[10]) == Keyword::Type::Let);

  // 11: y
  REQUIRE(isIdentifier(tokens[11]));
  REQUIRE(*asIdentifier(tokens[11]) == "y");

  // 12: =
  REQUIRE(isSymbol(tokens[12]));
  REQUIRE(*asSymbol(tokens[12]) == Symbol::Type::Equal);

  // 13: 20
  REQUIRE(isInteger(tokens[13]));
  REQUIRE(*asInteger(tokens[13]) == 20);

  // 14: ;
  REQUIRE(isSymbol(tokens[14]));
  REQUIRE(*asSymbol(tokens[14]) == Symbol::Type::Semicolon);

  // 15: let
  REQUIRE(isKeyword(tokens[15]));
  REQUIRE(*asKeyword(tokens[15]) == Keyword::Type::Let);

  // 16: z
  REQUIRE(isIdentifier(tokens[16]));
  REQUIRE(*asIdentifier(tokens[16]) == "z");

  // 17: =
  REQUIRE(isSymbol(tokens[17]));
  REQUIRE(*asSymbol(tokens[17]) == Symbol::Type::Equal);

  // 18: x
  REQUIRE(isIdentifier(tokens[18]));
  REQUIRE(*asIdentifier(tokens[18]) == "x");

  // 19: +
  REQUIRE(isSymbol(tokens[19]));
  REQUIRE(*asSymbol(tokens[19]) == Symbol::Type::Plus);

  // 20: y
  REQUIRE(isIdentifier(tokens[20]));
  REQUIRE(*asIdentifier(tokens[20]) == "y");

  // 21: ;
  REQUIRE(isSymbol(tokens[21]));
  REQUIRE(*asSymbol(tokens[21]) == Symbol::Type::Semicolon);

  // 22: return
  REQUIRE(isKeyword(tokens[22]));
  REQUIRE(*asKeyword(tokens[22]) == Keyword::Type::Return);

  // 23: z
  REQUIRE(isIdentifier(tokens[23]));
  REQUIRE(*asIdentifier(tokens[23]) == "z");

  // 24: ;
  REQUIRE(isSymbol(tokens[24]));
  REQUIRE(*asSymbol(tokens[24]) == Symbol::Type::Semicolon);

  // 25: }
  REQUIRE(isSymbol(tokens[25]));
  REQUIRE(*asSymbol(tokens[25]) == Symbol::Type::RightBrace);

  // 26: EOF
  REQUIRE(isEof(tokens[26]));
}
