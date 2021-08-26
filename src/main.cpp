#include <iostream>
#include <sstream>
#include <fstream>
#include "Scanner.hpp"
#include "LinkedList.hpp"
#include "Parser.hpp"
#include "CodeGenerator.hpp"
#include "ASM.hpp"

int main(int argc, char *argv[])
{
    lex::Lexer scanner;
    links::LinkedList<lex::Token* > tokens;

    std::ifstream ifs(argv[1]);
    std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );

    try{
        tokens = scanner.Scan(content);
    }catch (int x){
        std::cout << " unparsable Char at index " + x;
        return 0;
    }
    tokens.invert();
    parse::Parser parser;

    AST::Statment * Prog = parser.parseStmt(tokens);
    //std::string output = Prog->toString();
    //std::cout << output << std::endl;
    
    gen::CodeGenerator genny;
    ASMC::File file = genny.GenSTMT(Prog);
    //std::cout << "\n\n---------------------------- \n\n";
    file.text.invert();

    std::cout << ".global _start";

    file.linker.invert();
        while(file.linker.count > 0){
        std::cout << file.text.pop()->toString();
    }

    std::cout << ".text";

    std::cout << "_start: \n\tcall main\n\tret\n\n";

    while(file.text.count > 0){
        std::cout << file.text.pop()->toString();
    }

    return 0;
}