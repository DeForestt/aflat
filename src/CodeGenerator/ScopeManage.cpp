#include "CodeGenerator/ScopeManager.hpp"
#include "Exceptions.hpp"
#include "Parser/AST.hpp"

gen::scope::ScopeManager *gen::scope::ScopeManager::instance = nullptr;

#pragma region Helper Functions

int sizeToInt(asmc::Size size) {
  switch (size) {
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

gen::scope::ScopeManager::ScopeManager() {
  this->stackPos = 0;
  this->maxStackPos = 0;
  this->scopeStack.push_back(0);
}

void gen::scope::ScopeManager::reset() {
  this->stackPos = 0;
  this->maxStackPos = 0;
  this->scopeStack.clear();
  this->scopeStack.push_back(0);
}

int gen::scope::ScopeManager::assign(std::string symbol, ast::Type type, bool mask, bool mut = true) {
  gen::Symbol sym = gen::Symbol();

  // if the symbol is already in the stack, throw an error
  for (int i = 0; i < this->stack.size(); i++) {
    if (this->stack[i].symbol == symbol && symbol != "") {
      throw err::Exception("Redefinition of symbol: " + symbol);
    }
  }

  this->stackPos += sizeToInt(type.size) * type.arraySize;
  if (this->stackPos > this->maxStackPos) {
    this->maxStackPos = this->stackPos;
  }
  sym.symbol = symbol;
  sym.mask = mask;
  sym.type = type;
  sym.byteMod = this->stackPos;
  sym.mutable_ = mut;
  this->stack.push_back(sym);

  this->scopeStack.back()++;

  return sym.byteMod;
}

void gen::scope::ScopeManager::pushScope() { this->scopeStack.push_back(0); };

void gen::scope::ScopeManager::popScope(CodeGenerator * callback, asmc::File &OutputFile, bool fPop = false) {
  int size = this->scopeStack.back();
  for (int i = 0; i < size; i++) {
    this->stackPos -= sizeToInt(this->stack.back().type.size) *
                      this->stack.back().type.arraySize;
    gen::Symbol sym = this->stack.back();
    if (sym.symbol == ""){
      //OutputFile << callback->deScope(sym);
    }
    this->stack.pop_back();
  }
  if (fPop)
    this->maxStackPos = 0;
  this->scopeStack.pop_back();
};

int gen::scope::ScopeManager::getStackAlignment() {
  // align the stack
  if (this->maxStackPos < this->stackPos)
    this->maxStackPos = this->stackPos;
  int align = 16;
  if (this->stack.size() > 0) {
    align = ((this->maxStackPos + 15) / 16) * 16;
  }

  // if (align < 16){
  //     align = 16;
  // }
  return align;
};

gen::Symbol *gen::scope::ScopeManager::get(std::string symbol) {
  for (int i = 0; i < this->stack.size(); i++) {
    if (this->stack[i].symbol == symbol) {
      return &this->stack[i];
    }
  }

  // search global stack
  for (int i = 0; i < this->globalStack.size(); i++) {
    if (this->globalStack[i].symbol == symbol) {
      return &this->globalStack[i];
    }
  }

  return nullptr;
};