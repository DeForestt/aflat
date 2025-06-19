#pragma once

#include <string>
#include <vector>

struct CommandLineOptions {
  bool debug = false;
  std::string outputFile = "out.s";
  std::string configFile = "aflat.cfg";
  std::string command;
  std::vector<std::string> args;
};

bool parseCommandLine(int argc, char **argv, CommandLineOptions &opts);
void printUsage(const char *prog);

