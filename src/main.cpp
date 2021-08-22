#include <iostream>
#include <sstream>
#include <fstream>
#include "Scanner.hpp"
#include "LinkedList.hpp"
#include "Parser.hpp"

int main()
{
    lex::Lexer scanner;
    links::LinkedList<lex::Token* > tokens;
    try{
    tokens = scanner.Scan("byte function(){ byte imp; int blip; string slip; }");
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