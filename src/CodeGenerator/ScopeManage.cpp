#include "CodeGenerator/ScopeManager.hpp"


#pragma region Helper Functions

int sizeToInt(asmc::Size size){
    switch(size){
        case asmc::Size::Byte:
            return 1;
        case asmc::Size::Word:
            return 2;
        case asmc::Size::DWord:
            return 4;
        case asmc::Size::QWord:
            return 8;
        default:
            return 0;
    }
}

#pragma endregion

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