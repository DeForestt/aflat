#include <unistd.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "ASM.hpp"
#include "CLI.hpp"
#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CompilerUtils.hpp"
#include "Configs.hpp"
#include "ErrorReporter.hpp"
#include "Exceptions.hpp"
#include "LinkedList.hpp"
#include "Parser/Lower.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"
#include "Scanner.hpp"
#include "Progress.hpp"

std::string preProcess(std::string input);
std::string getExePath();
void buildTemplate(std::string value);
void libTemplate(std::string value);
bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);
void ensureBinPath(const std::string &path, std::vector<std::string> &pathList);

bool compileCFile(const std::string &path, bool debug);
bool runConfig(cfg::Config &config, const std::string &libPath, char pmode);
bool runConfig(cfg::Config &config, const std::string &libPath);

static CompileProgress *gProgress = nullptr;
static bool gQuiet = false;

#ifndef AFLAT_TEST
int main(int argc, char *argv[]) {
  CommandLineOptions cli;
  if (!parseCommandLine(argc, argv, cli)) {
    return 1;
  }
  gQuiet = cli.quiet;
  gen::CodeGenerator::enableAlertTrace(cli.traceAlerts);

  std::string filename = getExePath();
  std::string exepath = filename.substr(0, filename.find_last_of("/"));
  std::string libPathA =
      exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std/";

  std::string value = cli.command;

  if (value == "make") {
    if (cli.args.empty() || cli.args.size() > 2) {
      printUsage(argv[0]);
      return 1;
    }
    std::string pName = cli.args[0];
    std::cout << "creating " << pName << '\n';
    if (cli.args.size() == 2 && cli.args[1] == "lib") {
      libTemplate(pName);
    } else {
      buildTemplate(pName);
    }
    return 0;
  }
  if (value == "build") {
    cfg::Config config = cfg::loadConfig(cli.configFile);
    config.debug = cli.debug;
    config.outPutFile = cli.outputFile;
    runConfig(config, libPathA, 'e');
    return 0;
  }
  if (value == "run") {
    cfg::Config config = cfg::loadConfig(cli.configFile);
    config.debug = cli.debug;
    config.outPutFile = cli.outputFile;
    if (runConfig(config, libPathA, 'e')) {
      auto of = config.outPutFile;
      if (config.outPutFile[0] != '.' && config.outPutFile[1] != '/') {
        of = "./" + config.outPutFile;
      }
      [[maybe_unused]] int rc = system(of.c_str());
    }
    return 0;
  }
  if (value == "test") {
    cfg::Config config = cfg::loadConfig(cli.configFile);
    config.debug = cli.debug;
    config.outPutFile = cli.outputFile;
    if (runConfig(config, libPathA, 't')) {
      [[maybe_unused]] int rc = system("./bin/a.test");
    }
    return 0;
  }
  if (value == "add" || value == "module" || value == "file") {
    if (cli.args.empty()) {
      printUsage(argv[0]);
      return 1;
    }
    std::string modualName = cli.args[0];
    std::string srcName = value == "module" ? "./src/" + modualName + "/mod.af"
                                            : "./src/" + modualName + ".af";
    std::string headerName = "./head/" + modualName + ".gs";

    if (value == "module") {
      std::filesystem::create_directories("./src/" + modualName);
    } else {
      std::string path = "";

      if (modualName.find("/") != std::string::npos) {
        path = modualName.substr(0, modualName.find_last_of("/"));
      }

      if (!path.empty() && !std::filesystem::exists("./src/" + path)) {
        std::filesystem::create_directories("./src/" + path);
      }

      if (value == "add" && !path.empty() &&
          !std::filesystem::exists("./head/" + path)) {
        std::filesystem::create_directories("./head/" + path);
      }
    }

    // create the files
    std::ofstream srcFile(srcName);

    if (value == "add") {
      std::ofstream headerFile(headerName);
      srcFile << ".root \"head\"\n";
      srcFile << ".needs \"" << modualName << ".gs\"\n";
      headerFile.close();
    }
    srcFile.close();

    std::ifstream cfgIn(cli.configFile);
    std::string cfgContent((std::istreambuf_iterator<char>(cfgIn)),
                           std::istreambuf_iterator<char>());
    cfgIn.close();

    const std::string depHeader = "[dependencies]";
    const std::string entry =
        modualName + (value == "module"
                          ? " = \"./src/" + modualName + "/mod.af\"\n"
                          : " = \"./src/" + modualName + ".af\"\n");

    if (cfgContent.find(depHeader) == std::string::npos) {
      if (!cfgContent.empty() && cfgContent.back() != '\n') cfgContent += '\n';
      cfgContent += depHeader + "\n" + entry;
    } else {
      if (!cfgContent.empty() && cfgContent.back() != '\n') cfgContent += '\n';
      cfgContent += entry;
    }

    std::ofstream cfgOut(cli.configFile, std::ios::trunc);
    cfgOut << cfgContent;
    cfgOut.close();
    return 0;
  }
  if (value == "update") {
    std::string update_command =
        "curl -s "
        "https://raw.githubusercontent.com/DeForestt/aflat/refs/heads/main/"
        "install.sh | bash";
    [[maybe_unused]] int rc = system(update_command.c_str());
    return 0;
  }
  if (value == "clean") {
    std::filesystem::remove_all("./bin");
    return 0;
  }
  if (value == "install") {
    if (cli.args.empty()) {
      printUsage(argv[0]);
      return 1;
    }
    std::string gitRepo = cli.args[0];
    // if repo starts with 'https://' remove it
    if (gitRepo.find("https://") != std::string::npos) {
      gitRepo = gitRepo.substr(8);
    }

    std::string install_command =
        "curl -s "
        "https://aflat-server.fly.dev/"
        "api/package/" +
        gitRepo + " | bash";
    [[maybe_unused]] int rc = system(install_command.c_str());
  }
  std::string outputFile = cli.outputFile;
  if (std::filesystem::exists(value)) {
    build(value, outputFile, cfg::Mutability::Promiscuous, cli.debug);
  } else {
    std::cout << "File " << value << " does not exist\n";
    return 1;
  }
  return 0;
}
#endif

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug) {
  bool success = true;
  lex::Lexer scanner;
  links::LinkedList<lex::Token *> tokens;
  std::string origPath = path;
  if (!gQuiet) {
    if (gProgress)
      gProgress->update(origPath, "parsing");
    else
      std::cout << "[parsing] " << origPath << std::endl;
  }

  auto filename = getExePath();
  auto wd = std::filesystem::current_path();
  auto exepath = filename.substr(0, filename.find_last_of("/"));
  auto libPath =
      exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std/head/";
  std::ifstream ifs(path);
  std::string content((std::istreambuf_iterator<char>(ifs)),
                      (std::istreambuf_iterator<char>()));
  ifs.close();
  gen::scope::ScopeManager::getInstance()->reset();
  try {
    try {
      PreProcessor pp;
      tokens = scanner.Scan(pp.PreProcess(content, libPath));
    } catch (int x) {
      int line = 1;
      for (int i = 0; i < x && i < content.size(); ++i)
        if (content[i] == '\n') line++;
      error::report(path, line, "unparsable character", content);
      return false;
    }
    tokens.invert();
    parse::Parser parser(mutability);

    auto Prog = parser.parseStmt(tokens);

    auto lower = parse::lower::Lowerer(Prog);

    auto outputID = output.substr(0, output.find_last_of("."));
    if (outputID.find("/") != std::string::npos) {
      outputID = outputID.substr(outputID.find_last_of("/") + 1);
    }

    // if output id = mod then switch to the parent directory
    if (outputID == "mod") {
      outputID = output.substr(0, output.find_last_of("/"));
      outputID = outputID.substr(outputID.find_last_of("/") + 1);
    }

    gen::CodeGenerator genny(outputID, parser, content);
    genny.mutability = mutability;
    auto file = genny.GenSTMT(Prog);
    if (!gQuiet) {
      if (gProgress)
        gProgress->update(origPath, "generating");
      else
        std::cout << "[generating] " << origPath << std::endl;
    }
    file.collect();
    if (genny.hasError()) {
      success = false;
      if (std::filesystem::exists(output)) std::filesystem::remove(output);
      return success;
    }

    file.text.invert();
    file.bss.invert();
    file.linker.invert();
    file.data.invert();

    std::ofstream ofs;

    // if there is a dot as the first char of the path, remove it
    if (path[0] == '.') {
      path = path.substr(1);
    };

    if (path[0] != '/') {
      path = wd.string() + "/" + path;
    } else {
      path = wd.string() + path;
    }

    // assembly file output
    ofs.open(output);

    // output linker commands
    while (file.linker.head != nullptr) {
      ofs << file.linker.pop()->toString();
    }
    // text section output
    ofs << "\n\n.text\n\n";
    // write the .file directive if in debug mode
    if (debug) {
      ofs << ".file\t\"" << path << "\"\n";
    }
    int logicalLine = -1;

    while (file.text.head != nullptr) {
      auto inst = file.text.pop();
      if (inst->logicalLine != logicalLine && debug &&
          dynamic_cast<asmc::Label *>(inst) == nullptr &&
          inst->logicalLine > 0) {
        logicalLine = inst->logicalLine;
      }
      if (debug)
        if (inst->logicalLine > 0 &&
            dynamic_cast<asmc::Define *>(inst) == nullptr)
          ofs << ".line " << inst->logicalLine - 1 << "\n";
        else if (logicalLine > 0 &&
                 dynamic_cast<asmc::Define *>(inst) != nullptr)
          ofs << ".line " << logicalLine - 1 << "\n";
      auto str = inst->toString();
      // replace '\n' with "\n .line " + line number
      // while(str.find('\n') != std::string::npos){
      //   auto index = str.find('\n');
      //   str = str.substr(0, index) + "\n.line " +
      //   std::to_string(inst->logicalLine) + "\n" + str.substr(index +
      //   1);
      // }
      if (dynamic_cast<asmc::Define *>(inst) == nullptr) ofs << str;
      // if (debug && dynamic_cast<asmc::Define *>(inst) != nullptr) ofs <<
      // inst->toString();
    }

    // data section output
    ofs << "\n\n.data\n\n";
    while (file.data.head != nullptr) {
      ofs << file.data.pop()->toString();
    }

    // bss section output
    ofs << "\n\n.bss\n\n";
    while (file.bss.head != nullptr) {
      ofs << file.bss.pop()->toString();
    }
    ofs.close();
    if (!gQuiet) {
      if (gProgress)
        gProgress->update(origPath, "done");
      else
        std::cout << "[done] " << origPath << std::endl;
    }
  } catch (err::Exception &e) {
    success = false;
    int line = error::extractLine(e.errorMsg);
    error::report(path, line, e.errorMsg, content);
    if (std::filesystem::exists(output)) std::filesystem::remove(output);
    if (!gQuiet && gProgress) gProgress->update(origPath, "done");
  }
  return success;
};

