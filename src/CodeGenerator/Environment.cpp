#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"

using namespace gen::utils;

namespace gen {

asmc::File *CodeGenerator::deScope(gen::Symbol &sym) {
  if (sym.sold != -1) return nullptr;
  asmc::File *file = new asmc::File();
  Type **type = this->typeList[sym.type.typeName];
  if (type == nullptr) return nullptr;

  Class *classType = dynamic_cast<Class *>(*type);
  if (classType == nullptr) return nullptr;

  ast::Function *endScope = classType->nameTable["endScope"];
  if (endScope == nullptr) return nullptr;

  asmc::Push *push = new asmc::Push();
  push->logicalLine = this->logicalLine;
  push->op = this->registers["%rax"]->get(asmc::QWord);
  file->text << push;

  ast::Call *callDel = new ast::Call();
  callDel->logicalLine = this->logicalLine;
  callDel->ident = sym.symbol;
  callDel->Args = LinkedList<ast::Expr *>();
  callDel->modList = links::LinkedList<std::string>();
  callDel->modList.push("endScope");
  file->operator<<(this->GenSTMT(callDel));

  asmc::Pop *pop = new asmc::Pop();
  pop->logicalLine = this->logicalLine;
  pop->op = this->registers["%rax"]->get(asmc::QWord);
  file->text << pop;

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
  this->lambdaReturns = "";
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
}

}  // namespace gen
