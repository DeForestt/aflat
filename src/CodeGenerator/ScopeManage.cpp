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

int gen::scope::ScopeManager::assign(std::string symbol, ast::Type type,
                                     bool mask, bool mut = true) {
  auto sym = gen::Symbol();

  for (int i = 0; i < this->stack.size(); i++) {
    if (this->stack[i].symbol == symbol && symbol != "") {
      // add an underscore to the front of the symbol
      if (this->stack[i].type.typeName == type.typeName) throw err::Exception (
        "Cannot shadow symbol \"" + symbol + "\" with the same type");
      this->stack[i].symbol = "~" + this->stack[i].symbol;
      this->stack[i].underscores++;
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
  sym.refCount = 0;
  this->stack.push_back(sym);
  this->SStackSize++;

  this->scopeStack.back()++;

  return sym.byteMod;
}

void gen::scope::ScopeManager::pushScope(bool func) { 
  this->scopeStack.push_back(0);
  this->pleading.push_back({this->SStackSize, func});
  if (func) {
    this->SStackSize = 0;
  }
};

void gen::scope::ScopeManager::popScope(CodeGenerator *callback,
                                        asmc::File &OutputFile,
                                        bool fPop = false) {
  int size = this->scopeStack.back();
  for (int i = 0; i < size; i++) {
    if (!fPop) {
      this->SStackSize--;
      if (this->pleading.size() > 0 && this->pleading.back().pleading) {
        this->SStackSize = this->pleading.back().SScopeSize;
      }
    }
    this->stackPos -= sizeToInt(this->stack.back().type.size) *
                      this->stack.back().type.arraySize;
    gen::Symbol sym = this->stack.back();
    if (sym.symbol != ""
    && sym.symbol != "my"
    && sym.symbol.find("lambda") == std::string::npos
    && sym.symbol.find_first_not_of("0123456789") != std::string::npos) {
      // if the symboe has only numberes in it then it is a temp variable
      if (sym.symbol[0] != '~' && sym.symbol[0] != '$') {
        if (sym.refCount < 1 && sym.symbol.substr(0,2) != "__") {
          callback->alert("Symbol \"" + sym.symbol + "\" is assigned but never "
                          "used please consider removing it. If this is a placeholder var prefix with `__`", false);
        };
        if (sym.assignCount < 1 && sym.mutable_) {
          callback->alert("Symbol \"" + sym.symbol + "\" is mutable but never "
                          "assigned please consider making it immutable.", false);
        };
      }
      auto desc = callback->deScope(sym);
      if (desc) {
        OutputFile << *desc;
      }
      // find any symbols that have the same name and remove an underscore
      for (int i = 0; i < this->stack.size(); i++) {
        if (this->stack[i].symbol == sym.symbol && sym.symbol != "") {
          if (this->stack[i].underscores > 0) {
            this->stack[i].symbol = this->stack[i].symbol.substr(1);
            this->stack[i].underscores--;
          }
        }
      }
    }
    this->stack.pop_back();
  }
  if (fPop){
    this->maxStackPos = 0;
    this->SStackSize = 0;
  };
  this->scopeStack.pop_back();
  if (this->pleading.size() > 0) this->pleading.pop_back();
};

void gen::scope::ScopeManager::softPop(CodeGenerator *callback,
                                       asmc::File &OutputFile) {
  int size = (this->pleading.size() > 0 && this->pleading.back().pleading)? this->scopeStack.back() : this->SStackSize;
  int pos = this->stack.size() - 1;

  for (int i = 0; i < size; i++) {
    gen::Symbol sym = this->stack.at(pos);
    if (sym.symbol != "" && sym.symbol != "my") {
      auto desc = callback->deScope(sym);
      if (desc) {
        OutputFile << *desc;
      }
    }
    pos--;
  }
}



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

  for (int i = this->stack.size() - 1; i >= 0; i--) {
    if (this->stack[i].symbol == symbol) {
      if (this->stack[i].usable) { 
        this->stack[i].refCount++;
        return &this->stack[i];
      };
    }
  }

  // search global stack
  for (int i = this->globalStack.size() - 1; i >= 0; i--) {
    if (this->globalStack[i].symbol == symbol) {
      return &this->globalStack[i];
    }
  }

  return nullptr;
};

void gen::scope::ScopeManager::addAssign(std::string symbol) {
  for (int i = this->stack.size() - 1; i >= 0; i--) {
    if (this->stack[i].symbol == symbol) {
      this->stack[i].assignCount++;
      // cancel the ref that was added when getting the symbol
      this->stack[i].refCount--;
    }
  }
};