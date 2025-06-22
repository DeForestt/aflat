#include "Configs.hpp"

#include <algorithm>
#include <sstream>

#include "Utils.hpp"

cfg::Config cfg::getConfig(std::string content) {
  cfg::Config config;
  auto trim = [](const std::string &s) { return utils::trim(s); };

  std::stringstream detect(content);
  std::string line;
  bool ini = false;
  while (std::getline(detect, line)) {
    line = trim(line);
    if (line.empty() || line[0] == ';') continue;
    if (!line.empty() && line[0] == '[') ini = true;
    break;
  }

  std::stringstream ss(content);
  std::string section;
  if (ini) {
    while (std::getline(ss, line)) {
      line = trim(line);
      if (line.empty() || line[0] == ';') continue;
      if (line.front() == '[' && line.back() == ']') {
        section = line.substr(1, line.size() - 2);
        std::transform(section.begin(), section.end(), section.begin(),
                       ::tolower);
        continue;
      }
      auto eq = line.find('=');
      if (eq == std::string::npos) continue;
      auto key = trim(line.substr(0, eq));
      auto value = trim(line.substr(eq + 1));
      if (value.size() > 1 &&
          ((value.front() == '"' && value.back() == '"') ||
           (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
      }
      std::string lowerKey = key;
      std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(),
                     ::tolower);

      if (section == "build") {
        if (lowerKey == "output") {
          config.outPutFile = value;
        } else if (lowerKey == "debug") {
          std::transform(value.begin(), value.end(), value.begin(), ::tolower);
          config.debug = value == "true" || value == "1";
        } else if (lowerKey == "main") {
          config.entryPoint = value;
        } else if (lowerKey == "test") {
          config.testFile = value;
        } else if (lowerKey == "compatibility") {
          std::transform(value.begin(), value.end(), value.begin(), ::tolower);
          config.compatibility = value == "true" || value == "1";
        } else if (lowerKey == "asm") {
          std::transform(value.begin(), value.end(), value.begin(), ::tolower);
          config.asm_ = value == "true" || value == "1";
        } else if (lowerKey == "mutability") {
          std::transform(value.begin(), value.end(), value.begin(), ::tolower);
          if (value == "promiscuous")
            config.mutability = cfg::Mutability::Promiscuous;
          else if (value == "strict")
            config.mutability = cfg::Mutability::Strict;
          else if (value == "safe")
            config.mutability = cfg::Mutability::Safe;
        }
      } else if (section == "dependencies") {
        if (!value.empty()) {
          std::string mod = value;
          if (mod.rfind("./", 0) == 0) mod = mod.substr(2);
          if (mod.rfind("src/", 0) == 0) mod = mod.substr(4);
          if (mod.size() > 3 && mod.substr(mod.size() - 3) == ".af")
            mod = mod.substr(0, mod.size() - 3);
          config.modules.push_back(mod);
        }
      }
    }
  } else {
    while (std::getline(ss, line)) {
      line = utils::remove_char(line, '\t');

      auto copy = line.substr(1);
      copy = utils::remove_char(copy, ' ');

      auto lowerCaseCopy = copy;
      std::transform(lowerCaseCopy.begin(), lowerCaseCopy.end(),
                     lowerCaseCopy.begin(), ::tolower);

      if (line.empty()) continue;
      if (line[0] == ';') continue;

      if (line[0] == 'm') {
        std::string module = copy;
        config.modules.push_back(module);
      } else if (line[0] == 'e') {
        config.entryPoint = copy;
      } else if (line[0] == 't') {
        config.testFile = copy;
      } else if (line[0] == 'o') {
        config.outPutFile = copy;
      } else if (line[0] == 'c') {
        config.cFiles.push_back(copy);
      } else if (line[0] == 'v') {
        config.aflatVersion = copy;
      } else if (line.substr(0, 3) == "set") {
        auto setting = line.substr(4);
        if (setting == "debug") {
          config.debug = true;
        } else if (setting == "compatibility") {
          config.compatibility = true;
        } else if (setting == "asm") {
          config.asm_ = true;
        } else if (setting.substr(0, 3) == "mut") {
          auto mut = setting.substr(4);
          if (mut == "promiscuous") {
            config.mutability = cfg::Mutability::Promiscuous;
          } else if (mut == "strict") {
            config.mutability = cfg::Mutability::Strict;
          } else if (mut == "safe") {
            config.mutability = cfg::Mutability::Safe;
          }
        }
      }
    }
  }

  return config;
}