/*
 * function name:   getExePath
 * description:     gets the path of the executable
 * parameters:      none
 * return value:    string - the path of the executable
 */
std::string getExePath() {
  char result[200];
  auto count = readlink("/proc/self/exe", result, 200);
  return std::string(result, (count > 0) ? count : 0);
}

/*
 * function name:   buildTemplate
 * description:     creates a template file for the user to write their program
 * parameters:      std::string value - the name of the project to be created
 * return value:    void
 */
void buildTemplate(std::string value) {
  std::filesystem::create_directories(value);
  std::filesystem::create_directories(value + "/src");
  std::filesystem::create_directories(value + "/src/test");
  std::filesystem::create_directories(value + "/head");
  std::filesystem::create_directories(value + "/bin");

  std::ofstream outfile(value + "/src/main.af");
  outfile << ".needs <std>\n";
  outfile << "import {print} from \"io\" under io;\n\n";
  outfile
      << "int main(){\n\tio.print(\"Hello, World!\\n\");\n\treturn 0;\n};\n";
  outfile.close();

  outfile = std::ofstream(value + "/src/test/test.af");
  outfile << ".needs <std>\n\n";
  outfile
      << "import {describe, it, assertEqual} from \"ATest\" under test;\n"
         "import Map from \"Utils/Map\";\n\n"
         "fn main() -> int {\n"
         "\ttest.describe(\"Test Suite 1\", fn (Map __context) -> bool {\n"
         "\t\ttest.it(\"should pass the first test\", fn (Map __context) {\n"
         "\t\t\ttest.assertEqual(1, 1);\n"
         "\t\t});\n"
         "\t\ttest.it(\"should fail the second test\", fn (Map __context) {\n"
         "\t\t\ttest.assertEqual(`value`, `other`);\n"
         "\t\t});\n"
         "\t\treturn true;\n"
         "\t});\n"
         "\treturn 0;\n"
         "};";
  outfile.close();

  outfile = std::ofstream(value + "/aflat.cfg");

  // Write the standard Config file in INI format
  outfile << "[build]\n";
  outfile << "main = main\n";
  outfile << "test = test/test\n";
  outfile.close();
}

