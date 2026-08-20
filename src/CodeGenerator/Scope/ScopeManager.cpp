#include "CodeGenerator/Scope/ScopeManager.hpp"
#include <algorithm>
#include <memory>
#include <unordered_map>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Scope/ScopeTypes.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/AST.hpp"

typedef gen::scope::ScopeManager ScopeManager;

constexpr gen::scope::ScopeId GLOBAL_SCOPE = 0;

struct ScopeManager::Impl {
  struct Plead {
    int SScopeSize;
    bool pleading;
  };

  struct IsolatedState {
    std::vector<gen::Symbol> stack;
    std::vector<gen::Symbol> globalStack;
    std::vector<Plead> pleading;
    int stackPos;
    int maxStackPos;
    std::vector<ScopeId> activeScopes;
    std::vector<int> scopeStack;
    std::unordered_map<ScopeId, ScopeId> cpr; // child parent relation
    int SStackSize;
    ScopeId nextScopeId;
  };

  std::vector<gen::Symbol> stack;
  std::vector<gen::Symbol> globalStack;
  std::vector<Plead> pleading;
  int stackPos = 0;
  int maxStackPos = 0;
  std::vector<int> scopeStack;
  int SStackSize = 0;
  ScopeId nextScopeId = 1;
  std::vector<IsolatedState> isolated;
  std::vector<ScopeId> activeScopes;
  std::unordered_map<ScopeId, ScopeId> scopeParents; // child parent relation
};

thread_local ScopeManager *ScopeManager::instance = nullptr;

ScopeManager::ScopeManager() : impl(std::make_unique<Impl>()) {
  impl->scopeStack.push_back(0);
  impl->activeScopes.push_back(1);
  impl->scopeParents[1] = 0;
}

ScopeManager::~ScopeManager() = default;

void ScopeManager::reset() {
  impl->stack.clear();
  impl->globalStack.clear();
  impl->pleading.clear();
  impl->isolated.clear();
  impl->stackPos = 0;
  impl->maxStackPos = 0;
  impl->scopeStack.clear();
  impl->scopeStack.push_back(0);
  impl->SStackSize = 0;
  impl->activeScopes.clear();
  impl->scopeParents.clear();
  impl->scopeParents[1] = 0;
}

int ScopeManager::assign(std::string symbol, ast::Type type, bool mask,
                         bool mut, bool readOnly) {
  using namespace gen::utils;
  auto sym = gen::Symbol();

  for (int i = 0; i < impl->stack.size(); i++) {
    if (impl->stack[i].symbol == symbol && symbol != "") {
      // // add an underscore to the front of the symbol
      // if (impl->stack[i].type.typeName == type.typeName)
      //   throw err::Exception("Cannot shadow symbol \"" + symbol +
      //                        "\" with the same type");
      impl->stack[i].symbol = "~" + impl->stack[i].symbol;
      impl->stack[i].underscores++;
    }
  }

  impl->stackPos += sizeToInt(type.size) * type.arraySize;
  if (impl->stackPos > impl->maxStackPos) {
    impl->maxStackPos = impl->stackPos;
  }
  sym.symbol = symbol;
  sym.mask = mask;
  sym.type = type;
  sym.byteMod = impl->stackPos;
  sym.mutable_ = mut;
  sym.readOnly = readOnly;
  sym.refCount = 0;
  sym.declarationScope = currentScope();
  impl->stack.push_back(sym);
  impl->SStackSize++;

  impl->scopeStack.back()++;

  return sym.byteMod;
}

void ScopeManager::pushScope(bool func) {
  const auto parent = currentScope();
  const auto id = impl->nextScopeId++;
  impl->scopeParents[id] = parent;
  impl->activeScopes.push_back(id);

  impl->scopeStack.push_back(0);
  impl->pleading.push_back({impl->SStackSize, func});
  if (func) {
    impl->SStackSize = 0;
  }
};

