#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "LSP.hpp"
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

TEST_CASE("LSP survives deterministic incomplete parser constructs",
          "[lsp][parser][incomplete]") {
  const std::vector<std::string> incompleteSources = {
      "fn main() -> int { if true {",
      "fn main() -> int { if true { return 0; } else {",
      "fn main() -> int { while true {",
      "fn main() -> int { for let i = 0; i < 10;",
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