/*
 * function name: LibTemplate
 * description:   creates a template file for the user to write their library
 * parameters:    std::string value - the name of the library to be created
 * return value:  void
 */
void libTemplate(std::string value) {
  std::filesystem::create_directories(value);
  std::filesystem::create_directories(value + "/src");
  std::filesystem::create_directories(value + "/src/test");
  std::filesystem::create_directories(value + "/bin");

  std::ofstream outfile(value + "/src/mod.af");
  outfile << ".needs <std>\n";
  outfile << "export int " << value << "(int a, int b){\n\treturn a + b;\n};\n";
  outfile.close();

  outfile = std::ofstream(value + "/aflat.cfg");
  outfile << "[build]\n";
  outfile << "main = mod\n";
  outfile << "test = test/test\n";
  outfile << "\n[dependencies]\n";
  outfile << "mod = \"./src/mod.af\"\n";
  outfile.close();

  outfile = std::ofstream(value + "/src/test/test.af");
  outfile << ".needs <std>\n\n";
  outfile << "import {describe, it, assertEqual} from \"ATest\" under test;\n"
             "import Map from \"Utils/Map\";\n"
             "import {"
          << value
          << "} from \"src/mod\";\n\n"
             "fn main() -> int {\n"
             "\ttest.describe(\""
          << value
          << " Test Suite\", fn (Map __context) -> bool {\n"
             "\t\ttest.it(\"test_"
          << value
          << "\", fn (Map __context) {\n"
             "\t\t\ttest.assertEqual("
          << value
          << "(1, 2), 3);\n"
             "\t\t});\n"
             "\t\treturn true;\n"
             "\t});\n"
             "\treturn 0;\n"
             "};";

  outfile.close();
}

