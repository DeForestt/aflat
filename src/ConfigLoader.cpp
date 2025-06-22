#include <filesystem>
#include <fstream>
#include <iterator>

#include "Configs.hpp"

namespace cfg {

Config loadConfig(const std::string &cfgFile) {
  std::ifstream ifs(cfgFile);
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

  cfg::Config config = cfg::getConfig(content);

  std::vector<std::string> files;
  for (const auto &entry : std::filesystem::directory_iterator(".")) {
    if (entry.path().string().find(".aflat.cfg") != std::string::npos) {
      files.push_back(entry.path().string());
    }
  }

  for (const auto &file : files) {
    std::string id = file.substr(0, file.find(".aflat.cfg"));
    if (id.rfind("./", 0) == 0) id = id.substr(2);
    std::ifstream mifs(file);
    std::string subContent((std::istreambuf_iterator<char>(mifs)),
                           (std::istreambuf_iterator<char>()));
    cfg::Config sub = cfg::getConfig(subContent);
    for (auto &mod : sub.modules) {
      config.modules.push_back(id + "/" + mod);
    }
  }

  return config;
}

}  // namespace cfg
