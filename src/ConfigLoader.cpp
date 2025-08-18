#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

#include "Configs.hpp"

namespace cfg {

Config loadConfig(const std::string &cfgFile, bool updateDeps, bool cleanDeps) {
  std::ifstream ifs(cfgFile);
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));

  cfg::Config config = cfg::getConfig(content);

  for (const auto &dep : config.dependencies) {
    const auto &name = dep.first;
    const auto &url = dep.second;
    if (url.size() >= 4 && url.substr(url.size() - 4) == ".git") {
      auto cfgPath = "./" + name + ".aflat.cfg";
      auto srcPath = "./src/" + name;

      if (cleanDeps) {
        std::filesystem::remove(cfgPath);
        std::filesystem::remove_all(srcPath);
        continue;
      }

      bool existsCfg = std::filesystem::exists(cfgPath);
      bool existsSrc = std::filesystem::exists(srcPath);
      if (updateDeps || !existsCfg || !existsSrc) {
        std::filesystem::remove(cfgPath);
        std::filesystem::remove_all(srcPath);

        auto baseTmp = std::filesystem::path(".aflat/tmp/git");
        std::filesystem::create_directories(baseTmp);
        auto tmpDir =
            baseTmp / (name + "-" + std::to_string(std::time(nullptr)));

        std::string cmd = "git clone --depth 1 " + url + " " + tmpDir.string();
        if (system(cmd.c_str()) != 0) {
          std::cerr << "Failed to clone " << url << std::endl;
          std::filesystem::remove_all(tmpDir);
          continue;
        }

        auto srcDir = tmpDir / "src";
        if (!std::filesystem::exists(srcDir)) {
          std::cerr << "Dependency " << name << " missing src/ directory"
                    << std::endl;
          std::filesystem::remove_all(tmpDir);
          continue;
        }

        std::filesystem::create_directories(srcPath);
        std::filesystem::copy(
            srcDir, srcPath,
            std::filesystem::copy_options::recursive |
                std::filesystem::copy_options::overwrite_existing);

        auto repoCfg = tmpDir / "aflat.cfg";
        if (std::filesystem::exists(repoCfg)) {
          std::filesystem::copy_file(
              repoCfg, cfgPath,
              std::filesystem::copy_options::overwrite_existing);
        } else {
          std::cerr << "Warning: aflat.cfg missing in repo " << url
                    << std::endl;
        }

        std::filesystem::remove_all(tmpDir);
      }
    }
  }

  std::vector<std::string> files;
  for (const auto &entry : std::filesystem::directory_iterator(".")) {
    if (entry.path().string().find(".aflat.cfg") != std::string::npos) {
      files.push_back(entry.path().string());
    }
  }

  for (const auto &file : files) {
    std::string id = file.substr(0, file.find(".aflat.cfg"));
    if (id.rfind("./", 0) == 0)
      id = id.substr(2);
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

} // namespace cfg
