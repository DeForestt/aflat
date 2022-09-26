#include "Configs.hpp"
#include "Utils.hpp"
#include <sstream>
#include <algorithm>

cfg::Config cfg::getConfig(std::string content) {
    cfg::Config config;
    std::stringstream ss(content);
    std::string line;

    while (std::getline(ss, line)){
        line = utils::remove_char(line, '\t');

        auto copy = line.substr(1);
        copy = utils::remove_char(copy, ' ');

        auto lowerCaseCopy = copy;
        std::transform(lowerCaseCopy.begin(), lowerCaseCopy.end(),
                   lowerCaseCopy.begin(), ::tolower);
        
        if (line[0] == ';')
            continue;
        
            // if the line is a dependency, build it and add it to the linker
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
        } else if (line.substr(0, 3) == "set") {
            auto setting = line.substr(4);
            if (setting == "debug") {
                config.debug = true;
            } else if (setting == "compatibility") {
                config.compatibility = true;
            } else if (setting.substr(0, 3) == "mut") {
                auto mut = setting.substr(4);
                if (mut == "promiscuous") {
                    config.mutibility = cfg::Mutibility::Promiscuous;
                } else if (mut == "strict") {
                    config.mutibility = cfg::Mutibility::Strict;
                } else if (mut == "safe") {
                    config.mutibility = cfg::Mutibility::Safe;
                }
            }
        }
    }

    return config;
};