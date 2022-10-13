#pragma once
#include <string>
#include <vector>

namespace cfg
{

enum Mutibility {
    Promiscuous = 0,
    Strict = 1,
    Safe = 2
};

struct Config {
    std::string aflatVersion;
    std::vector<std::string> modules;
    std::vector<std::string> cFiles;
    std::string testFile;
    std::string entryPoint;
    std::string outPutFile = "./bin/a.out";
    Mutibility mutibility = Mutibility::Promiscuous;
    bool debug = false;
    bool compatibility = false;
};

Config getConfig(std::string content);

} // namespace Config