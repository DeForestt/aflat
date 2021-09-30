#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "Scanner.hpp"
#include "LinkedList.hpp"
#include "Parser.hpp"
#include "CodeGenerator.hpp"
#include "ASM.hpp"
#include "Exceptions.hpp"

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
        std::cout << argv[2];
        try{
            tokens = scanner.Scan(preProcess(content));
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

        file.data.invert();
        file.linker.invert();

    

        std::ofstream ofs;

        if (argc == 1) ofs.open("out.s"); else ofs.open(argv[2]);

        while(file.linker.head != nullptr){
            ofs << file.linker.pop()->toString();
        }
        ofs << "\n\n.text\n\n";
        while(file.data.head != nullptr){
            ofs << file.data.pop()->toString();
        }

        while(file.text.head != nullptr){
            ofs << file.text.pop()->toString();
        }
        ofs.close();
    }catch(err::Exception e){
        std::cout << std::endl << "Exception: " << e.errorMsg << std::endl << std::endl;
    }
    return 0;
}

std::string preProcess(std::string input){
    std::string output = "";
    std::stringstream input_stringstream(input);
    std::string line;
    std::string root = "~/";

    while (getline(input_stringstream, line, '\n'))
    {
        line = trim(line);
        if(line.substr(0, 2) != "//"){
            if(line.substr(0, 6) == ".needs"){
                int startPos = line.find_first_of('\"') + 1;
                int endPos = line.find_last_of('\"');
                
                std::string relpath = line.substr(startPos, endPos - startPos);
                std::string path = root + relpath;
                std::fstream f(path, std::fstream::in);
                string content;
                std::getline( f, content, '\0');
                output += preProcess(content);
            }else if(line.substr(0, 5) == ".root"){
                int startPos = line.find_first_of('\"') + 1;
                int endPos = line.find_last_of('\"');
                root = line.substr(startPos, endPos - startPos);
            }
            else{
                output += line;
            };
        }
    }
    return output;
};

void buildTemplate(std::string value){
    std::filesystem::create_directories(value);
}

std::string trim( std::string str )
{
    // remove trailing white space
    while( !str.empty() && std::isspace( str.back() ) ) str.pop_back() ;

    // return residue after leading white space
    std::size_t pos = 0 ;
    while( pos < str.size() && std::isspace( str[pos] ) ) ++pos ;
    return str.substr(pos) ;
}