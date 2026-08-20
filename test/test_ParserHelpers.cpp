#include <string>

#include "Exceptions.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

namespace {

links::LinkedList<lex::Token *> scanForParser(const std::string &source,
                                              int line = 1) {
  lex::Lexer lexer;
  auto tokens = lexer.Scan(source, line);
  tokens.invert();
  return tokens;
}

std::string requireDoubleAmpersandError(parse::Parser &parser,
                                        links::LinkedList<lex::Token *> &tokens,
                                        const std::string &context = "") {
  try {
    parser.requireDoubleAmpersand(tokens, context);
  } catch (const err::Exception &error) {
    return error.errorMsg;
  }
  return "";
}

} // namespace

TEST_CASE("parser helper consumes a double ampersand", "[parser][helper]") {
  auto tokens = scanForParser("&&value");
  parse::Parser parser;

  parser.requireDoubleAmpersand(tokens);

  auto *value = dynamic_cast<lex::LObj *>(tokens.peek());
  REQUIRE(value != nullptr);
  CHECK(value->meta == "value");
}

TEST_CASE("optional parser helper consumes a double ampersand",
          "[parser][helper]") {
  auto tokens = scanForParser("&&value");
  parse::Parser parser;

  CHECK(parser.consumeDoubleAmpersandIfPresent(tokens));

  auto *value = dynamic_cast<lex::LObj *>(tokens.peek());
  REQUIRE(value != nullptr);
  CHECK(value->meta == "value");
}

TEST_CASE("optional parser helper leaves a non-ampersand token untouched",
          "[parser][helper]") {
  auto tokens = scanForParser("value");
  parse::Parser parser;
  auto *before = tokens.peek();

  CHECK_FALSE(parser.consumeDoubleAmpersandIfPresent(tokens));

  CHECK(tokens.peek() == before);
  auto *value = dynamic_cast<lex::LObj *>(tokens.peek());
  REQUIRE(value != nullptr);
  CHECK(value->meta == "value");
}

TEST_CASE("optional parser helper diagnoses a lone ampersand",
          "[parser][helper][diagnostic]") {
  auto tokens = scanForParser("&value", 5);
  parse::Parser parser;

  std::string error;
  try {
    (void)parser.consumeDoubleAmpersandIfPresent(tokens);
  } catch (const err::Exception &exception) {
    error = exception.errorMsg;
  }

  CHECK(error.find("Line: 5") != std::string::npos);
  CHECK(error.find("Expected a second `&` to complete `&&`") !=
        std::string::npos);
  auto *first = dynamic_cast<lex::OpSym *>(tokens.peek());
  REQUIRE(first != nullptr);
  CHECK(first->Sym == '&');
}

TEST_CASE("parser helper diagnoses a missing first ampersand",
          "[parser][helper][diagnostic]") {
  auto tokens = scanForParser("value", 7);
  parse::Parser parser;

  const auto error = requireDoubleAmpersandError(
      parser, tokens, "while parsing an owning parameter");

  CHECK(error.find("Line: 7") != std::string::npos);
  CHECK(error.find("Expected `&&` while parsing an owning parameter") !=
        std::string::npos);
  CHECK(error.find("got `value`") != std::string::npos);
}

TEST_CASE("parser helper diagnoses a missing second ampersand",
          "[parser][helper][diagnostic]") {
  auto tokens = scanForParser("&value", 9);
  parse::Parser parser;

  const auto error = requireDoubleAmpersandError(parser, tokens);

  CHECK(error.find("Line: 9") != std::string::npos);
  CHECK(error.find("Expected a second `&` to complete `&&`") !=
        std::string::npos);
  CHECK(error.find("got `value`") != std::string::npos);
  auto *first = dynamic_cast<lex::OpSym *>(tokens.peek());
  REQUIRE(first != nullptr);
  CHECK(first->Sym == '&');
}

TEST_CASE("parser helper diagnoses end of input",
          "[parser][helper][diagnostic]") {
  links::LinkedList<lex::Token *> tokens;
  parse::Parser parser;

  const auto error = requireDoubleAmpersandError(parser, tokens);

  CHECK(error.find("Expected `&&`") != std::string::npos);
  CHECK(error.find("got end of input") != std::string::npos);
}
