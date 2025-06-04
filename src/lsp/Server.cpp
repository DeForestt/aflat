#include "lsp/Server.hpp"
#include <iostream>
#include <cctype>

using json = nlohmann::json;

namespace {
json readMessage() {
    std::string line;
    size_t length = 0;
    while (std::getline(std::cin, line)) {
        if (line.rfind("Content-Length:", 0) == 0) {
            length = std::stoul(line.substr(15));
        }
        if (line == "\r" || line.empty()) break;
    }
    std::string content(length, '\0');
    if (length > 0) {
        std::cin.read(content.data(), length);
    }
    return json::parse(content);
}

void sendMessage(const json &msg) {
    std::string content = msg.dump();
    std::cout << "Content-Length: " << content.size() << "\r\n\r\n";
    std::cout << content;
    std::cout.flush();
}

std::string getWordAt(const std::string &text, int line, int character) {
    size_t pos = 0;
    int currentLine = 0;
    while (currentLine < line && pos < text.size()) {
        if (text[pos] == '\n') currentLine++;
        pos++;
    }
    pos += character;
    if (pos >= text.size()) return "";

    size_t start = pos;
    while (start > 0 && (std::isalnum(text[start-1]) || text[start-1] == '_')) {
        start--;
    }
    size_t end = pos;
    while (end < text.size() && (std::isalnum(text[end]) || text[end] == '_')) {
        end++;
    }
    return text.substr(start, end - start);
}
}

void LspServer::run() {
    while (std::cin.good()) {
        auto request = readMessage();
        auto response = process(request);
        if (!response.is_null()) {
            response["id"] = request["id"];
            sendMessage(response);
        }
        if (request["method"] == "shutdown") {
            break;
        }
    }
}

json LspServer::process(const json &request) {
    json result;
    std::string method = request.value("method", "");

    if (method == "initialize") {
        result["result"]["capabilities"] = {
            {"hoverProvider", true},
            {"completionProvider", json::object()}
        };
    } else if (method == "textDocument/didOpen") {
        auto params = request["params"];
        std::string uri = params["textDocument"]["uri"].get<std::string>();
        std::string text = params["textDocument"]["text"].get<std::string>();
        documents[uri] = text;
        result["result"] = nullptr;
    } else if (method == "textDocument/hover") {
        auto params = request["params"];
        std::string uri = params["textDocument"]["uri"].get<std::string>();
        int line = params["position"]["line"].get<int>();
        int character = params["position"]["character"].get<int>();
        std::string word = getWordAt(documents[uri], line, character);
        json hover;
        hover["contents"] = word.empty() ? "" : word;
        result["result"] = hover;
    } else if (method == "textDocument/completion") {
        json items = json::array();
        for (const auto &kw : {"fun", "let", "if", "else", "for"}) {
            items.push_back({{"label", kw}});
        }
        result["result"] = items;
    } else if (method == "shutdown") {
        result["result"] = nullptr;
    } else if (method == "exit") {
        // no response
        return json();
    } else {
        // method not supported
        result["error"] = {
            {"code", -32601},
            {"message", "Method not found"}
        };
    }

    return result;
}
