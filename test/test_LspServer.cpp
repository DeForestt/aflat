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

