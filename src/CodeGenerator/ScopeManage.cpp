#include "CodeGenerator/ScopeManager.hpp"

gen::scope::ScopeManager::ScopeManager(){
    this->stackPos = 0;
}

int gen::scope::ScopeManager::assign(std::string symbol, AST::Type type, bool mask){
    gen::Symbol sym = gen::Symbol();
    sym.byteMod = this->stackPos;
    sym.symbol = symbol;
    sym.type = type;
    sym.mask = mask;
    this->stack.push_back(sym);

    return 0;
}