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
void runConfig(std::string path, std::string libPath);

int main(int argc, char *argv[]){
    // Usage error
    if(argc < 2){
        std::cout << "Usage: aflat <file> <output>\n";
        return 1;
    }

    lex::Lexer scanner;
    links::LinkedList<lex::Token* > tokens;

    std::string filename = getExePath();
    std::string exepath = filename.substr(0, filename.find_last_of("/"));
    std::string libPathA = exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std/";
    std::string libPath = exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std/head/";

    std::ifstream ifs(argv[1]);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                    (std::istreambuf_iterator<char>()    ) );
    ifs.close();
    std::string value = argv[1];

    if (value == "make"){
        std::cout << "creating " << argv[2] << '\n';
        std::string pName = argv[2];
        buildTemplate(pName);
        return 0;
    }
    if (value == "build"){
        runConfig("./aflat.cfg", libPathA);
        return 0;
    }

    std::string outputFile;
    if (argc == 2) outputFile = "out.s"; else outputFile = argv[2];
    build(value, outputFile);
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
    try{
        try{
            PreProcessor pp;
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
        std::cout << std::endl << "Exception: " << e.errorMsg << std::endl << std::endl;
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
    std::filesystem::create_directories(value + "/head");
    std::filesystem::create_directories(value + "/bin");


    std::filesystem::path cwd = std::filesystem::current_path();
    std::string root =  cwd.string() + "/" + value;
    std::ofstream outfile (value + "/src/main.af");
    outfile << ".needs <io>\n";
    outfile << "int main(){\n\tprint(\"Hello, World!\\n\");\n\treturn 0;\n};\n";
    outfile.close();

    outfile = std::ofstream(value + "/aflat.cfg");
    
    // Write the standard Config file
    outfile << "; Aflat Config File\n";
    outfile << "m main\n";
    outfile.close();
}

void runConfig(std::string path, std::string libPath){
    std::vector<std::string> linker;
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
        if(line[0] == 'm'){
            std::string path = getExePath();
            std::string exepath = path.substr(0, path.find_last_of("/"));
            std::string libPath = exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std";

            // add the thread to the vector of threads
            threads.push_back(std::thread([libPath, copy](){
                build("./src/" + copy + ".af", "./bin/" + copy + ".s");;
            }));

            // add the library to the linker
            linker.push_back("./bin/" + copy + ".s");
        }

    }

    // join all the threads
    for(auto& t : threads){
        t.join();
    }

    // run gcc on the linkerList
    std::string linkerList = "";
    for(auto& s : linker){
        linkerList += s + " ";
    }
    std::string gcc = "gcc -O0 -g -no-pie -o bin/a.out " + linkerList;
    system(gcc.c_str());

    // remove first 8 elements from the linker list
    linker.erase(linker.begin(), linker.begin() + 8);

    // delete the linkerList files
    for(auto& s : linker){
        std::filesystem::remove(s);
    }
}
