#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Exceptions.hpp"
#include "Parser/AST.hpp"

typedef gen::scope::ScopeManager ScopeManager;

ScopeManager *ScopeManager::instance = nullptr;

ScopeManager::ScopeManager() {
  this->stackPos = 0;
  this->maxStackPos = 0;
  this->scopeStack.push_back(0);
}

void ScopeManager::reset() {
  this->stackPos = 0;
  this->maxStackPos = 0;
  this->scopeStack.clear();
  this->scopeStack.push_back(0);
}

int ScopeManager::assign(std::string symbol, ast::Type type, bool mask,
                         bool mut, bool readOnly) {
  using namespace gen::utils;
  auto sym = gen::Symbol();

  for (int i = 0; i < this->stack.size(); i++) {
    if (this->stack[i].symbol == symbol && symbol != "") {
      // // add an underscore to the front of the symbol
      // if (this->stack[i].type.typeName == type.typeName)
      //   throw err::Exception("Cannot shadow symbol \"" + symbol +
      //                        "\" with the same type");
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
  sym.readOnly = readOnly;
  sym.refCount = 0;
  this->stack.push_back(sym);
  this->SStackSize++;

  this->scopeStack.back()++;

  return sym.byteMod;
}

void ScopeManager::pushScope(bool func) {
  this->scopeStack.push_back(0);
  this->pleading.push_back({this->SStackSize, func});
  if (func) {
    this->SStackSize = 0;
  }
};

std::string removeTildes(const std::string &input) {
  // Find the first character that is not a tilde
  size_t start = input.find_first_not_of('~');

  // If no non-tilde character is found, return an empty string
  if (start == std::string::npos) return "";

  // Create a substring from the first non-tilde character to the end
  return input.substr(start);
}

void ScopeManager::popScope(CodeGenerator *callback, asmc::File &OutputFile,
                            bool fPop) {
  using namespace gen::utils;
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
    if (sym.symbol != "" && sym.symbol.find("lambda") == std::string::npos &&
        sym.symbol.find_first_not_of("0123456789") != std::string::npos) {
      // if the symbol has only numbers in it then it is a temp variable
      if (sym.symbol[0] != '~' && sym.symbol[0] != '$' && !sym.mask) {
        if (sym.refCount < 1 && sym.symbol.substr(0, 2) != "__" &&
            sym.symbol != "my") {
          callback->alert("Symbol \"" + sym.symbol +
                              "\" is assigned but never "
                              "used please consider removing it. If this is a "
                              "placeholder var prefix with `__`",
                          false, __FILE__, __LINE__);
        };
        if (sym.assignCount < 1 && sym.mutable_) {
          callback->alert("Symbol \"" + sym.symbol +
                              "\" is mutable but never "
                              "assigned please consider making it immutable.",
                          false, __FILE__, __LINE__);
        };
      }
      if (!fPop) {
        auto desc = callback->deScope(sym);
        if (desc) {
          OutputFile << *desc;
        }
      }
      // find any symbols that have the same name and remove an underscore
      for (int j = 0; j < this->stack.size(); j++) {
        if (removeTildes(this->stack[j].symbol) == sym.symbol &&
            sym.symbol != "") {
          if (this->stack[j].underscores > 0) {
            this->stack[j].symbol = this->stack[j].symbol.substr(1);
            this->stack[j].underscores--;
          }
        }
      }
    }
    this->stack.pop_back();
  }
  if (fPop) {
    this->maxStackPos = 0;
    this->SStackSize = 0;
  };
  this->scopeStack.pop_back();
  if (this->pleading.size() > 0) this->pleading.pop_back();
};

void ScopeManager::softPop(CodeGenerator *callback, asmc::File &OutputFile) {
  int size = (this->pleading.size() > 0 && this->pleading.back().pleading)
                 ? this->scopeStack.back()
                 : this->SStackSize;
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

int ScopeManager::getStackAlignment() {
  // align the stack
  if (this->maxStackPos < this->stackPos) this->maxStackPos = this->stackPos;
  int align = 16;
  if (this->stack.size() > 0) {
    align = ((this->maxStackPos + 15) / 16) * 16;
  }

  // if (align < 16){
  //     align = 16;
  // }
  return align;
};

gen::Symbol *ScopeManager::get(std::string symbol) {
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

std::vector<gen::Symbol> ScopeManager::getScope(const bool used) {
  std::vector<gen::Symbol> scope;
  for (int i = this->stack.size() - 1; i >= 0; i--) {
    if (this->stack[i].symbol != "") {
      if (used) this->stack[i].refCount++;
      scope.push_back(this->stack[i]);
    }
  }
  return scope;
};

void ScopeManager::addAssign(std::string symbol, bool get) {
  for (int i = this->stack.size() - 1; i >= 0; i--) {
    if (this->stack[i].symbol == symbol) {
      this->stack[i].assignCount++;
      // cancel the ref that was added when getting the symbol
      if (get) this->stack[i].refCount--;
    }
  }
};

void ScopeManager::pushIsolated() {
  isoState state;
  state.stack = this->stack;
  state.globalStack = this->globalStack;
  state.pleading = this->pleading;
  state.stackPos = this->stackPos;
  state.maxStackPos = this->maxStackPos;
  state.scopeStack = this->scopeStack;
  state.SStackSize = this->SStackSize;
  this->isolated.push_back(state);

  this->stack.clear();
  this->stackPos = 0;
  this->maxStackPos = 0;
  this->scopeStack.clear();
  this->scopeStack.push_back(0);
  this->SStackSize = 0;
  this->pleading.clear();
};

void ScopeManager::popIsolated() {
  if (this->isolated.empty()) return;
  isoState state = this->isolated.back();
  this->isolated.pop_back();
  this->stack = state.stack;
  this->globalStack = state.globalStack;
  this->pleading = state.pleading;
  this->stackPos = state.stackPos;
  this->maxStackPos = state.maxStackPos;
  this->scopeStack = state.scopeStack;
  this->SStackSize = state.SStackSize;
};
