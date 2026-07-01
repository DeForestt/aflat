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

  auto methodLabel = [&](ast::Function *method) {
    std::string scopeName =
        method->scopeName != "global" ? method->scopeName : sym.type.typeName;
    return "pub_" + scopeName + "_" + method->ident.ident;
  };

  auto emitObjectCleanup = [&](const std::string &function) {
    auto file = new asmc::File();

    auto pushRax = new asmc::Push();
    pushRax->logicalLine = logicalLine();
    pushRax->op = registers()["%rax"]->get(asmc::QWord);
    file->text << pushRax;

    auto pushRdi = new asmc::Push();
    pushRdi->logicalLine = logicalLine();
    pushRdi->op = registers()["%rdi"]->get(asmc::QWord);
    file->text << pushRdi;

    auto pushRdx = new asmc::Push();
    pushRdx->logicalLine = logicalLine();
    pushRdx->op = registers()["%rdx"]->get(asmc::QWord);
    file->text << pushRdx;

    auto lea = new asmc::Lea();
    lea->logicalLine = logicalLine();
    lea->from = "-" + std::to_string(sym.byteMod) + "(%rbp)";
    lea->to = registers()["%rax"]->get(asmc::QWord);
    file->text << lea;

    auto load = new asmc::Mov();
    load->logicalLine = logicalLine();
    load->size = asmc::QWord;
    load->from = "(" + registers()["%rax"]->get(asmc::QWord) + ")";
    load->to = registers()["%rdi"]->get(asmc::QWord);
    file->text << load;

    auto call = new asmc::Call();
    call->logicalLine = logicalLine();
    call->function = function;
    file->text << call;

    auto popRdx = new asmc::Pop();
    popRdx->logicalLine = logicalLine();
    popRdx->op = registers()["%rdx"]->get(asmc::QWord);
    file->text << popRdx;

    auto popRdi = new asmc::Pop();
    popRdi->logicalLine = logicalLine();
    popRdi->op = registers()["%rdi"]->get(asmc::QWord);
    file->text << popRdi;

    auto popRax = new asmc::Pop();
    popRax->logicalLine = logicalLine();
    popRax->op = registers()["%rax"]->get(asmc::QWord);
    file->text << popRax;

    return file;
  };

  if (!(*type)->uniqueType) {
    auto classType = dynamic_cast<Class *>(*type);
    if (classType == nullptr)
      return nullptr;
    auto endScope = classType->nameTable["endScope"];
    if (endScope == nullptr)
      return nullptr;
    return emitObjectCleanup(methodLabel(endScope));
  }

  auto file = new asmc::File();

  if (auto classType = dynamic_cast<Class *>(*type)) {
    if (auto destructor = classType->nameTable["del"]) {
      file = emitObjectCleanup(methodLabel(destructor));
    }
  }

  if (nameTable()["af_free"] != nullptr) {
    auto freeFile = emitObjectCleanup("af_free");
    *file << *freeFile;
  }

  return file;
}

void CodeGenerator::pushEnv() {
  EnvState state;
  state.SymbolTable = std::move(SymbolTable());
  state.GlobalSymbolTable = std::move(GlobalSymbolTable());
  state.includedMemo = std::move(includedMemo());
  state.includedClasses = std::move(includedClasses());
  state.activeImports = std::move(activeImports());
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
  activeImports().clear();
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
  activeImports() = std::move(state.activeImports);
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
