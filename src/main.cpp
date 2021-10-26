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
            tokens = scanner.Scan(pp.PreProcess(content));
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
    std::filesystem::create_directories(value + "/std");
    std::filesystem::create_directories(value + "/std/src");
    std::filesystem::create_directories(value + "/std/head");

    std::cout << libPath << std::endl;

    std::ifstream in(libPath);
    std::ofstream out(value + "/std");

    std::string ip = libPath + "/head";
    std::string op = value + "/std/head";
    in = std::ifstream(ip);
    out = std::ofstream(op);

    out << in.rdbuf();

    ip = libPath + "/head/io.gs";
    op = value + "/std/head/io.gs";
    in = std::ifstream(ip);
    out = std::ofstream(op);

    out << in.rdbuf();

     in = std::ifstream(std::string(libPath + "/asm.s"));
    out = std::ofstream(std::string(value + "/std/asm.s"));

    out << in.rdbuf();

    in = std::ifstream(std::string(libPath + "/head/math.gs"));
    out = std::ofstream(std::string(value + "/std/head/math.gs"));

    out << in.rdbuf();

    in = std::ifstream(std::string(libPath + "/head/strings.gs"));
    out = std::ofstream(std::string(value + "/std/head/strings.gs"));

    out << in.rdbuf();

    in = std::ifstream(std::string(libPath + "/head/asm.gs"));
    out = std::ofstream(std::string(value + "/std/head/asm.gs"));

    out << in.rdbuf();

    in = std::ifstream(std::string(libPath + "/head/files.gs"));
    out = std::ofstream(std::string(value + "/std/head/files.gs"));

    out << in.rdbuf();

    in = std::ifstream(std::string(libPath + "/head/Collections.gs"));
    out = std::ofstream(std::string(value + "/std/head/Collections.gs"));

    out << in.rdbuf();

    in = std::ifstream(std::string(libPath + "/head/std.gs"));
    out = std::ofstream(std::string(value + "/std/head/std.gs"));

    out << in.rdbuf();

    in = std::ifstream(std::string(libPath + "/src"));
    out = std::ofstream(std::string(value + "/std/src"));

    out << in.rdbuf();

    
    in = std::ifstream(std::string(libPath + "/src/io.af"));
    out = std::ofstream(std::string(value + "/std/src/io.af"));

    out << in.rdbuf();

    in = std::ifstream(std::string(libPath + "/src/Collections.af"));
    out = std::ofstream(std::string(value + "/std/src/Collections.af"));

    out << in.rdbuf();

    in = std::ifstream(std::string(libPath + "/src/std.af"));
    out = std::ofstream(std::string(value + "/std/src/std.af"));

    out << in.rdbuf();

    in = std::ifstream(std::string(libPath + "/src/files.af"));
    out = std::ofstream(std::string(value + "/std/src/files.af"));

    out << in.rdbuf();

    

    in = std::ifstream(std::string(libPath + "/src/math.af"));
    out = std::ofstream(std::string(value + "/std/src/math.af"));

    out << in.rdbuf();
    
    in = std::ifstream(std::string(libPath + "/src/strings.af"));
    out = std::ofstream(std::string(value + "/std/src/strings.af"));

    out << in.rdbuf();

    std::filesystem::path cwd = std::filesystem::current_path();
    std::string root =  cwd.string() + "/" + value;
    std::ofstream outfile (value + "/src/main.af");
    outfile << ".needs \"" <<"/std/head/io.gs\"\n";
    outfile << "int main(){\n\tprint(\"Hello, World!\\n\");\n\treturn 0;\n};\n";
    outfile.close();

    outfile = std::ofstream(value + "/build.sh");
    outfile << "#!/bin/sh\n" << filename << " src/main.af out.s\n";
    outfile <<filename << " std/src/io.af std/io.s\n";
    outfile <<  filename << " std/src/math.af std/math.s\n";
    outfile <<  filename << " std/src/strings.af std/strings.s\n";
    outfile <<  filename << " std/src/Collections.af std/collections.s\n";
    outfile <<  filename << " std/src/files.af std/files.s\n";
    outfile <<  filename << " std/src/std.af std/std.s\n";
    outfile << "gcc -O0 -g -no-pie out.s std/asm.s std/std.s std/io.s std/collections.s std/math.s std/strings.s std/files.s";

}

