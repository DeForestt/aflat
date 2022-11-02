#include "ASM.hpp"
#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Exceptions.hpp"
#include "LinkedList.hpp"
#include "Parser/Lower.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"
#include "Scanner.hpp"
#include "Configs.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <vector>

std::string preProcess(std::string input);
std::string getExePath();
void buildTemplate(std::string value);
void build(std::string path, std::string output, cfg::Mutibility mutibility, bool debug);
void runConfig(std::string path, std::string libPath, char pmode = 'e');
void runConfig(cfg::Config config, std::string libPath, char pmode);

int main(int argc, char *argv[]) {
  // Usage error
  if (argc < 2) {
    std::cout << "Usage: aflat <file> <output>\n";
    return 1;
  }

  std::string filename = getExePath();
  std::string exepath = filename.substr(0, filename.find_last_of("/"));
  std::string libPathA =
      exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std/";

  std::string value = argv[1];

  if (value == "make") {
    if (argc != 3) {
      std::cout << "Usage: aflat <make> <project name>\n";
      return 1;
    }
    std::cout << "creating " << argv[2] << '\n';
    std::string pName = argv[2];
    buildTemplate(pName);
    return 0;
  }
  if (value == "build") {
    std::ifstream ifs("aflat.cfg");
    std::string content((std::istreambuf_iterator<char>(ifs)),
                    (std::istreambuf_iterator<char>()));
    cfg::Config config = cfg::getConfig(content);
    runConfig(config, libPathA, 'e');
    return 0;
  }
  if (value == "run") {
    std::ifstream ifs("aflat.cfg");
    std::string content((std::istreambuf_iterator<char>(ifs)),
                    (std::istreambuf_iterator<char>()));
    cfg::Config config = cfg::getConfig(content);
    runConfig(config, libPathA, 'e');
    auto of = config.outPutFile;
    if (config.outPutFile[0] != '.' && config.outPutFile[1] != '/') {
      of = "./" + config.outPutFile;
    }
    system(of.c_str());
    return 0;
  }
  if (value == "test") {
    std::ifstream ifs("aflat.cfg");
    std::string content((std::istreambuf_iterator<char>(ifs)),
                    (std::istreambuf_iterator<char>()));
    cfg::Config config = cfg::getConfig(content);
    runConfig(config, libPathA, 't');
    system("./bin/a.test");
    return 0;
  }
  if (value == "add" || value == "module") {
    if (argc < 3) {
      std::cout << "Usage: aflat add <modual name>\n";
      return 1;
    }
    std::string modualName = argv[2];
    std::string srcName = "./src/" + modualName + ".af";
    std::string headerName = "./head/" + modualName + ".gs";

    std::string path = "";

    // Check if the modual name has a path
    if (modualName.find("/") != std::string::npos) {
      path = modualName.substr(0, modualName.find_last_of("/"));
    }

    // Check if the modual path exists
    if (path != "") {
      if (!std::filesystem::exists(path)) {
        // Create the paths
        std::filesystem::create_directories("./src/" + path);
        if (value == "add")
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

    // Read the last line of the config file
    std::fstream cFile("./aflat.cfg", std::fstream::in | std::fstream::out);
    cFile.seekg(-1, cFile.end); // move the the end
    char c;
    cFile.get(c);
    cFile.close();

    // add the modual to the config file
    std::fstream configFile("./aflat.cfg", std::ios::app | std::ios::in);
    // if the last line is not a newline add a newline
    if (c != '\n') {
      configFile << '\n';
    }

    configFile << "m " << modualName << "\n";
    return 0;
  }
  if (value == "update") {
    std::string update_command = "curl -s https://raw.githubusercontent.com/DeForestt/aflat/main/install.sh | bash";
    system(update_command.c_str());
    return 0;
  }
  std::string outputFile;
  if (argc == 2)
    outputFile = "out.s";
  else
    outputFile = argv[2];
  if (std::filesystem::exists(value)) {
    build(value, outputFile, cfg::Mutibility::Promiscuous, true);
  } else {
    std::cout << "File " << value << " does not exist\n";
    return 1;
  }
  return 0;
}

void build(std::string path, std::string output, cfg::Mutibility mutability, bool debug) {
  lex::Lexer scanner;
  links::LinkedList<lex::Token *> tokens;

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
      std::cout << " unparsable Char at index " + x;
      return;
    }
    tokens.invert();
    parse::Parser parser(mutability);

    auto Prog = parser.parseStmt(tokens);

    auto lower = parse::lower::Lowerer(Prog);

    auto outputID = output.substr(0, output.find_last_of("."));
    if (outputID.find("/") != std::string::npos) {
      outputID = outputID.substr(outputID.find_last_of("/") + 1);
    }

    gen::CodeGenerator genny(outputID);
    auto file = genny.GenSTMT(Prog);
    file.collect();

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
    if (debug){
      ofs << ".file\t\"" << path << "\"\n";
    }
    int logicalLine = -1;

    while (file.text.head != nullptr) {
      auto inst = file.text.pop();
      if (inst->logicalLine != logicalLine && debug && dynamic_cast<asmc::Lable*>(inst) == nullptr && inst->logicalLine > 0) {
        logicalLine = inst->logicalLine;
      }
      
      if (inst->logicalLine > 0 && dynamic_cast<asmc::Define *>(inst) == nullptr) ofs << ".line " << inst->logicalLine - 1 << "\n";
      else if (logicalLine > 0 && dynamic_cast<asmc::Define *>(inst) != nullptr) ofs << ".line " << logicalLine - 1 << "\n";
      auto str = inst->toString();
      // replace '\n' with "\n .line " + line number
      // while(str.find('\n') != std::string::npos){
      //   auto index = str.find('\n');
      //   str = str.substr(0, index) + "\n.line " + std::to_string(inst->logicalLine) + "\n" + str.substr(index + 1);
      // }
      if (dynamic_cast<asmc::Define *>(inst) == nullptr) ofs << str;
      //if (debug && dynamic_cast<asmc::Define *>(inst) != nullptr) ofs << inst->toString();
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
  } catch (err::Exception e) {
    std::cout << std::endl
              << "Exception in " << path << ": " << e.errorMsg << std::endl
              << std::endl;
  }
  return;
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
  auto filename = getExePath();
  auto exepath = filename.substr(0, filename.find_last_of("/"));
  auto libPath =
      exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std";
  std::filesystem::create_directories(value);
  std::filesystem::create_directories(value + "/src");
  std::filesystem::create_directories(value + "/src/test");
  std::filesystem::create_directories(value + "/head");
  std::filesystem::create_directories(value + "/bin");

  auto cwd = std::filesystem::current_path();
  auto root = cwd.string() + "/" + value;
  std::ofstream outfile(value + "/src/main.af");
  outfile << ".needs <std>\n";

  outfile << "import * from \"io\" under io;\n\n";
  outfile
      << "int main(){\n\tio.print(\"Hello, World!\\n\");\n\treturn 0;\n};\n";
  outfile.close();

  outfile = std::ofstream(value + "/src/test/test.af");
  outfile << ".needs <std>\n\n";
  outfile << "import {case, report, require} from \"ATest.af\" under test;\n"
             "import string from \"String\";\n"
             "import TestSuite from \"ATest.af\";\n\n"
             "bool simpleTest(adr _arg) : test.case {\n"
	           "\ttest.require(3 != 3, \"3 is 3\");\n"
	           "\treturn 1 == 1;\n};\n\n";
  
  outfile << "bool simpleFail(adr _arg) : test.case {\n"
	           "\treturn 1 == 2;\n"
             "};\n\n";

  outfile << "int main() {\n"
	           "\tTestSuite suite = new TestSuite(\"Simple Test Suite\");\n"
	           "\tsuite.addCase(simpleTest, \"simpleTest\");\n"
	           "\tsuite.addCase(simpleFail, \"simpleFail\");\n"
	           "\tsuite.run();\n"
	           "\ttest.report();\n"
	           "\treturn 0;\n"
             "};";
  outfile.close();

  outfile = std::ofstream(value + "/aflat.cfg");

  // Write the standard Config file
  outfile << "; Aflat Config File\n";
  outfile << "e main\n";
  outfile << "t test/test\n";
  outfile.close();
}

void runConfig(cfg::Config config, std::string libPath, char pmode) {
  std::vector<std::string> linker;
  std::vector<std::string> pathList;

    std::string ofile = config.outPutFile;

  if (pmode == 'e') {
    config.modules.push_back(config.entryPoint);
  } else if (pmode == 't') {
    config.modules.push_back(config.testFile);
  }

  for (auto mod : config.modules) {
   std::string path = mod;
   std::string addPath = "";

   if (path.find("/") != std::string::npos) {
     addPath = path.substr(0, path.find_last_of("/"));
   }

    bool found = false;
    for (auto path : pathList) {
      if (path == addPath) {
        found = true;
        break;
      }
    }

    if (!found && addPath != "") {
      std::filesystem::create_directories("./bin/" + addPath);
      pathList.push_back(addPath);
    }

    build("./src/" + path + ".af", "./bin/" + path + ".s", config.mutibility, config.debug);
    linker.push_back("./bin/" + path + ".s");
  }

  for (auto file : config.cFiles) {
    std::string path = file;
    std::string addPath = "";

    if (path.find("/") != std::string::npos) {
      addPath = path.substr(0, path.find_last_of("/"));
    }

    bool found = false;
    for (auto path : pathList) {
      if (path == addPath) {
        found = true;
        break;
      }
    }

    if (!found && addPath != "") {
      std::filesystem::create_directories("./bin/" + addPath);
      pathList.push_back(addPath);
    }

    if (config.debug)
      system(
          ("gcc -g -no-pie -S ./src/" + path + ".c -o ./bin/" + path + ".s")
              .c_str());
    else
      system(("gcc -S -no-pie ./src/" + path + ".c -o ./bin/" + path + ".s")
                 .c_str());

    linker.push_back("./bin/" + path + ".s");
  }
  
  linker.insert(linker.begin(), libPath + "io.s");
  linker.insert(linker.begin(), libPath + "Collections.s");
  linker.insert(linker.begin(), libPath + "math.s");
  linker.insert(linker.begin(), libPath + "strings.s");
  if (config.compatibility) linker.insert(linker.begin(), libPath + "std-cmp.s"); else linker.insert(linker.begin(), libPath + "std.s");
  linker.insert(linker.begin(), libPath + "concurrency.s");
  linker.insert(linker.begin(), libPath + "files.s");
  linker.insert(linker.begin(), libPath + "asm.s");
  linker.insert(linker.begin(), libPath + "String.s");
  linker.insert(linker.begin(), libPath + "DateTime.s");
  linker.insert(linker.begin(), libPath + "HTTP.s");
  linker.insert(linker.begin(), libPath + "request.s");
  linker.insert(linker.begin(), libPath + "ATest.s");
  linker.insert(linker.begin(), libPath + "CLArgs.s");

  // run gcc on the linkerList
  std::string linkerList = "";
  for (auto &s : linker) {
    linkerList += s + " ";
  }

  if (pmode == 't') {
    ofile = "./bin/a.test ";
  };

  auto gcc = "gcc -no-pie -o " + ofile + " " + linkerList;
  if (config.debug) {
    gcc = "gcc -O0 -g -no-pie -o " + ofile + " " + linkerList;
  }

  system(gcc.c_str());
  linker.erase(linker.begin(), linker.begin() + 14);

  if (!config.asm_) for (auto &s : linker) {
    std::filesystem::remove(s);
  }

  // remove the paths from the pathList
  for (auto &s : pathList) {
    std::filesystem::remove_all("./bin/" + s);
  }
}