std::string removeTildes(const std::string &input) {
  // Find the first character that is not a tilde
  size_t start = input.find_first_not_of('~');

  // If no non-tilde character is found, return an empty string
  if (start == std::string::npos)
    return "";

  // Create a substring from the first non-tilde character to the end
  return input.substr(start);
}

void ScopeManager::popScope(CodeGenerator *callback, asmc::File &OutputFile,
                            bool fPop) {
  using namespace gen::utils;

  int size = impl->scopeStack.back();
  for (int i = 0; i < size; i++) {
    if (!fPop) {
      impl->SStackSize--;
      if (impl->pleading.size() > 0 && impl->pleading.back().pleading) {
        impl->SStackSize = impl->pleading.back().SScopeSize;
      }
    }
    impl->stackPos -= sizeToInt(impl->stack.back().type.size) *
                      impl->stack.back().type.arraySize;
    gen::Symbol sym = impl->stack.back();
    if (sym.byteMod > impl->maxStackPos) {
      impl->maxStackPos = sym.byteMod;
    }
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
          delete desc;
        }
      }
      // find any symbols that have the same name and remove an underscore
      for (int j = 0; j < impl->stack.size(); j++) {
        if (removeTildes(impl->stack[j].symbol) == sym.symbol &&
            sym.symbol != "") {
          if (impl->stack[j].underscores > 0) {
            impl->stack[j].symbol = impl->stack[j].symbol.substr(1);
            impl->stack[j].underscores--;
          }
        }
      }
    }
    impl->stack.pop_back();
  }
  if (fPop) {
    impl->maxStackPos = 0;
    impl->SStackSize = 0;
  };
  impl->scopeStack.pop_back();
  impl->activeScopes.pop_back();
  if (impl->pleading.size() > 0)
    impl->pleading.pop_back();
};

bool ScopeManager::contains(ScopeId outer, ScopeId inner) const {
  auto current = inner;

  while (true) {
    if (current == outer)
      return true;

    if (current == GLOBAL_SCOPE)
      return false;

    auto parent = impl->scopeParents.find(current);
    if (parent == impl->scopeParents.end())
      return false;

    current = parent->second;
  }
}

void ScopeManager::softPop(CodeGenerator *callback, asmc::File &OutputFile) {
  int size = (impl->pleading.size() > 0 && impl->pleading.back().pleading)
                 ? impl->scopeStack.back()
                 : impl->SStackSize;
  int pos = impl->stack.size() - 1;

  for (int i = 0; i < size; i++) {
    gen::Symbol sym = impl->stack.at(pos);
    if (sym.symbol != "") {
      auto desc = callback->deScope(sym);
      if (desc) {
        OutputFile << *desc;
        delete desc;
      }
    }
    pos--;
  }
}

gen::scope::ScopeId ScopeManager::currentScope() const {
  return impl->activeScopes.empty() ? GLOBAL_SCOPE : impl->activeScopes.back();
}

int ScopeManager::getStackAlignment() {
  // align the stack
  if (impl->maxStackPos < impl->stackPos)
    impl->maxStackPos = impl->stackPos;
  for (const auto &sym : impl->stack) {
    if (sym.byteMod > impl->maxStackPos) {
      impl->maxStackPos = sym.byteMod;
    }
  }
  int align = 16;
  if (impl->stack.size() > 0) {
    align = ((impl->maxStackPos + 15) / 16) * 16;
  }

  // if (align < 16){
  //     align = 16;
  // }
  return align;
};

gen::Symbol *ScopeManager::get(std::string symbol) {
  for (int i = impl->stack.size() - 1; i >= 0; i--) {
    if (impl->stack[i].symbol == symbol) {
      if (impl->stack[i].usable) {
        impl->stack[i].refCount++;
        return &impl->stack[i];
      };
    }
  }

  // search global stack
  for (int i = impl->globalStack.size() - 1; i >= 0; i--) {
    if (impl->globalStack[i].symbol == symbol) {
      return &impl->globalStack[i];
    }
  }

  return nullptr;
};

