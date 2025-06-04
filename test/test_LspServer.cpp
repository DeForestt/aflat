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
    REQUIRE(resp["result"]["capabilities"]["hoverProvider"].get<bool>());
    REQUIRE(resp["result"]["capabilities"].contains("completionProvider"));
    REQUIRE(resp["result"]["capabilities"]["definitionProvider"].get<bool>());
    REQUIRE(resp["result"]["capabilities"]["renameProvider"].get<bool>());
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
    REQUIRE(resp["result"]["contents"] == "int");
}

TEST_CASE("Completion lists identifiers", "[lsp]") {
    LspServer server;
    json openReq = {
        {"id", 1},
        {"method", "textDocument/didOpen"},
        {"params", {{"textDocument", {{"uri", "file:///test.af"}, {"text", "int x;"}}}}}
    };
    server.process(openReq);

    json req = {
        {"id", 2},
        {"method", "textDocument/completion"},
        {"params", {{"textDocument", {{"uri", "file:///test.af"}}}}}
    };
    json resp = server.process(req);
    REQUIRE(resp["result"].is_array());
    bool hasX = false;
    for (auto &item : resp["result"]) {
        if (item["label"] == "x") hasX = true;
    }
    REQUIRE(hasX);
}

TEST_CASE("Definition returns first occurrence", "[lsp]") {
    LspServer server;
    json openReq = {
        {"id", 1},
        {"method", "textDocument/didOpen"},
        {"params", {{"textDocument", {{"uri", "file:///test.af"}, {"text", "int x;\nx = 1;"}}}}}
    };
    server.process(openReq);

    json defReq = {
        {"id", 2},
        {"method", "textDocument/definition"},
        {"params", {{"textDocument", {{"uri", "file:///test.af"}}}, {"position", {{"line", 1}, {"character", 0}}}}}
    };
    json resp = server.process(defReq);
    REQUIRE(resp["result"].is_array());
    REQUIRE(resp["result"].size() == 1);
    REQUIRE(resp["result"][0]["range"]["start"]["line"].get<int>() == 0);
}

TEST_CASE("Rename replaces identifier", "[lsp]") {
    LspServer server;
    json openReq = {
        {"id", 1},
        {"method", "textDocument/didOpen"},
        {"params", {{"textDocument", {{"uri", "file:///test.af"}, {"text", "int x;\nx = 1;"}}}}}
    };
    server.process(openReq);

    json renameReq = {
        {"id", 2},
        {"method", "textDocument/rename"},
        {"params", {{"textDocument", {{"uri", "file:///test.af"}}}, {"position", {{"line", 0}, {"character", 4}}}, {"newName", "y"}}}
    };
    json resp = server.process(renameReq);
    std::string newText = resp["result"]["changes"]["file:///test.af"][0]["newText"].get<std::string>();
    REQUIRE(newText.find("y") != std::string::npos);
    REQUIRE(newText.find("x") == std::string::npos);
}

