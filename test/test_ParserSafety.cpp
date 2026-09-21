#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Exceptions.hpp"
#include "LSP.hpp"
#include "Parser/AST/Statements/For.hpp"
#include "catch.hpp"

namespace {

std::string frameParserSafetyMessage(const std::string &message) {
  return "Content-Length: " + std::to_string(message.size()) + "\r\n\r\n" +
         message;
}

std::string runParserSafetyLanguageServer(const std::string &input) {
  std::istringstream request(input);
  std::ostringstream response;
  auto *oldIn = std::cin.rdbuf(request.rdbuf());
  auto *oldOut = std::cout.rdbuf(response.rdbuf());
  std::cin.clear();
  std::cout.clear();

  startLanguageServer();

  std::cin.rdbuf(oldIn);
  std::cout.rdbuf(oldOut);
  std::cin.clear();
  std::cout.clear();
  return response.str();
}

std::string jsonEscape(const std::string &text) {
  std::string escaped;
  escaped.reserve(text.size());
  for (const char c : text) {
    switch (c) {
    case '\\':
      escaped += "\\\\";
      break;
    case '"':
      escaped += "\\\"";
      break;
    case '\n':
      escaped += "\\n";
      break;
    case '\r':
      escaped += "\\r";
      break;
    case '\t':
      escaped += "\\t";
      break;
    default:
      escaped += c;
      break;
    }
  }
  return escaped;
}

} // namespace

TEST_CASE("for loops reject missing bodies", "[parser][incomplete]") {
  for (const std::string source :
       {"for let i = 0; i < 10;", "for let i = 0; i < 10; i++",
        "for let i = 0; i < 10; i++;"}) {
    CAPTURE(source);
    ast::StatementAllocationScope statements;
    ast::TypeAllocationScope types;
    lex::TokenAllocationScope tokenAllocations;
    lex::Lexer lexer;
    auto tokens = lexer.Scan(source, 1);
    tokens.invert();
    parse::Parser parser;
    CHECK_THROWS_AS(parser.parseStmt(tokens, true), err::Exception);
  }
}

TEST_CASE("for loops preserve braced and single statement bodies",
          "[parser][for]") {
  for (const std::string body :
       {"{ return 0; }", "; { return 0; }", "return 0;", "; return 0;"}) {
    CAPTURE(body);
    ast::StatementAllocationScope statements;
    ast::TypeAllocationScope types;
    lex::TokenAllocationScope tokenAllocations;
    lex::Lexer lexer;
    auto tokens = lexer.Scan("for let i = 0; i < 10; i++ " + body, 1);
    tokens.invert();
    parse::Parser parser;
    const auto *loop = dynamic_cast<ast::For *>(parser.parseStmt(tokens, true));
    REQUIRE(loop != nullptr);
    auto *statement = loop->Run;
    if (const auto *sequence = dynamic_cast<ast::Sequence *>(statement))
      statement = sequence->Statement1;
    CHECK(dynamic_cast<ast::Return *>(statement) != nullptr);
  }
}

TEST_CASE("LSP survives deterministic incomplete parser constructs",
          "[lsp][parser][incomplete]") {
  const std::vector<std::string> incompleteSources = {
      "fn main() -> int { if true {",
      "fn main() -> int { if true { return 0; } else {",
      "fn main() -> int { while true {",
      "fn main() -> int { for let i = 0; i < 10;",
      "fn main() -> int { for let i = 0; i < 10; i++",
      "fn main() -> int { for let i = 0; i < 10; i++;",
      "fn main() -> int { foreach item in",
      "fn main() -> int { match value {",
      "fn main() -> int { match value { Some(item) => {",
      "fn main() -> int { match value { Some(",
      "fn main() -> int { let value =",
      "fn main() -> int { call(",
      "fn main() -> int { value.",
      "fn main() -> int { let value = (1",
      "fn main() -> int { let value = [1,",
      "fn main() -> int { let value = {1:",
      "fn main(",
      "class Thing {",
      "union Choice { Some(",
      "enum State { Ready,",
      "struct Pair {",
      "import {Thing,",
      "import Thing from",
      "types(T,",
      "when(",
      "delete",
      "return",
  };

  std::string input;
  for (std::size_t i = 0; i < incompleteSources.size(); ++i) {
    const auto uri = "file:///tmp/parser-safety-" + std::to_string(i) + ".af";
    const auto open =
        R"({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{"uri":")" +
        uri + R"(","languageId":"aflat","version":1,"text":")" +
        jsonEscape(incompleteSources[i]) + R"("}}})";
    input += frameParserSafetyMessage(open);
  }
  input += frameParserSafetyMessage(
      R"({"jsonrpc":"2.0","method":"exit","params":{}})");

  const auto output = runParserSafetyLanguageServer(input);

  for (std::size_t i = 0; i < incompleteSources.size(); ++i) {
    CAPTURE(i, incompleteSources[i]);
    CHECK(output.find("file:///tmp/parser-safety-" + std::to_string(i) +
                      ".af") != std::string::npos);
  }
}
