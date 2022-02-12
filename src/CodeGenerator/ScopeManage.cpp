#include "CodeGenerator/ScopeManager.hpp"
#include "Exceptions.hpp"


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
    this->scopeStack.push_back(0);
}

int gen::scope::ScopeManager::assign(std::string symbol, ast::Type type, bool mask, bool isGlobal = false){
    gen::Symbol sym = gen::Symbol();

    // if the symbol is already in the stack, throw an error
    for(int i = 0; i < this->stack.size(); i++){
        if(this->stack[i].symbol == symbol){
            throw err::Exception("Redefinition of symbol: " + symbol);
        }
    }

    sym.byteMod = this->stackPos;
    sym.symbol = symbol;
    sym.type = type;
    sym.mask = mask;
    this->stack.push_back(sym);
    this->stackPos += sizeToInt(type.size) * type.arraySize;

    this->scopeStack.back()++;

    return sym.byteMod;
}

void gen::scope::ScopeManager::pushScope(){
    this->scopeStack.push_back(0);
};

void gen::scope::ScopeManager::popScope(){
    int size = this->scopeStack.back();
    for (int i = 0; i < size; i++){
        this->stack.pop_back();
    }

    this->scopeStack.pop_back();
};

gen::Symbol * gen::scope::ScopeManager::get(std::string symbol){
    for(int i = 0; i < this->stack.size(); i++){
        if(this->stack[i].symbol == symbol){
            return &this->stack[i];
        }
    }

    // search global stack
    for(int i = 0; i < this->globalStack.size(); i++){
        if(this->globalStack[i].symbol == symbol){
            return &this->globalStack[i];
        }
    }
    
    throw err::Exception("Symbol not found: " + symbol);

};