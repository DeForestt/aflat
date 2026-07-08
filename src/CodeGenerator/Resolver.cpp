#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/AST/Statements/Class.hpp"
#include "Parser/AST/Statements/Union.hpp"

using namespace gen::utils;

namespace gen {
namespace {

void registerModuleGenericTemplates(
    ast::Statement *stmt, ast::Statement *moduleRoot, CodeGenerator &generator,
    const std::string &moduleCwd,
    const std::unordered_map<std::string, std::string> &moduleNamespaceMap,
    asmc::File &output) {
  if (stmt == nullptr)
    return;
  if (auto seq = dynamic_cast<ast::Sequence *>(stmt)) {
    registerModuleGenericTemplates(seq->Statement1, moduleRoot, generator,
                                   moduleCwd, moduleNamespaceMap, output);
    registerModuleGenericTemplates(seq->Statement2, moduleRoot, generator,
                                   moduleCwd, moduleNamespaceMap, output);
    return;
  }

  auto cls = dynamic_cast<ast::Class *>(stmt);
  if (cls == nullptr || cls->genericTypes.empty())
    return;

  auto *copy = dynamic_cast<ast::Class *>(ast::deepCopy(cls));
  if (copy == nullptr)
    return;
  copy->templateModuleRoot = moduleRoot;
  copy->templateModuleCwd = moduleCwd;
  copy->templateNamespaceMap = moduleNamespaceMap;
  output << generator.GenSTMT(copy);
}

} // namespace

Type **CodeGenerator::instantiateGenericClass(
    ast::Class *cls, const std::vector<std::string> &types,
    std::string &newName, asmc::File &OutputFile) {
  auto classStatement = dynamic_cast<ast::Class *>(ast::deepCopy(cls));
  std::unordered_map<std::string, std::string> genericMap;
  newName = classStatement->ident.ident + "<";
  if (types.size() != classStatement->genericTypes.size())
    alert("Generic class " + cls->ident.ident + " requires " +
              std::to_string(classStatement->genericTypes.size()) +
              " template types, but got " + std::to_string(types.size()),
          true, __FILE__, __LINE__);
  for (size_t i = 0; i < types.size(); i++) {
    newName += types[i];
    if (i < types.size() - 1)
      newName += ",";
    genericMap[classStatement->genericTypes[i]] = types[i];
  }
  newName += ">";

  classStatement->replaceTypes(genericMap);
  classStatement->ident.ident = newName;
  classStatement->genericTypes.clear();
  classStatement->hidden =
      true; // we hid the class so all of its functions are private
  Type **result;
  if (typeList()[newName] == nullptr) {
    if (OutputFile.lambdas == nullptr)
      OutputFile.lambdas = new asmc::File;
    OutputFile.hasLambda = true;
    scope::ScopeManager::getInstance()->pushIsolated();
    auto outerIncludedMemo = this->includedMemo();
    auto outerIncludedClasses = this->includedClasses();
    auto outerActiveImports = this->activeImports();
    this->pushEnv();
    this->includedMemo() = outerIncludedMemo;
    this->includedClasses() = outerIncludedClasses;
    this->activeImports() = outerActiveImports;
    auto savedCwd = this->cwd();
    auto savedNameSpaceTable = this->nameSpaceTable();
    if (cls->templateModuleRoot != nullptr) {
      if (!cls->templateModuleCwd.empty())
        this->cwd() = cls->templateModuleCwd;
      for (const auto &[alias, target] : cls->templateNamespaceMap)
        this->nameSpaceTable().insert(alias, target);
      OutputFile.lambdas->operator<<(
          this->ImportsOnly(cls->templateModuleRoot, true));
      registerModuleGenericTemplates(
          cls->templateModuleRoot, cls->templateModuleRoot, *this,
          cls->templateModuleCwd, cls->templateNamespaceMap,
          *OutputFile.lambdas);
    }
    OutputFile.lambdas->operator<<(this->GenSTMT(classStatement));
    result = typeList()[newName];
    this->nameSpaceTable() = savedNameSpaceTable;
    this->cwd() = savedCwd;
    this->popEnv();
    scope::ScopeManager::getInstance()->popIsolated();
  } else {
    result = typeList()[newName];
  }
  return result;
}

std::tuple<std::string, Symbol, bool, asmc::File, Symbol *>
CodeGenerator::resolveSymbol(std::string ident,
                             links::LinkedList<std::string> modList,
                             asmc::File &OutputFile,
                             links::LinkedList<ast::Expr *> indicies,
                             bool internal) {
  asmc::File pops;
  modList.invert();
  modList.reset();

  std::string nsp;
  bool global = false;
  bool owned = false;
  Symbol *sym = scope::ScopeManager::getInstance()->get(ident);
  bool readOnly = sym ? sym->readOnly : false;

  if (sym == nullptr) {
    sym = GlobalSymbolTable().search<std::string>(searchSymbol, ident);
    global = true;
    owned = true; // global symbols are corporatly owned
  }

  if (sym == nullptr && nameSpaceTable().contains(ident)) {
    nsp = nameSpaceTable().get(ident) + ".";
    if (modList.count == 0)
      alert("NameSpace " + ident + " cannot be used as a variable", true,
            __FILE__, __LINE__);
    ident = nsp + modList.shift();

    global = false;
    owned = false;
    sym = scope::ScopeManager::getInstance()->get(ident);
    readOnly = sym ? sym->readOnly : false;
    if (sym == nullptr) {
      sym = GlobalSymbolTable().search<std::string>(searchSymbol, ident);
      global = true;
      owned = true;
    }
  }

  if (sym == nullptr)
    return std::make_tuple("", Symbol(), false, pops, nullptr);

  if (sym->owned) {
    owned = true;
  }
  std::string access = "";
  if (global)
    access = sym->symbol;
  else
    access = '-' + std::to_string(sym->byteMod) + "(%rbp)";
  ast::Type last = sym->type;
  Symbol *modSym = sym;
  const int checkTo = (internal) ? 1 : 0;
  if (modList.trail() > checkTo) {
    asmc::Push *push = new asmc::Push;
    push->logicalLine = logicalLine();
    push->op = registers()["%r14"]->get(asmc::QWord);
    OutputFile.text << push;
    while (modList.trail() > checkTo) {
      Type *type = *this->getType(last.typeName, OutputFile);
      std::string sto = modList.touch();
      if (scope() == type) {
        modSym = type->SymbolTable.search<std::string>(searchSymbol,
                                                       modList.shift());
      } else {
        modSym = type->publicSymbols.search<std::string>(searchSymbol,
                                                         modList.shift());
      };
      if (modSym == nullptr)
        alert("variable not found " + last.typeName + "." + sto, true, __FILE__,
              __LINE__);
      last = modSym->type;
      readOnly = readOnly || modSym->readOnly;
      int tbyte = modSym->byteMod;
      asmc::Mov *mov = new asmc::Mov();
      mov->size = asmc::QWord;
      mov->to = registers()["%r14"]->get(asmc::QWord);
      mov->from = access;
      mov->logicalLine = logicalLine();
      OutputFile.text << mov;
      access = std::to_string(tbyte - (sizeToInt(last.size) * last.arraySize)) +
               '(' + mov->to + ')';
    }

    asmc::Pop *pop = new asmc::Pop;
    pop->op = registers()["%r14"]->get(asmc::QWord);
    pop->logicalLine = logicalLine();
    pops.text << pop;
  };

  modList.invert();
  modList.reset();

  indicies.reset();
  modSym->type.indices.reset();
  Symbol retSym = *modSym;
  if (auto resolvedType = typeList()[retSym.type.typeName]) {
    if (dynamic_cast<Enum *>(*resolvedType) != nullptr) {
      retSym.type.size = asmc::DWord;
      retSym.type.opType = asmc::Hard;
    }
  }
  if (readOnly)
    retSym.mutable_ = false;
  retSym.readOnly = readOnly || modSym->readOnly;

  if (indicies.trail() != 0) {
    this->canAssign(modSym->type, "adr",
                    "the given type {} is not subscriptable");

    if (modSym->type.indices.trail() != indicies.trail())
      alert("invalid index count", true, __FILE__, __LINE__);

    int multiplier = sizeToInt(modSym->type.typeHint->size);

    int count = 1;

    asmc::Mov *zero = new asmc::Mov();
    zero->from = "$0";
    zero->to = registers()["%r12"]->get(asmc::QWord);
    zero->logicalLine = logicalLine();
    zero->size = asmc::QWord;

    OutputFile.text << zero;

    while (indicies.trail() > 0) {
      ast::Expr *index = indicies.shift();
      Expr expr = this->GenExpr(index, OutputFile);
      asmc::Xor *xr = new asmc::Xor();
      xr->op1 = registers()["%r13"]->get(asmc::QWord);
      xr->op2 = registers()["%r13"]->get(asmc::QWord);
      xr->size = asmc::QWord;
      xr->logicalLine = logicalLine();
      OutputFile.text << xr;

      asmc::Mov *mov = new asmc::Mov();
      mov->size = asmc::DWord;
      mov->to = registers()["%r13"]->get(asmc::DWord);
      mov->from = expr.access;
      mov->logicalLine = logicalLine();
      OutputFile.text << mov;

      asmc::Mul *mul = new asmc::Mul();
      mul->op2 = registers()["%r13"]->get(asmc::QWord);
      mul->op1 = '$' + std::to_string(count);
      mul->logicalLine = logicalLine();
      OutputFile.text << mul;

      asmc::Add *add = new asmc::Add();
      add->op1 = registers()["%r13"]->get(asmc::QWord);
      add->op2 = registers()["%r12"]->get(asmc::QWord);
      add->size = asmc::QWord;
      add->logicalLine = logicalLine();

      OutputFile.text << add;
      count = modSym->type.indices.shift();
    };
    asmc::Mul *mul = new asmc::Mul();
    mul->op2 = registers()["%r12"]->get(asmc::QWord);
    mul->op1 = '$' + std::to_string(multiplier);
    mul->size = asmc::QWord;
    mul->logicalLine = logicalLine();

    OutputFile.text << mul;
    asmc::Mov *mov = new asmc::Mov();
    mov->size = asmc::QWord;
    mov->to = registers()["%rdx"]->get(asmc::QWord);
    mov->from = access;
    mov->logicalLine = logicalLine();
    OutputFile.text << mov;

    asmc::Add *add = new asmc::Add();
    add->op1 = registers()["%rdx"]->get(asmc::QWord);
    add->op2 = registers()["%r12"]->get(asmc::QWord);
    add->size = asmc::QWord;
    add->logicalLine = logicalLine();

    OutputFile.text << add;

    access = '(' + registers()["%r12"]->get(asmc::QWord) + ')';
    retSym.type = *retSym.type.typeHint;
  };
  retSym.owned = owned;
  return std::make_tuple(access, retSym, true, pops, modSym);
}

} // namespace gen
