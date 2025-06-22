#pragma once

#include <string>
#include <vector>

struct CommandLineOptions {
  bool debug = false;
  bool traceAlerts = false;
  bool quiet = false;
  std::string outputFile;  // set via -o when compiling single files
  std::string configFile = "aflat.cfg";
  std::string command;
  std::vector<std::string> args;
};

bool parseCommandLine(int argc, char **argv, CommandLineOptions &opts);
void printUsage(const char *prog);
