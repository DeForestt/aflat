#pragma once
#include <string>
#include <vector>

namespace cfg
{

enum Mutability {
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
    Mutability mutability = Mutability::Strict;
    bool debug = false;
    bool compatibility = false;
    bool asm_ = false;
};

Config getConfig(std::string content);

} // namespace Config