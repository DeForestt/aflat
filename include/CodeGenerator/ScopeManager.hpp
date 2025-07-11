#ifndef Scope
#define Scope

#include <vector>

#include "ASM.hpp"
#include "CodeGenerator/CodeGenerator.hpp"

namespace gen {

namespace scope {

class ScopeManager {
 private:
  struct plead {
    int SScopeSize;
    bool pleading;
  };
  struct isoState {
    std::vector<gen::Symbol> stack;
    std::vector<gen::Symbol> globalStack;
    std::vector<plead> pleading;
    int stackPos;
    int maxStackPos;
    std::vector<int> scopeStack;
    int SStackSize;
  };
  ScopeManager();
  ~ScopeManager() = default;
  static ScopeManager *instance;

  // Stack
  std::vector<gen::Symbol> stack;

  // Global Scope
  std::vector<gen::Symbol> globalStack;

  std::vector<plead> pleading;

  // hold the current memory location relative to the rbp
  int stackPos;

  int maxStackPos;

  // scopeStack holds the number of symbols in the current scope
  std::vector<int> scopeStack;
  int SStackSize = 0;

  std::vector<isoState> isolated;

  // int scopeStackPos;

 public:
  // Singleton Access
  static ScopeManager *getInstance() {
    if (!ScopeManager::instance) ScopeManager::instance = new ScopeManager;
    return ScopeManager::instance;
  };

  // cannot be copied
  ScopeManager(ScopeManager const &) = delete;

  // cannot be assigned
  void operator=(ScopeManager const &) = delete;

  // Assign a new symbol and return the byteMod
  int assign(std::string symbol, ast::Type type, bool mask, bool mut = true,
             bool readOnly = false);

  // Add to a symbols assign count
  void addAssign(std::string symbol, const bool get = true);

  // Get a symbol
  gen::Symbol *get(std::string symbol);

  // Get all of the symbols in the current scope as a vector
  std::vector<gen::Symbol> getScope(const bool used);

  // push a new scope
  void pushScope(bool func);

  // pop a scope
  void popScope(gen::CodeGenerator *callback, asmc::File &OutputFile,
                bool fPop = false);

  void pushIsolated();
  void popIsolated();

  void softPop(gen::CodeGenerator *callback, asmc::File &OutputFile);

  // Get stack alignment value
  int getStackAlignment();

  // reset everything
  void reset();
};

}  // namespace scope

}  // namespace gen
#endif