void ensureBinPath(const std::string &path,
                   std::vector<std::string> &pathList) {
  std::string addPath = "";
  if (path.find("/") != std::string::npos) {
    addPath = path.substr(0, path.find_last_of("/"));
  }

  const bool found = std::any_of(
      pathList.begin(), pathList.end(),
      [&](const std::string &searchPath) { return searchPath == addPath; });

  if (!found && !addPath.empty()) {
    std::filesystem::create_directories("./bin/" + addPath);
    pathList.push_back(addPath);
  }
}

bool compileCFile(const std::string &path, bool debug) {
  std::string src = "./src/" + path + ".c";
  std::string dst = "./bin/" + path + ".s";
  if (!gQuiet) {
    if (gProgress)
      gProgress->update(src, "parsing");
    else
      std::cout << "[parsing] " << src << std::endl;
  }
  std::string cmd = compilerutils::buildCompileCmd(src, dst, debug);
  if (!gQuiet) {
    if (gProgress)
      gProgress->update(src, "generating");
    else
      std::cout << "[generating] " << src << std::endl;
  }
  bool result = system(cmd.c_str()) == 0;
  if (!gQuiet) {
    if (gProgress)
      gProgress->update(src, "done");
    else if (result)
      std::cout << "[done] " << src << std::endl;
  }
  return result;
}