std::vector<gen::Symbol> ScopeManager::getScope(const bool used) {
  std::vector<gen::Symbol> scope;
  for (int i = impl->stack.size() - 1; i >= 0; i--) {
    if (impl->stack[i].symbol != "") {
      if (used)
        impl->stack[i].refCount++;
      scope.push_back(impl->stack[i]);
    }
  }
  return scope;
};

ScopeManager::OwnershipState ScopeManager::captureOwnershipState() const {
  OwnershipState state;
  state.stackSold.reserve(impl->stack.size());
  for (const auto &symbol : impl->stack)
    state.stackSold.push_back(symbol.sold);
  state.globalStackSold.reserve(impl->globalStack.size());
  for (const auto &symbol : impl->globalStack)
    state.globalStackSold.push_back(symbol.sold);
  return state;
}

void ScopeManager::restoreOwnershipState(const OwnershipState &state) {
  const auto stackCount = std::min(impl->stack.size(), state.stackSold.size());
  for (std::size_t i = 0; i < stackCount; ++i)
    impl->stack[i].sold = state.stackSold[i];

  const auto globalCount =
      std::min(impl->globalStack.size(), state.globalStackSold.size());
  for (std::size_t i = 0; i < globalCount; ++i)
    impl->globalStack[i].sold = state.globalStackSold[i];
}

void ScopeManager::mergeOwnershipStates(const OwnershipState &left,
                                        const OwnershipState &right) {
  const auto stackCount = std::min(
      {impl->stack.size(), left.stackSold.size(), right.stackSold.size()});
  for (std::size_t i = 0; i < stackCount; ++i) {
    impl->stack[i].sold =
        left.stackSold[i] != -1 ? left.stackSold[i] : right.stackSold[i];
  }

  const auto globalCount =
      std::min({impl->globalStack.size(), left.globalStackSold.size(),
                right.globalStackSold.size()});
  for (std::size_t i = 0; i < globalCount; ++i) {
    impl->globalStack[i].sold = left.globalStackSold[i] != -1
                                    ? left.globalStackSold[i]
                                    : right.globalStackSold[i];
  }
}

void ScopeManager::addAssign(std::string symbol, bool get) {
  for (int i = impl->stack.size() - 1; i >= 0; i--) {
    if (impl->stack[i].symbol == symbol) {
      impl->stack[i].assignCount++;
      // cancel the ref that was added when getting the symbol
      if (get)
        impl->stack[i].refCount--;
    }
  }
};

void ScopeManager::pushIsolated() {
  Impl::IsolatedState state;
  state.stack = impl->stack;
  state.globalStack = impl->globalStack;
  state.pleading = impl->pleading;
  state.stackPos = impl->stackPos;
  state.maxStackPos = impl->maxStackPos;
  state.scopeStack = impl->scopeStack;
  state.SStackSize = impl->SStackSize;
  state.activeScopes = impl->activeScopes;
  state.cpr = impl->scopeParents;
  state.nextScopeId = impl->nextScopeId;
  impl->isolated.push_back(state);

  impl->stack.clear();
  impl->stackPos = 0;
  impl->maxStackPos = 0;
  impl->scopeStack.clear();
  impl->scopeStack.push_back(0);
  impl->SStackSize = 0;
  impl->pleading.clear();
  impl->scopeParents.clear();
  impl->nextScopeId = 0;
  impl->activeScopes.clear();
  impl->nextScopeId = 0;
  impl->scopeParents[1] = 0;
};

void ScopeManager::popIsolated() {
  if (impl->isolated.empty())
    return;
  Impl::IsolatedState state = impl->isolated.back();
  impl->isolated.pop_back();
  impl->stack = state.stack;
  impl->globalStack = state.globalStack;
  impl->pleading = state.pleading;
  impl->stackPos = state.stackPos;
  impl->maxStackPos = state.maxStackPos;
  impl->scopeStack = state.scopeStack;
  impl->SStackSize = state.SStackSize;
  impl->activeScopes = state.activeScopes;
  impl->scopeParents = state.cpr;
  impl->nextScopeId = state.nextScopeId;
};
