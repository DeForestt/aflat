#include "lsp/Server.hpp"
#include <iostream>
#include <cctype>
#include <unordered_set>

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

std::pair<int, int> indexToPos(const std::string &text, size_t idx) {
    int line = 0;
    int ch = 0;
    for (size_t i = 0; i < idx && i < text.size(); ++i) {
        if (text[i] == '\n') {
            line++; ch = 0;
        } else {
            ch++;
        }
    }
    return {line, ch};
}

std::pair<int, int> findWord(const std::string &text, const std::string &word) {
    size_t pos = text.find(word);
    while (pos != std::string::npos) {
        bool left = pos == 0 || !(std::isalnum(text[pos-1]) || text[pos-1]=='_');
        bool right = pos + word.size() >= text.size() || !(std::isalnum(text[pos+word.size()]) || text[pos+word.size()]=='_');
        if (left && right) {
            return indexToPos(text, pos);
        }
        pos = text.find(word, pos + word.size());
    }
    return {-1, -1};
}

int countLines(const std::string &text) {
    return std::count(text.begin(), text.end(), '\n');
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
            {"completionProvider", json::object()},
            {"definitionProvider", true},
            {"renameProvider", true}
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
        auto params = request["params"];
        std::unordered_set<std::string> tokens;
        for (const auto &entry : documents) {
            const std::string &txt = entry.second;
            std::string cur;
            for (char c : txt) {
                if (std::isalnum(c) || c == '_') cur += c; else { if (!cur.empty()) { tokens.insert(cur); cur.clear(); } }
            }
            if (!cur.empty()) tokens.insert(cur);
        }
        json items = json::array();
        for (const auto &tok : tokens) items.push_back({{"label", tok}});
        for (const auto &kw : {"fun", "let", "if", "else", "for"}) {
            items.push_back({{"label", kw}});
        }
        result["result"] = items;
    } else if (method == "textDocument/didChange") {
        auto params = request["params"];
        std::string uri = params["textDocument"]["uri"].get<std::string>();
        std::string text = params["contentChanges"][0]["text"].get<std::string>();
        documents[uri] = text;
        result["result"] = nullptr;
    } else if (method == "textDocument/definition") {
        auto params = request["params"];
        std::string uri = params["textDocument"]["uri"].get<std::string>();
        int line = params["position"]["line"].get<int>();
        int character = params["position"]["character"].get<int>();
        std::string word = getWordAt(documents[uri], line, character);
        std::string defUri;
        std::pair<int,int> pos{-1,-1};
        for (const auto &entry : documents) {
            auto p = findWord(entry.second, word);
            if (p.first >= 0) { pos = p; defUri = entry.first; break; }
        }
        if (pos.first >= 0) {
            json loc;
            loc["uri"] = defUri;
            loc["range"]["start"] = {{"line", pos.first}, {"character", pos.second}};
            loc["range"]["end"] = {{"line", pos.first}, {"character", pos.second + (int)word.size()}};
            result["result"] = json::array({loc});
        } else {
            result["result"] = json::array();
        }
    } else if (method == "textDocument/rename") {
        auto params = request["params"];
        std::string uri = params["textDocument"]["uri"].get<std::string>();
        int line = params["position"]["line"].get<int>();
        int character = params["position"]["character"].get<int>();
        std::string newName = params["newName"].get<std::string>();
        std::string word = getWordAt(documents[uri], line, character);
        if (!word.empty()) {
            json edit;
            for (auto &entry : documents) {
                std::string &text = entry.second;
                size_t pos = 0;
                bool changed = false;
                while ((pos = text.find(word, pos)) != std::string::npos) {
                    bool left = pos == 0 || !(std::isalnum(text[pos-1]) || text[pos-1]=='_');
                    bool right = pos + word.size() >= text.size() || !(std::isalnum(text[pos+word.size()]) || text[pos+word.size()]=='_');
                    if (left && right) {
                        text.replace(pos, word.size(), newName);
                        pos += newName.size();
                        changed = true;
                    } else {
                        pos += word.size();
                    }
                }
                if (changed) {
                    json change;
                    change["range"]["start"] = {{"line", 0}, {"character", 0}};
                    change["range"]["end"] = {{"line", countLines(text)}, {"character", 0}};
                    change["newText"] = text;
                    edit["changes"][entry.first] = json::array({change});
                }
            }
            result["result"] = edit;
        } else {
            result["result"] = json();
        }
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
