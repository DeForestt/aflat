#ifndef AFLAT_LSP_SERVER_HPP
#define AFLAT_LSP_SERVER_HPP

#include <string>
#include <map>
#include <nlohmann/json.hpp>

class LspServer {
public:
    void run();
    nlohmann::json process(const nlohmann::json &request);
    nlohmann::json validate(const std::string &text);
private:
    std::map<std::string, std::string> documents;
};

#endif // AFLAT_LSP_SERVER_HPP
