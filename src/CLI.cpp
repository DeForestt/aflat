#include "CLI.hpp"

#include <getopt.h>

#include <iostream>

bool parseCommandLine(int argc, char **argv, CommandLineOptions &opts) {
  optind = 1; // reset getopt state for repeated calls
  static option longOptions[] = {{"help", no_argument, nullptr, 'h'},
                                 {"output", required_argument, nullptr, 'o'},
                                 {"debug", no_argument, nullptr, 'd'},
                                 {"quiet", no_argument, nullptr, 'q'},
                                 {"jobs", no_argument, nullptr, 'j'},
                                 {"trace-alerts", no_argument, nullptr, 't'},
                                 {"config", required_argument, nullptr, 'c'},
                                 {"name", required_argument, nullptr, 'n'},
                                 {"update-deps", no_argument, nullptr, 'U'},
                                 {"clean-deps", no_argument, nullptr, 'K'},
                                 {"clean-cache", no_argument, nullptr, 'C'},
                                 {"no-cache", no_argument, nullptr, 'N'},
                                 {"lib", no_argument, nullptr, 'L'},
                                 {nullptr, 0, nullptr, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "hdo:tc:qjUKn:LNC", longOptions,
                            nullptr)) != -1) {
    switch (opt) {
    case 'o':
      opts.outputFile = optarg;
      break;
    case 'd':
      opts.debug = true;
      break;
    case 'q':
      opts.quiet = true;
      break;
    case 'j':
      opts.concurrent = true;
      break;
    case 't':
      opts.traceAlerts = true;
      break;
    case 'c':
      opts.configFile = optarg;
      break;
    case 'n':
      opts.installName = optarg;
      break;
    case 'U':
      opts.updateDeps = true;
      break;
    case 'K':
      opts.cleanDeps = true;
      break;
    case 'L':
      opts.library = true;
      break;
    case 'C':
      opts.cleanCache = true;
      break;
    case 'N':
      opts.noCache = true;
      break;
    case 'h':
    case '?':
      printUsage(argv[0]);
      return false;
    }
  }

  if (optind >= argc) {
    printUsage(argv[0]);
    return false;
  }

  opts.command = argv[optind++];
  for (; optind < argc; ++optind) {
    opts.args.emplace_back(argv[optind]);
  }
  return true;
}

void printUsage(const char *prog) {
  std::cout << "Usage: " << prog << " [options] <command> [args]\n";
  std::cout
      << "Commands:\n"
      << "  make <name>         Create project template\n"
      << "  build               Build using aflat.cfg\n"
      << "  run                 Build and run using aflat.cfg\n"
      << "  test                Build and run tests\n"
      << "  add <module>        Add a header/src pair\n"
      << "  file <name>         Add a single source file\n"
      << "  module <name>       Add a folder-based module (mod.af)\n"
      << "  install <repo>      Add git dependency (use --name to set module)\n"
      << "  readme [dir] [name] Generate README scaffolding\n"
      << "  docs <lib|file>     Show classes and functions for a library/file\n"
      << "  update              Update AFlat\n"
      << "  clean               Remove ./bin directory\n"
      << "  <file> [out.s]      Compile single file\n\n"
      << "Options:\n"
      << "  -o, --output <file> Output file when compiling a single file\n"
      << "  -c, --config <file> Use alternative config file\n"
      << "  -n, --name <name>   Module name when installing\n"
      << "  -d, --debug         Enable debug information\n"
      << "  -q, --quiet         Suppress build progress output\n"
      << "  -t, --trace-alerts  Trace CodeGenerator alerts\n"
      << "  -U, --update-deps   Refresh git dependencies\n"
      << "  -K, --clean-deps    Remove all git dependencies\n"
      << "  -C, --clean-cache   Remove build cache\n"
      << "  -N, --no-cache      Disable cache for this run\n"
      << "  -j, --jobs         Enable concurrent module builds\n"
      << "  -L, --lib           Create a library project with make\n"
      << "  -h, --help          Display this help message\n";
}