bool runConfig(cfg::Config &config, const std::string &libPath) {
  return runConfig(config, libPath, 'e');
}

bool runConfig(cfg::Config &config, const std::string &libPath, char pmode) {
  std::vector<std::string> linker;
  std::vector<std::string> pathList;
  bool hasError = false;

  std::string ofile = config.outPutFile;

  if (pmode == 'e') {
    const auto entryPoint = config.entryPoint;
    config.modules.push_back(entryPoint);
  } else if (pmode == 't') {
    config.modules.push_back(config.testFile);
  }

  std::vector<std::string> sources;
  for (const auto &mod : config.modules) sources.push_back("./src/" + mod + ".af");
  for (const auto &file : config.cFiles) sources.push_back("./src/" + file + ".c");

  CompileProgress progress(sources, gQuiet);
  if (!gQuiet) gProgress = &progress;

  for (auto mod : config.modules) {
    ensureBinPath(mod, pathList);
    if (build("./src/" + mod + ".af", "./bin/" + mod + ".s", config.mutability,
              config.debug)) {
      linker.push_back("./bin/" + mod + ".s");
    } else {
      hasError = true;
    }
  }

  for (auto file : config.cFiles) {
    ensureBinPath(file, pathList);
    if (compileCFile(file, config.debug)) {
      linker.push_back("./bin/" + file + ".s");
    } else {
      hasError = true;
    }
  }

  std::vector<std::string> libs = {"io.s",
                                   "Collections.s",
                                   "math.s",
                                   "strings.s",
                                   config.compatibility ? "std-cmp.s" : "std.s",
                                   "concurrency.s",
                                   "files.s",
                                   "asm.s",
                                   "String.s",
                                   "DateTime.s",
                                   "HTTP.s",
                                   "request.s",
                                   "ATest.s",
                                   "CLArgs.s",
                                   "System.s",
                                   "Utils_Result.s",
                                   "Utils_result.s",
                                   "Utils_Option.s",
                                   "Utils_Functions.s",
                                   "Utils_Map.s",
                                   "Utils_Properties.s",
                                   "Utils_Object.s",
                                   "Utils_Error.s",
                                   "Error_Render.s",
                                   "HTTP_Endpoint.s",
                                   "HTTP_Server.s",
                                   "HTTP_Endpoints.s",
                                   "HTTP_Middleware.s",
                                   "Web_Content.s",
                                   "Web_Content_Bind.s",
                                   "JSON.s",
                                   "JSON_Parse.s",
                                   "JSON_Property.s",
                                   "Iterator.s",
                                   "Enumerator.s",
                                   "Scroller.s",
                                   "vector.s",
                                   "Utils_Defer.s",
                                   "Memory.s",
                                   "Utils_Observable.s"};

  for (const auto &lib : libs) {
    linker.insert(linker.begin(), libPath + lib);
  }

  if (hasError) {
    std::cout << "Errors detected. Skipping linking." << std::endl;
    gProgress = nullptr;
    return false;
  }

  // run gcc on the linkerList
  std::string linkerList = "";

  for (auto &s : linker) linkerList += s + " ";

  if (pmode == 't') {
    ofile = "./bin/a.test ";
  };

  std::string gcc =
      compilerutils::buildLinkCmd(ofile, linkerList, config.debug);
  [[maybe_unused]] int rc = system(gcc.c_str());
  linker.erase(linker.begin(), linker.begin() + libs.size());

  if (!config.asm_) {
    for (auto &s : linker) {
      std::filesystem::remove(s);
    }

    // remove the paths from the pathList
    for (auto &s : pathList) {
      std::filesystem::remove_all("./bin/" + s);
    }
  }
  gProgress = nullptr;
  return true;
}
