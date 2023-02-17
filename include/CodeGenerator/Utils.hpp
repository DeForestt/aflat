#pragma once
#include "ASM.hpp"
#include "CodeGenerator/CodeGenerator.hpp"

namespace gen::utils {
    bool searchSymbol(gen::Symbol sym, std::string str);
    int sizeToInt(asmc::Size size);
    bool compareFunc(ast::Function F, std::string input);
    ast::Statement* extract(std::string ident, ast::Statement* stmt, std::string id);
    ast::Statement* extract(std::string ident, ast::Statement* stmt);
    void shellStatement(ast::Statement* stmt);
    std::string getLibPath(std::string lib);
}; // namespace gen::utils