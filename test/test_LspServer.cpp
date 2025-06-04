#include "lsp/Server.hpp"
#include "catch.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TEST_CASE("Initialize capabilities", "[lsp]") {
    LspServer server;
    json req = {
        {"id", 1},
        {"method", "initialize"},
        {"params", json::object()}
    };
    json resp = server.process(req);
    REQUIRE(resp["jsonrpc"] == "2.0");
    REQUIRE(resp["result"]["capabilities"]["hoverProvider"].get<bool>());
    REQUIRE(resp["result"]["capabilities"].contains("completionProvider"));
    REQUIRE(resp["result"]["capabilities"].contains("semanticTokensProvider"));
}

TEST_CASE("Hover returns word under cursor", "[lsp]") {
    LspServer server;
    json openReq = {
        {"id", 1},
        {"method", "textDocument/didOpen"},
        {"params", {{"textDocument", {{"uri", "file:///test.af"}, {"text", "int x;"}}}}}
    };
    server.process(openReq);

    json hoverReq = {
        {"id", 2},
        {"method", "textDocument/hover"},
        {"params", {{"textDocument", {{"uri", "file:///test.af"}}}, {"position", {{"line", 0}, {"character", 1}}}}}
    };
    json resp = server.process(hoverReq);
    REQUIRE(resp["jsonrpc"] == "2.0");
    REQUIRE(resp["result"]["contents"] == "int");
}

TEST_CASE("Handles initialized notification", "[lsp]") {
    LspServer server;
    json initReq = {{"id", 1}, {"method", "initialized"}, {"params", json::object()}};
    json resp = server.process(initReq);
    REQUIRE(resp["jsonrpc"] == "2.0");
    REQUIRE(resp["result"].is_null());
}

TEST_CASE("Completion lists keywords", "[lsp]") {
    LspServer server;
    json req = {
        {"id", 1},
        {"method", "textDocument/completion"},
        {"params", json::object()}
    };
    json resp = server.process(req);
    REQUIRE(resp["jsonrpc"] == "2.0");
    REQUIRE(resp["result"].is_array());
    REQUIRE(resp["result"].size() >= 1);
    REQUIRE(resp["result"][0]["label"].get<std::string>() == "fun");
}

TEST_CASE("Completion includes symbols", "[lsp]") {
    LspServer server;
    json openReq = {{"id",1}, {"method","textDocument/didOpen"},
        {"params", {{"textDocument", {{"uri","file:///s.af"}, {"text","let foo=1;"}}}}}};
    server.process(openReq);

    json compReq = {{"id",2}, {"method","textDocument/completion"},
        {"params", {{"textDocument", {{"uri","file:///s.af"}}}}}};
    json resp = server.process(compReq);
    bool foundFoo = false;
    for (const auto &item : resp["result"]) {
        if (item["label"].get<std::string>() == "foo") foundFoo = true;
    }
    REQUIRE(foundFoo);
}

TEST_CASE("Semantic tokens returned", "[lsp]") {
    LspServer server;
    json openReq = {{"id",1}, {"method","textDocument/didOpen"},
        {"params", {{"textDocument", {{"uri","file:///tok.af"}, {"text","fun x(){let y=0;}"}}}}}};
    server.process(openReq);

    json semReq = {{"id",2}, {"method","textDocument/semanticTokens/full"},
        {"params", {{"textDocument", {{"uri","file:///tok.af"}}}}}};
    json resp = server.process(semReq);
    REQUIRE(resp["jsonrpc"] == "2.0");
    REQUIRE(resp["result"].contains("data"));
    REQUIRE(resp["result"]["data"].is_array());
    REQUIRE(!resp["result"]["data"].empty());
}

