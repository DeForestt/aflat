#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace cfg {

enum Mutability { Promiscuous = 0, Strict = 1, Safe = 2 };

struct Config {
  std::string aflatVersion;
  std::vector<std::string> modules;
  std::vector<std::string> cFiles;
  std::unordered_map<std::string, std::string> dependencies;
  std::string testFile;
  std::string entryPoint;
  std::string outPutFile = "./bin/a.out";
  Mutability mutability = Mutability::Strict;
  bool debug = false;
  bool compatibility = false;
  bool asm_ = false;
};

Config getConfig(std::string content);
Config loadConfig(const std::string &cfgFile, bool updateDeps = false,
                  bool cleanDeps = false);

}  // namespace cfg