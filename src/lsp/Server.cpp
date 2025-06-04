#include "lsp/Server.hpp"
#include <iostream>
#include <fstream>
#include <regex>
#include <cctype>
#include <vector>
#include <unordered_set>
#include <sstream>
#include "Scanner.hpp"

using json = nlohmann::json;

LspServer::LspServer(const std::string &keywordsPath) {
    loadKeywords(keywordsPath);
}

void LspServer::loadKeywords(const std::string &path) {
    std::ifstream in(path);
    if (in.good()) {
        json arr;
        in >> arr;
        if (arr.is_array()) {
            keywords.clear();
            for (const auto &item : arr) {
                keywords.push_back(item.get<std::string>());
            }
        }
    }
    if (keywords.empty()) {
        keywords = {"fun", "let", "if", "else", "for"};
    }
}

static std::vector<std::string> extractSymbols(const std::string &text) {
    std::vector<std::string> vars;
    std::regex re(R"(\b([A-Za-z_][A-Za-z0-9_]*)\b\s*=)");
    for (std::sregex_iterator it(text.begin(), text.end(), re), end; it != end; ++it) {
        vars.push_back((*it)[1].str());
    }
    return vars;
}

static json makeSemanticTokens(const std::string &text,
                               const std::vector<std::string> &keywords) {
    json data = json::array();
    lex::Lexer lexer;
    auto tokens = lexer.Scan(text);

    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    std::vector<size_t> positions(lines.size(), 0);
    int prevLine = 0;
    int prevChar = 0;

    for (int i = 0; i < tokens.size(); ++i) {
        auto *tok = tokens.get(i);
        auto *obj = dynamic_cast<lex::LObj *>(tok);
        if (!obj)
            continue;
        int lineIdx = tok->lineCount - 1;
        if (lineIdx < 0 || lineIdx >= static_cast<int>(lines.size()))
            continue;
        size_t pos = lines[lineIdx].find(obj->meta, positions[lineIdx]);
        if (pos == std::string::npos)
            continue;
        positions[lineIdx] = pos + obj->meta.size();
        int type = std::find(keywords.begin(), keywords.end(), obj->meta) != keywords.end() ? 0 : 1;
        int deltaLine = lineIdx - prevLine;
        int deltaStart = (deltaLine == 0) ? static_cast<int>(pos) - prevChar : static_cast<int>(pos);
        data.push_back(deltaLine);
        data.push_back(deltaStart);
        data.push_back(obj->meta.size());
        data.push_back(type);
        data.push_back(0);
        prevLine = lineIdx;
        prevChar = pos;
    }

    return data;
}

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

        if (!response.is_null() && request.contains("id")) {
            response["id"] = request["id"];
            response["jsonrpc"] = "2.0";
            sendMessage(response);
        }

        if (request.value("method", "") == "shutdown") {
            break;
        }
    }
}

json LspServer::process(const json &request) {
    json result;
    result["jsonrpc"] = "2.0";
    std::string method = request.value("method", "");

    if (method == "initialize") {
        result["result"] = {
            {"capabilities", {
                {"hoverProvider", true},
                {"completionProvider", json::object()},
                {"semanticTokensProvider", {
                    {"legend", {
                        {"tokenTypes", {"keyword", "variable"}},
                        {"tokenModifiers", json::array()}
                    }},
                    {"full", true}
                }}
            }},
            {"serverInfo", {{"name", "aflat-lsp"}, {"version", "0.1"}}}
        };
    } else if (method == "initialized") {
        result["result"] = nullptr;
    } else if (method == "textDocument/didOpen") {
        auto params = request["params"];
        std::string uri = params["textDocument"]["uri"].get<std::string>();
        std::string text = params["textDocument"]["text"].get<std::string>();
        documents[uri] = text;
        symbols[uri] = extractSymbols(text);
        result["result"] = nullptr;
    } else if (method == "textDocument/didChange") {
        auto params = request["params"];
        std::string uri = params["textDocument"]["uri"].get<std::string>();
        auto changes = params["contentChanges"];
        if (!changes.empty()) {
            documents[uri] = changes[0]["text"].get<std::string>();
            symbols[uri] = extractSymbols(documents[uri]);
        }
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
        for (const auto &kw : keywords) {
            items.push_back({{"label", kw}});
        }
        auto params = request["params"];
        if (params.contains("textDocument")) {
            std::string uri = params["textDocument"]["uri"].get<std::string>();
            for (const auto &sym : symbols[uri]) {
                items.push_back({{"label", sym}});
            }
        }
        result["result"] = items;
    } else if (method == "textDocument/semanticTokens/full") {
        auto params = request["params"];
        std::string uri = params["textDocument"]["uri"].get<std::string>();
        json tokenResult;
        if (documents.count(uri)) {
            tokenResult["data"] = makeSemanticTokens(documents[uri], keywords);
        } else {
            tokenResult["data"] = json::array();
        }
        result["result"] = tokenResult;
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
