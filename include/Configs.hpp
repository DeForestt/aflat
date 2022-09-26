#pragma once
#include <string>
#include <vector>

namespace cfg
{

enum Mutibility {
    Promiscuous,
    Strict,
    Safe
};

struct Config {
    std::string aflatVersion;
    std::vector<std::string> modules;
    std::vector<std::string> cFiles;
    std::string testFile;
    std::string entryPoint;
    std::string outPutFile;
    Mutibility mutibility;
    bool debug;
    bool compatibility;
};

Config getConfig(std::string content);

} // namespace Config