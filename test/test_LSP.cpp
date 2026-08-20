#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "LSP.hpp"
#include "catch.hpp"

namespace {

std::string frame(const std::string &message) {
  return "Content-Length: " + std::to_string(message.size()) + "\r\n\r\n" +
         message;
}

std::string runLanguageServer(const std::string &input) {
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

size_t countOccurrences(const std::string &text, const std::string &needle) {
  size_t count = 0;
  size_t position = 0;
  while ((position = text.find(needle, position)) != std::string::npos) {
    ++count;
    position += needle.size();
  }
  return count;
}

} // namespace

TEST_CASE("LSP semantic tokens support unions", "[lsp][union]") {
  const std::string uri = "file:///tmp/lsp-union.af";
  const auto open =
      R"({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{"uri":")" +
      uri +
      R"(","languageId":"aflat","version":1,"text":"union Choice { A(int), B };"}}})";
  const auto semantic =
      R"({"jsonrpc":"2.0","id":2,"method":"textDocument/semanticTokens/full","params":{"textDocument":{"uri":")" +
      uri + R"("}}})";
  const auto exit = R"({"jsonrpc":"2.0","method":"exit","params":{}})";

  const auto output =
      runLanguageServer(frame(open) + frame(semantic) + frame(exit));

  REQUIRE(output.find("\"id\":2") != std::string::npos);
  REQUIRE(output.find("\"data\":[") != std::string::npos);
  REQUIRE(output.find("\"diagnostics\":[]") != std::string::npos);
}

TEST_CASE("LSP publishes type-check diagnostics", "[lsp][diagnostics]") {
  const std::string uri = "file:///tmp/lsp-type-error.af";
  const auto open =
      R"({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{"uri":")" +
      uri +
      R"(","languageId":"aflat","version":1,"text":"fn main() -> int { return 0; };"}}})";
  const auto change =
      R"({"jsonrpc":"2.0","method":"textDocument/didChange","params":{"textDocument":{"uri":")" +
      uri +
      R"(","version":2},"contentChanges":[{"text":"fn main() -> int { missing(); return 0; };"}]}})";
  const auto exit = R"({"jsonrpc":"2.0","method":"exit","params":{}})";

  const auto output =
      runLanguageServer(frame(open) + frame(change) + frame(exit));

  REQUIRE(output.find("Cannot Find Function: missing") != std::string::npos);
  REQUIRE(output.find("\033[1;31m") == std::string::npos);
}

TEST_CASE("LSP tolerates incomplete statements while editing",
          "[lsp][diagnostics][incomplete]") {
  const std::string uri = "file:///tmp/lsp-incomplete.af";
  const std::string exit = R"({"jsonrpc":"2.0","method":"exit","params":{}})";

  const auto runIncompleteDocument = [&](const std::string &text) {
    const auto open =
        R"({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{"uri":")" +
        uri + R"(","languageId":"aflat","version":1,"text":")" + text +
        R"("}}})";
    const auto semantic =
        R"({"jsonrpc":"2.0","id":40,"method":"textDocument/semanticTokens/full","params":{"textDocument":{"uri":")" +
        uri + R"("}}})";
    return runLanguageServer(frame(open) + frame(semantic) + frame(exit));
  };

  SECTION("class header without a body") {
    const auto output = runIncompleteDocument("class Ident");
    REQUIRE(output.find("\"id\":40") != std::string::npos);
    REQUIRE(output.find("Unopened UDeffType") != std::string::npos);
  }

  SECTION("assignment without a value") {
    const auto output = runIncompleteDocument("value =");
    REQUIRE(output.find("\"id\":40") != std::string::npos);
    REQUIRE(output.find("Expected expression") != std::string::npos);
  }

  SECTION("declaration assignment without a value") {
    const auto output = runIncompleteDocument("let value =");
    REQUIRE(output.find("\"id\":40") != std::string::npos);
    REQUIRE(output.find("Expected expression") != std::string::npos);
  }

  SECTION("assignment with an unfinished parenthesized value") {
    const auto output = runIncompleteDocument("value = (1");
    REQUIRE(output.find("\"id\":40") != std::string::npos);
    REQUIRE(output.find("Need an ) to end parenthesis") != std::string::npos);
  }

  SECTION("each prefix of an exported function declaration") {
    const std::vector<std::string> prefixes = {
        "export",
        "export fn",
        "export fn Long",
        "export fn Long(",
        "export fn Long()",
        "export fn Long() -",
        "export fn Long() ->",
        "export fn Long() -> DBT",
        "export fn Long() -> DBT {",
        "export fn Long() -> DBT { return",
    };

    for (const auto &prefix : prefixes) {
      CAPTURE(prefix);
      const auto output = runIncompleteDocument(prefix);
      REQUIRE(output.find("\"id\":40") != std::string::npos);
      REQUIRE(output.find("textDocument/publishDiagnostics") !=
              std::string::npos);
    }
  }

  SECTION("each prefix of a foreach loop") {
    const std::vector<std::string> prefixes = {
        "foreach",
        "foreach record",
        "foreach record in",
        "foreach record in records",
        "foreach record in records.",
        "foreach record in records.scan(",
        "foreach record in records.scan() {",
        "foreach record in records.scan() { let row =",
        "foreach record in records.scan() { let row = record.",
    };

    for (const auto &prefix : prefixes) {
      CAPTURE(prefix);
      const auto output = runIncompleteDocument("fn inspect() -> int { " +
                                                prefix + " return 0; };");
      REQUIRE(output.find("textDocument/publishDiagnostics") !=
              std::string::npos);
    }
  }
}

