#include <iostream>
#include <sstream>
#include <fstream>
#include "Scanner.hpp"
#include "LinkedList.hpp"
#include "Parser.hpp"
#include "CodeGenerator.hpp"
#include "ASM.hpp"
#include "Exceptions.hpp"

int main(int argc, char *argv[])
{
    try{
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
    
    gen::CodeGenerator genny;
    ASMC::File file = genny.GenSTMT(Prog);
    file.text.invert();

   
    file.linker.invert();
    while(file.linker.head != nullptr){
        std::cout << file.linker.pop()->toString();
    }

    std::cout << "\n\n.text\n\n";

    while(file.text.count > 0){
        std::cout << file.text.pop()->toString();
    }
    }
    catch(err::Exception e){
        std::cout << std::endl << "Exception: " << e.errorMsg << std::endl << std::endl;
    }
    return 0;
}