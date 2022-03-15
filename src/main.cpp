#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <algorithm>
#include "Scanner.hpp"
#include "LinkedList.hpp"
#include "Parser.hpp"
#include "CodeGenerator/CodeGenerator.hpp"
#include "ASM.hpp"
#include "Exceptions.hpp" 
#include "PreProcessor.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include <thread>

std::string preProcess(std::string input);
std::string getExePath();
void buildTemplate(std::string value);
void build(std::string path, std::string output);
void runConfig(std::string path, std::string libPath, char pmode = 'e');

int main(int argc, char *argv[]){
    // Usage error
    if(argc < 2){
        std::cout << "Usage: aflat <file> <output>\n";
        return 1;
    }

    std::string filename = getExePath();
    std::string exepath = filename.substr(0, filename.find_last_of("/"));
    std::string libPathA = exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std/";

    std::string value = argv[1];

    if (value == "make"){
        if (argc != 3) {
            std::cout << "Usage: aflat <make> <project name>\n";
            return 1;
        }
        std::cout << "creating " << argv[2] << '\n';
        std::string pName = argv[2];
        buildTemplate(pName);
        return 0;
    }
    if (value == "build"){
        runConfig("./aflat.cfg", libPathA);
        return 0;
    }
    if (value == "run"){
        runConfig("./aflat.cfg", libPathA);
        system("./bin/a.out");
        return 0;
    }
    if (value == "test"){
        runConfig("./aflat.cfg", libPathA, 't');
        system("./bin/a.test");
        return 0;
    }
    if (value == "add"){
        if (argc < 3){
            std::cout << "Usage: aflat add <modual name>\n";
            return 1;
        }
        std::string modualName = argv[2];
        std::string srcName = "./src/" + modualName + ".af";
        std::string headerName = "./head/" + modualName + ".gs";

        std::string path = "";

        //Check if the modual name has a path
        if (modualName.find("/") != std::string::npos){
            path = modualName.substr(0, modualName.find_last_of("/"));
        }

        //Check if the modual path exists
        if (path != ""){
            if (!std::filesystem::exists(path)){
                //Create the paths
                std::filesystem::create_directories("./src/" + path);
                std::filesystem::create_directories("./head/" + path);
            }
        }

        // create the files
        std::ofstream srcFile(srcName);
        std::ofstream headerFile(headerName);
        srcFile << ".root \"head\"\n";
        srcFile << ".needs \"" << modualName << ".gs\"\n";
        srcFile.close();
        headerFile.close();

        //Read the last line of the config file
        std::fstream cFile("./aflat.cfg", std::fstream::in | std::fstream::out);
        cFile.seekg(-1,cFile.end); // move the the end
        char c;
        cFile.get(c);
        cFile.close();

        // add the modual to the config file
        std::fstream configFile("./aflat.cfg", std::ios::app | std::ios::in);
        // if the last line is not a newline add a newline
        if (c != '\n'){
            configFile << '\n';
        }

        configFile << "m "<< modualName << "\n";
        return 0;
    }

    std::string outputFile;
    if (argc == 2) outputFile = "out.s"; else outputFile = argv[2];
    if (std::filesystem::exists(value)){
        build(value, outputFile);
    } else {
        std::cout << "File " << value << " does not exist\n";
        return 1;
    }
    return 0;
}

void build(std::string path, std::string output){
    lex::Lexer scanner;
    links::LinkedList<lex::Token* > tokens;

    std::string filename = getExePath();
    std::string exepath = filename.substr(0, filename.find_last_of("/"));
    std::string libPath = exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std/head/";
    std::ifstream ifs(path);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );
    ifs.close();
    gen::scope::ScopeManager::getInstance()->reset();
    try{
        try{
            PreProcessor pp;
            //pp.debug = true;
            tokens = scanner.Scan(pp.PreProcess(content, libPath));
        }catch (int x){
            std::cout << " unparsable Char at index " + x;
            return;
        }
        tokens.invert();
        parse::Parser parser;

        ast::Statment * Prog = parser.parseStmt(tokens);
        
        gen::CodeGenerator genny;
        asmc::File file = genny.GenSTMT(Prog);
        file.collect();
        
        file.text.invert();
        file.bss.invert();
        file.linker.invert();
        file.data.invert();

        std::ofstream ofs;
        
        // assembly file output
        ofs.open(output);


        // output linker commands
        while(file.linker.head != nullptr){
            ofs << file.linker.pop()->toString();
        }

        // text section output
        ofs << "\n\n.text\n\n";

        while(file.text.head != nullptr){
            ofs << file.text.pop()->toString();
        }

        // data section output
        ofs << "\n\n.data\n\n";
        while(file.data.head != nullptr){
            ofs << file.data.pop()->toString();
        }


        // bss section output
        ofs << "\n\n.bss\n\n";
        while(file.bss.head != nullptr){
            ofs << file.bss.pop()->toString();
        }
        ofs.close();
    }catch(err::Exception e){
        std::cout << std::endl << "Exception in " << path << ": " << e.errorMsg << std::endl << std::endl;
    }
    return;
};

/*
 * function name:   remove_char
 * Description:     removes a character from a string
 * Parameters:      std::string str - the string to remove from
 *                 char ch - the character to remove
 * Returns:         std::string - the string with the character removed
 */
