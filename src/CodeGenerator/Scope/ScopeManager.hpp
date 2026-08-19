#ifndef Scope
#define Scope

#include <memory>
#include <string>
#include <vector>

#include "CodeGenerator/Scope/ScopeTypes.hpp"
#include "CodeGenerator/Types.hpp"

namespace gen {

class CodeGenerator;

namespace scope {

class ScopeManager {
private:
  struct Impl;

  ScopeManager();
  ~ScopeManager();
  static thread_local ScopeManager *instance;
  std::unique_ptr<Impl> impl;

public:
  struct OwnershipState {
    std::vector<int> stackSold;
    std::vector<int> globalStackSold;
  };

  // Singleton Access
  static ScopeManager *getInstance() {
    if (!ScopeManager::instance)
      ScopeManager::instance = new ScopeManager;
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

  OwnershipState captureOwnershipState() const;
  void restoreOwnershipState(const OwnershipState &state);
  void mergeOwnershipStates(const OwnershipState &left,
                            const OwnershipState &right);

  // push a new scope
  void pushScope(bool func);

  // pop a scope
  void popScope(gen::CodeGenerator *callback, asmc::File &OutputFile,
                bool fPop = false);

  void pushIsolated();
  void popIsolated();

  void softPop(gen::CodeGenerator *callback, asmc::File &OutputFile);

  void parentScope(ScopeId scope) const;

  bool contains(ScopeId outer, ScopeId inner) const;

  // Get stack alignment value
  int getStackAlignment();

  ScopeId currentScope() const;

  // reset everything
  void reset();
};

} // namespace scope

} // namespace gen
#endif
