#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <algorithm>
#include "Scanner.hpp"
#include "LinkedList.hpp"
#include "Parser.hpp"
#include "CodeGenerator.hpp"
#include "ASM.hpp"
#include "Exceptions.hpp" 
#include "PreProcessor.hpp"

std::string trim(std::string str);
std::string preProcess(std::string input);
void buildTemplate(std::string value);

int main(int argc, char *argv[])
{
    try{
        lex::Lexer scanner;
        links::LinkedList<lex::Token* > tokens;

        std::string filename = getExePath();
        std::string exepath = filename.substr(0, filename.find_last_of("/"));
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
        try{
            PreProcessor pp;
            tokens = scanner.Scan(pp.PreProcess(content, libPath));
        }catch (int x){
            std::cout << " unparsable Char at index " + x;
            return 0;
        }
        tokens.invert();
        parse::Parser parser;

        AST::Statment * Prog = parser.parseStmt(tokens);
        
        gen::CodeGenerator genny;
        ASMC::File file = genny.GenSTMT(Prog);
        file.text.invert();

        file.bss.invert();
        file.linker.invert();
        file.data.invert();

        std::ofstream ofs;

        if (argc == 1) ofs.open("out.s"); else ofs.open(argv[2]);

        while(file.linker.head != nullptr){
            ofs << file.linker.pop()->toString();
        }

        ofs << "\n\n.text\n\n";

        while(file.text.head != nullptr){
            ofs << file.text.pop()->toString();
        }

        ofs << "\n\n.data\n\n";
        while(file.data.head != nullptr){
            ofs << file.data.pop()->toString();
        }

        ofs << "\n\n.bss\n\n";
        while(file.bss.head != nullptr){
            ofs << file.bss.pop()->toString();
        }
        ofs.close();
    }catch(err::Exception e){
        std::cout << std::endl << "Exception: " << e.errorMsg << std::endl << std::endl;
    }
    return 0;
}

std::string remove_char( std::string str, char ch )
{
    // remove all occurrences of char ch from str
    str.erase( std::remove( str.begin(), str.end(), ch ), str.end() ) ;
    return str ;
}


std::string getExePath()
{
  char result[ 200 ];
  ssize_t count = readlink("/proc/self/exe", result, 200 );
  return std::string( result, (count > 0) ? count : 0 );
}

/* Creates a new Project */
void buildTemplate(std::string value){
    std::string filename = getExePath();
    std::string exepath = filename.substr(0, filename.find_last_of("/"));
    std::string libPath = exepath.substr(0, exepath.find_last_of("/")) + "/libraries/std";
    std::filesystem::create_directories(value);
    std::filesystem::create_directories(value + "/src");
    std::filesystem::create_directories(value + "/head");


    std::filesystem::path cwd = std::filesystem::current_path();
    std::string root =  cwd.string() + "/" + value;
    std::ofstream outfile (value + "/src/main.af");
    outfile << ".needs \"" <<"/std/head/io.gs\"\n";
    outfile << "int main(){\n\tprint(\"Hello, World!\\n\");\n\treturn 0;\n};\n";
    outfile.close();

    outfile = std::ofstream(value + "/build.sh");
    outfile << "#!/bin/sh\n" << filename << " src/main.af out.s\n";
    outfile << "gcc -O0 -g -no-pie out.s " << libPath << "/asm.s " << libPath << "/std.s " << libPath << "/io.s "
    << libPath << "/collections.s " << libPath << "/math.s " << libPath << "/strings.s " << libPath << "/files.s ";

}