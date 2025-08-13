#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"

using namespace gen::utils;

namespace gen {

asmc::File *CodeGenerator::deScope(gen::Symbol &sym) {
  // if the symbol has been sold (moved/returned) do not clean it up
  if (sym.sold != -1) return nullptr;

  // only owned symbols require clean up
  if (!sym.owned) return nullptr;

  // primitives do not need to be cleaned up
  if (parse::PRIMITIVE_TYPES.find(sym.type.typeName) !=
      parse::PRIMITIVE_TYPES.end())
    return nullptr;

  auto type = this->typeList[sym.type.typeName];
  if (type == nullptr) return nullptr;

  if (!(*type)->uniqueType) {
    auto classType = dynamic_cast<Class *>(*type);
    if (classType == nullptr) return nullptr;
    auto endScope = classType->nameTable["endScope"];
    if (endScope == nullptr) return nullptr;

    auto file = new asmc::File();
    auto push = new asmc::Push();
    push->logicalLine = this->logicalLine;
    push->op = this->registers["%rax"]->get(asmc::QWord);
    file->text << push;

    auto callDel = new ast::Call();
    callDel->logicalLine = this->logicalLine;
    callDel->ident = sym.symbol;
    callDel->Args = LinkedList<ast::Expr *>();
    callDel->modList = links::LinkedList<std::string>();
    callDel->modList.push("endScope");
    *file << this->GenSTMT(callDel);

    auto pop = new asmc::Pop();
    pop->logicalLine = this->logicalLine;
    pop->op = this->registers["%rax"]->get(asmc::QWord);
    file->text << pop;
    return file;
  }

  auto file = new asmc::File();

  if (auto classType = dynamic_cast<Class *>(*type)) {
    if (auto destructor = classType->nameTable["del"]) {
      auto callDel = new ast::Call();
      callDel->logicalLine = this->logicalLine;
      callDel->ident = sym.symbol;
      callDel->Args = LinkedList<ast::Expr *>();
      callDel->modList = links::LinkedList<std::string>();
      callDel->modList.push("del");
      *file << this->GenSTMT(callDel);
    }
  }

  if (this->nameTable["free"] != nullptr) {
    auto var = new ast::Var();
    var->logicalLine = this->logicalLine;
    var->Ident = sym.symbol;
    var->modList = LinkedList<std::string>();

    auto freeCall = new ast::Call();
    freeCall->logicalLine = this->logicalLine;
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
  state.SymbolTable = std::move(this->SymbolTable);
  state.GlobalSymbolTable = std::move(this->GlobalSymbolTable);
  state.includedMemo = std::move(this->includedMemo);
  state.includedClasses = std::move(this->includedClasses);
  state.genericTypeConversions = std::move(this->genericTypeConversions);
  state.transforms = std::move(this->transforms);
  state.inFunction = this->inFunction;
  state.globalScope = this->globalScope;
  state.lambdaReturns = this->lambdaReturns;
  state.lambdaSize = this->lambdaSize;
  state.tempCount = this->tempCount;
  state.currentFunction = this->currentFunction;
  state.scope = this->scope;
  state.returnType = this->returnType;
  state.matchScope = this->matchScope;
  this->envStack.push_back(std::move(state));

  this->SymbolTable = links::LinkedList<Symbol>();
  this->GlobalSymbolTable = links::LinkedList<Symbol>();
  this->includedMemo = HashMap<ast::Statement *>();
  this->includedClasses = HashMap<ast::Statement *>();
  this->genericTypeConversions.clear();
  this->transforms.clear();
  this->currentFunction = nullptr;
  this->inFunction = false;
  this->globalScope = false;
  this->scope = nullptr;
  this->returnType = ast::Type();
  this->lambdaReturns = "";
  this->matchScope = nullptr;
}

void CodeGenerator::popEnv() {
  if (this->envStack.empty()) return;
  EnvState state = std::move(this->envStack.back());
  this->envStack.pop_back();

  this->SymbolTable = std::move(state.SymbolTable);
  this->GlobalSymbolTable = std::move(state.GlobalSymbolTable);
  this->includedMemo = std::move(state.includedMemo);
  this->includedClasses = std::move(state.includedClasses);
  this->genericTypeConversions = std::move(state.genericTypeConversions);
  this->transforms = std::move(state.transforms);
  this->inFunction = state.inFunction;
  this->globalScope = state.globalScope;
  this->lambdaReturns = state.lambdaReturns;
  this->lambdaSize = state.lambdaSize;
  this->tempCount = state.tempCount;
  this->currentFunction = state.currentFunction;
  this->scope = state.scope;
  this->returnType = state.returnType;
  this->matchScope = state.matchScope;
}

}  // namespace gen