std::string remove_char( std::string str, char ch ){
    // remove all occurrences of char ch from str
    str.erase( std::remove( str.begin(), str.end(), ch ), str.end() ) ;
    return str ;
}

/*
 * function name:   getExePath
 * description:     gets the path of the executable
 * parameters:      none
 * return value:    string - the path of the executable
 */
std::string getExePath()
{
  char result[ 200 ];
  ssize_t count = readlink("/proc/self/exe", result, 200 );
  return std::string( result, (count > 0) ? count : 0 );
}

/*
 * function name:   buildTemplate
 * description:     creates a template file for the user to write their program
 * parameters:      std::string value - the name of the project to be created
 * return value:    void
 */
void buildTemplate(std::string value){
    std::string filename = getExePath();
    std::string exepath = filename.substr(0, filename.find_last_of("/"));
    std::string libPath = exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std";
    std::filesystem::create_directories(value);
    std::filesystem::create_directories(value + "/src");
    std::filesystem::create_directories(value + "/src/test");
    std::filesystem::create_directories(value + "/head");
    std::filesystem::create_directories(value + "/bin");


    std::filesystem::path cwd = std::filesystem::current_path();
    std::string root =  cwd.string() + "/" + value;
    std::ofstream outfile (value + "/src/main.af");
    outfile << ".needs <io>\n";
    outfile << "int main(){\n\tprint(\"Hello, World!\\n\");\n\treturn 0;\n};\n";
    outfile.close();

    outfile = std::ofstream(value + "/src/test/test.af");
    outfile << ".needs <io>\n";
    outfile << "int main(){\n\tassert(1 == 1, \"Failed 1 == 1 assert\");\n\treturn 0;\n};\n";
    outfile.close();

    outfile = std::ofstream(value + "/aflat.cfg");
    
    // Write the standard Config file
    outfile << "; Aflat Config File\n";
    outfile << "e main\n";
    outfile << "t test/test\n";
    outfile.close();
}

void runConfig(std::string path, std::string libPath, char pmode = 'e'){
    bool debug = false;
    std::vector<std::string> linker;
    std::vector<std::string> pathList;
    std::vector<std::thread> threads;
    // open the config file
    std::ifstream ifs(path);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );

    // loop through the config file line by line
    std::stringstream ss(content);
    std::string line;

    linker.push_back(libPath + "io.s");
    linker.push_back(libPath + "collections.s");
    linker.push_back(libPath + "math.s");
    linker.push_back(libPath + "strings.s");
    linker.push_back(libPath + "std.s");
    linker.push_back(libPath + "concurrancy.s");
    linker.push_back(libPath + "files.s");
    linker.push_back(libPath + "asm.s");
    linker.push_back(libPath + "string.s");

    while(std::getline(ss, line)){
        line = remove_char(line, '\t');

        // get a copy of the line after the first char
        std::string copy = line.substr(1);
        copy = remove_char(copy, ' ');
        std::string lowerCaseCopy = copy;
        std::transform(lowerCaseCopy.begin(), lowerCaseCopy.end(), lowerCaseCopy.begin(), ::tolower);

        // if the line is a comment, skip it
        if(line[0] == ';') continue;

        // if the line is a dependency, build it and add it to the linker
        if(line[0] == 'm' | line[0] == pmode){
            std::string addPath = "";

            //Check if the modual name has a path
            if (copy.find("/") != std::string::npos){
                addPath = copy.substr(0, copy.find_last_of("/"));
            }

            // Check if path is in the pathList
            bool found = false;
            for(int i = 0; i < pathList.size(); i++){
                if(pathList[i] == addPath){
                    found = true;
                    break;
                }
            }

            if(!found && addPath != ""){
                std::filesystem::create_directories("./bin/" + addPath);
                pathList.push_back(addPath);
            }

            // add the thread to the vector of threads
            build("./src/" + copy + ".af", "./bin/" + copy + ".s");

            // add the library to the linker
            linker.push_back("./bin/" + copy + ".s");
        }

        // check if line starts with 'settings'
        if(line.substr(0, 3) == "set"){
            std::string setting = line.substr(4);
            if (setting == "debug"){
                debug = true;
            };
        }

    }

    // join all the threads
    for(int i = 0; i < threads.size(); i++){
        threads[i].join();
    }  

    // run gcc on the linkerList
    std::string linkerList = "";
    for(auto& s : linker){
        linkerList += s + " ";
    }

    std::string ofile = "./bin/a.out ";
    if (pmode == 't'){
        ofile = "./bin/a.test ";
    };

    std::string gcc = "gcc -O0 -no-pie -o " + ofile + linkerList;
    if (debug){
        gcc = "gcc -O0 -g -no-pie -o " + ofile + linkerList;
    }

    system(gcc.c_str());

    // remove first 8 elements from the linker list
    linker.erase(linker.begin(), linker.begin() + 9);

    // delete the linkerList files
    if (!debug){
        for(auto& s : linker){
            std::filesystem::remove(s);
        }

        // remove the paths from the pathList
        for(auto& s : pathList){
            std::filesystem::remove_all("./bin/" + s);
        }
    };
}