TEST_CASE("LSP signature help includes function and method types",
          "[lsp][signature]") {
  const std::string uri = "file:///tmp/lsp-signatures.af";
  const std::string text =
      "class Calculator { fn sum(int left, int right) -> int { return left + "
      "right; }; }; fn add(int left, int right) -> int { return left + right; "
      "}; fn main() -> int { add(1, 2); return calc.sum(1, 2); };";
  const auto open =
      R"({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{"uri":")" +
      uri + R"(","languageId":"aflat","version":1,"text":")" + text + R"("}}})";
  const auto functionCursor = text.find("add(1, 2)") + 6;
  const auto methodCursor = text.find("calc.sum(1, 2)") + 11;
  const auto functionHelp =
      R"({"jsonrpc":"2.0","id":20,"method":"textDocument/signatureHelp","params":{"textDocument":{"uri":")" +
      uri + R"("},"position":{"line":0,"character":)" +
      std::to_string(functionCursor) + "}}}";
  const auto methodHelp =
      R"({"jsonrpc":"2.0","id":21,"method":"textDocument/signatureHelp","params":{"textDocument":{"uri":")" +
      uri + R"("},"position":{"line":0,"character":)" +
      std::to_string(methodCursor) + "}}}";
  const auto exit = R"({"jsonrpc":"2.0","method":"exit","params":{}})";

  const auto output = runLanguageServer(frame(open) + frame(functionHelp) +
                                        frame(methodHelp) + frame(exit));

  REQUIRE(output.find("fn add(int left, int right) -> int") !=
          std::string::npos);
  REQUIRE(output.find("fn Calculator.sum(int left, int right) -> int") !=
          std::string::npos);
  REQUIRE(output.find("Type: `int`") != std::string::npos);
  REQUIRE(output.find("\"activeParameter\":1") != std::string::npos);
}

TEST_CASE("LSP provides public member completion and inferred let hints",
          "[lsp][completion][inlay]") {
  const std::string uri = "file:///tmp/lsp-members.af";
  const std::string text =
      "class Calculator { public int value; private int secret; public fn "
      "sum(int left, int right) -> int { return left + right; }; private fn "
      "hidden() -> int { return 0; }; }; fn main() -> int { let calc = new "
      "Calculator(); calc.sum(1, 2); return 0; };";
  const auto open =
      R"({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{"uri":")" +
      uri + R"(","languageId":"aflat","version":1,"text":")" + text + R"("}}})";
  const auto completionCursor = text.find("calc.sum") + 5;
  const auto completion =
      R"({"jsonrpc":"2.0","id":30,"method":"textDocument/completion","params":{"textDocument":{"uri":")" +
      uri + R"("},"position":{"line":0,"character":)" +
      std::to_string(completionCursor) + "}}}";
  const auto hints =
      R"({"jsonrpc":"2.0","id":31,"method":"textDocument/inlayHint","params":{"textDocument":{"uri":")" +
      uri +
      R"("},"range":{"start":{"line":0,"character":0},"end":{"line":0,"character":999}}}})";
  const auto exit = R"({"jsonrpc":"2.0","method":"exit","params":{}})";

  const auto output = runLanguageServer(frame(open) + frame(completion) +
                                        frame(hints) + frame(exit));

  REQUIRE(output.find("\"label\":\"value\"") != std::string::npos);
  REQUIRE(output.find("\"label\":\"sum\"") != std::string::npos);
  REQUIRE(output.find("\"label\":\"secret\"") == std::string::npos);
  REQUIRE(output.find("\"label\":\"hidden\"") == std::string::npos);
  REQUIRE(output.find("\"label\":\"left\"") == std::string::npos);
  REQUIRE(output.find("\"label\":\": Calculator\"") != std::string::npos);
  REQUIRE(output.find("\"position\":{\"character\":" +
                      std::to_string(text.find("calc") + 4) + ",\"line\":0}") !=
          std::string::npos);
}

TEST_CASE("LSP provides inferred type hints for match bindings",
          "[lsp][match][inlay]") {
  const std::string uri = "file:///tmp/lsp-match-bindings.af";
  const std::string text = "union Choice { Number(int), Empty };\n"
                           "fn inspect(Choice choice) -> int {\n"
                           "  match choice {\n"
                           "    Number(value) => return value,\n"
                           "    Empty => return 0\n"
                           "  };\n"
                           "};";
  const auto open =
      R"({"jsonrpc":"2.0","method":"textDocument/didOpen","params":{"textDocument":{"uri":")" +
      uri + R"(","languageId":"aflat","version":1,"text":")" + text + R"("}}})";
  const auto hints =
      R"({"jsonrpc":"2.0","id":32,"method":"textDocument/inlayHint","params":{"textDocument":{"uri":")" +
      uri +
      R"("},"range":{"start":{"line":0,"character":0},"end":{"line":6,"character":1}}}})";
  const auto hover =
      R"({"jsonrpc":"2.0","id":33,"method":"textDocument/hover","params":{"textDocument":{"uri":")" +
      uri + R"("},"position":{"line":3,"character":12}}})";
  const auto exit = R"({"jsonrpc":"2.0","method":"exit","params":{}})";

  const auto output = runLanguageServer(frame(open) + frame(hints) +
                                        frame(hover) + frame(exit));

  INFO(output);
  REQUIRE(output.find("\"label\":\": int\"") != std::string::npos);
  REQUIRE(countOccurrences(output, "\"label\":\": int\"") == 1);
  REQUIRE(output.find("\"position\":{\"character\":16,\"line\":3}") !=
          std::string::npos);
  REQUIRE(output.find("value\\nint") != std::string::npos);
}
