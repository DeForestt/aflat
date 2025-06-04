#ifndef AFLAT_LSP_SERVER_HPP
#define AFLAT_LSP_SERVER_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>

#ifndef LSP_KEYWORDS_PATH
#define LSP_KEYWORDS_PATH "lsp/keywords.json"
#endif

class LspServer {
public:
    explicit LspServer(const std::string &keywordsPath = LSP_KEYWORDS_PATH);
    void run();
    nlohmann::json process(const nlohmann::json &request);

private:
    void loadKeywords(const std::string &path);
    std::vector<std::string> keywords;
    std::unordered_map<std::string, std::string> documents;
    std::unordered_map<std::string, std::vector<std::string>> symbols;
};

#endif // AFLAT_LSP_SERVER_HPP
