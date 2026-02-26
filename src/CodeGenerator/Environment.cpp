#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"

using namespace gen::utils;

namespace gen {

asmc::File *CodeGenerator::deScope(gen::Symbol &sym) {
  // if the symbol has been sold (moved) or is being returned do not clean it up
  if (sym.sold != -1 || sym.returned)
    return nullptr;

  // only owned symbols require clean up
  if (!sym.owned)
    return nullptr;

  // primitives do not need to be cleaned up
  if (parse::PRIMITIVE_TYPES.find(sym.type.typeName) !=
      parse::PRIMITIVE_TYPES.end())
    return nullptr;

  auto type = typeList()[sym.type.typeName];
  if (type == nullptr)
    return nullptr;

  if (!(*type)->uniqueType) {
    auto classType = dynamic_cast<Class *>(*type);
    if (classType == nullptr)
      return nullptr;
    auto endScope = classType->nameTable["endScope"];
    if (endScope == nullptr)
      return nullptr;

    auto file = new asmc::File();
    auto push = new asmc::Push();
    push->logicalLine = logicalLine();
    push->op = registers()["%rax"]->get(asmc::QWord);
    file->text << push;

    auto callDel = new ast::Call();
    callDel->logicalLine = logicalLine();
    callDel->ident = sym.symbol;
    callDel->Args = LinkedList<ast::Expr *>();
    callDel->modList = links::LinkedList<std::string>();
    callDel->modList.push("endScope");
    *file << this->GenSTMT(callDel);

    auto pop = new asmc::Pop();
    pop->logicalLine = logicalLine();
    pop->op = registers()["%rax"]->get(asmc::QWord);
    file->text << pop;
    return file;
  }

  auto file = new asmc::File();

  if (auto classType = dynamic_cast<Class *>(*type)) {
    if (auto destructor = classType->nameTable["del"]) {
      auto callDel = new ast::Call();
      callDel->logicalLine = logicalLine();
      callDel->ident = sym.symbol;
      callDel->Args = LinkedList<ast::Expr *>();
      callDel->modList = links::LinkedList<std::string>();
      callDel->modList.push("del");
      *file << this->GenSTMT(callDel);
    }
  }

  if (nameTable()["free"] != nullptr) {
    auto var = new ast::Var();
    var->logicalLine = logicalLine();
    var->Ident = sym.symbol;
    var->modList = LinkedList<std::string>();

    auto freeCall = new ast::Call();
    freeCall->logicalLine = logicalLine();
    freeCall->ident = "free";
    freeCall->Args = LinkedList<ast::Expr *>();
    freeCall->Args.push(var);
    freeCall->modList = LinkedList<std::string>();

    *file << this->GenSTMT(freeCall);
  }

  return file;
}

void CodeGenerator::pushEnv() {
  EnvState state;
  state.SymbolTable = std::move(SymbolTable());
  state.GlobalSymbolTable = std::move(GlobalSymbolTable());
  state.includedMemo = std::move(includedMemo());
  state.includedClasses = std::move(includedClasses());
  state.genericTypeConversions = std::move(genericTypeConversions());
  state.transforms = std::move(transforms());
  state.inFunction = inFunction();
  state.globalScope = globalScope();
  state.lambdaReturns = lambdaReturns();
  state.lambdaSize = lambdaSize();
  state.tempCount = tempCount();
  state.currentFunction = currentFunction();
  state.scope = scope();
  state.returnType = returnType();
  state.matchScope = matchScope();
  envStack().push_back(std::move(state));

  SymbolTable() = links::LinkedList<Symbol>();
  GlobalSymbolTable() = links::LinkedList<Symbol>();
  includedMemo() = HashMap<ast::Statement *>();
  includedClasses() = HashMap<ast::Statement *>();
  genericTypeConversions().clear();
  transforms().clear();
  currentFunction() = nullptr;
  inFunction() = false;
  globalScope() = false;
  scope() = nullptr;
  returnType() = ast::Type();
  lambdaReturns() = "";
  matchScope() = nullptr;
}

void CodeGenerator::popEnv() {
  if (envStack().empty())
    return;
  EnvState state = std::move(envStack().back());
  envStack().pop_back();

  SymbolTable() = std::move(state.SymbolTable);
  GlobalSymbolTable() = std::move(state.GlobalSymbolTable);
  includedMemo() = std::move(state.includedMemo);
  includedClasses() = std::move(state.includedClasses);
  genericTypeConversions() = std::move(state.genericTypeConversions);
  transforms() = std::move(state.transforms);
  inFunction() = state.inFunction;
  globalScope() = state.globalScope;
  lambdaReturns() = state.lambdaReturns;
  lambdaSize() = state.lambdaSize;
  tempCount() = state.tempCount;
  currentFunction() = state.currentFunction;
  scope() = state.scope;
  returnType() = state.returnType;
  matchScope() = state.matchScope;
}

} // namespace gen
