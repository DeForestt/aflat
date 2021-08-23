#include <iostream>
#include <sstream>
#include <fstream>
#include "Scanner.hpp"
#include "LinkedList.hpp"
#include "Parser.hpp"

int main(int argc, char *argv[])
{
    lex::Lexer scanner;
    links::LinkedList<lex::Token* > tokens;

    std::ifstream ifs(argv[0]);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );

    try{
        tokens = scanner.Scan(content);
    }catch (int x){
        std::cout << "unparsable Char at index " + x;
        return 0;
    }
    tokens.invert();
    parse::Parser parser;
    std::string output = parser.parseStmt(tokens)->toString();
    std::cout << output << std::endl;
    return 0;
}