#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"

using namespace gen::utils;

namespace gen {

Type **CodeGenerator::instantiateGenericClass(
    ast::Class *cls, const std::vector<std::string> &types,
    std::string &newName, asmc::File &OutputFile) {
  auto classStatement = dynamic_cast<ast::Class *>(ast::deepCopy(cls));
  std::unordered_map<std::string, std::string> genericMap;
  newName = classStatement->ident.ident;
  if (types.size() != classStatement->genericTypes.size())
    alert("Generic class " + cls->ident.ident + " requires " +
          std::to_string(classStatement->genericTypes.size()) +
          " template types, but got " + std::to_string(types.size()));
  for (size_t i = 0; i < types.size(); i++) {
    newName += "." + types[i];
    genericMap[classStatement->genericTypes[i]] = types[i];
  }
  classStatement->replaceTypes(genericMap);
  classStatement->ident.ident = newName;
  classStatement->genericTypes.clear();
  classStatement->hidden =
      true;  // we hid the class so all of its functions are private
  Type **result;
  if (this->TypeList[newName] == nullptr) {
    if (OutputFile.lambdas == nullptr) OutputFile.lambdas = new asmc::File;
    OutputFile.hasLambda = true;
    scope::ScopeManager::getInstance()->pushIsolated();
    this->pushEnv();
    OutputFile.lambdas->operator<<(this->GenSTMT(classStatement));
    result = this->typeList[newName];
    this->popEnv();
    scope::ScopeManager::getInstance()->popIsolated();
  } else {
    result = this->typeList[newName];
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
  if (this->nameSpaceTable.contains(ident)) {
    nsp = this->nameSpaceTable.get(ident) + ".";
    if (modList.count == 0)
      alert("NameSpace " + ident + " cannot be used as a variable");
    ident = nsp + modList.shift();
  };

  bool global = false;
  Symbol *sym = scope::ScopeManager::getInstance()->get(ident);

  if (sym == nullptr) {
    sym = this->GlobalSymbolTable.search<std::string>(searchSymbol, ident);
    global = true;
  }
  if (sym == nullptr)
    return std::make_tuple("", Symbol(), false, pops, nullptr);

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
    push->logicalLine = this->logicalLine;
    push->op = this->registers["%r14"]->get(asmc::QWord);
    OutputFile.text << push;
    while (modList.trail() > checkTo) {
      Type type = **this->getType(last.typeName, OutputFile);
      std::string sto = modList.touch();
      if (this->scope == *this->typeList[last.typeName]) {
        modSym =
            type.SymbolTable.search<std::string>(searchSymbol, modList.shift());
      } else {
        modSym = type.publicSymbols.search<std::string>(searchSymbol,
                                                        modList.shift());
      };
      if (modSym == nullptr)
        alert("variable not found " + last.typeName + "." + sto);
      last = modSym->type;
      int tbyte = modSym->byteMod;
      asmc::Mov *mov = new asmc::Mov();
      mov->size = asmc::QWord;
      mov->to = this->registers["%r14"]->get(asmc::QWord);
      mov->from = access;
      mov->logicalLine = this->logicalLine;
      OutputFile.text << mov;
      access = std::to_string(tbyte - (sizeToInt(last.size) * last.arraySize)) +
               '(' + mov->to + ')';
    }

    asmc::Pop *pop = new asmc::Pop;
    pop->op = this->registers["%r14"]->get(asmc::QWord);
    pop->logicalLine = this->logicalLine;
    pops.text << pop;
  };

  modList.invert();
  modList.reset();

  indicies.reset();
  modSym->type.indices.reset();
  Symbol retSym = *modSym;

  if (indicies.trail() != 0) {
    this->canAssign(modSym->type, "adr",
                    "the given type {} is not subscriptable");

    if (modSym->type.indices.trail() != indicies.trail())
      alert("invalid index count");

    int multiplier = sizeToInt(modSym->type.typeHint->size);

    int count = 1;

    asmc::Mov *zero = new asmc::Mov();
    zero->from = "$0";
    zero->to = this->registers["%r12"]->get(asmc::QWord);
    zero->logicalLine = this->logicalLine;
    zero->size = asmc::QWord;

    OutputFile.text << zero;

    while (indicies.trail() > 0) {
      ast::Expr *index = indicies.shift();
      Expr expr = this->GenExpr(index, OutputFile);
      asmc::Xor *xr = new asmc::Xor();
      xr->op1 = this->registers["%r13"]->get(asmc::QWord);
      xr->op2 = this->registers["%r13"]->get(asmc::QWord);
      xr->logicalLine = this->logicalLine;
      OutputFile.text << xr;

      asmc::Mov *mov = new asmc::Mov();
      mov->size = asmc::DWord;
      mov->to = this->registers["%r13"]->get(asmc::DWord);
      mov->from = expr.access;
      mov->logicalLine = this->logicalLine;
      OutputFile.text << mov;

      asmc::Mul *mul = new asmc::Mul();
      mul->op2 = this->registers["%r13"]->get(asmc::QWord);
      mul->op1 = '$' + std::to_string(count);
      mul->logicalLine = this->logicalLine;
      OutputFile.text << mul;

      asmc::Add *add = new asmc::Add();
      add->op1 = this->registers["%r13"]->get(asmc::QWord);
      add->op2 = this->registers["%r12"]->get(asmc::QWord);
      add->size = asmc::QWord;
      add->logicalLine = this->logicalLine;

      OutputFile.text << add;
      count = modSym->type.indices.shift();
    };
    asmc::Mul *mul = new asmc::Mul();
    mul->op2 = this->registers["%r12"]->get(asmc::QWord);
    mul->op1 = '$' + std::to_string(multiplier);
    mul->size = asmc::QWord;
    mul->logicalLine = this->logicalLine;

    OutputFile.text << mul;
    asmc::Mov *mov = new asmc::Mov();
    mov->size = asmc::QWord;
    mov->to = this->registers["%rdx"]->get(asmc::QWord);
    mov->from = access;
    mov->logicalLine = this->logicalLine;
    OutputFile.text << mov;

    asmc::Add *add = new asmc::Add();
    add->op1 = this->registers["%rdx"]->get(asmc::QWord);
    add->op2 = this->registers["%r12"]->get(asmc::QWord);
    add->size = asmc::QWord;
    add->logicalLine = this->logicalLine;

    OutputFile.text << add;

    access = '(' + this->registers["%r12"]->get(asmc::QWord) + ')';
    retSym.type = *retSym.type.typeHint;
  };
  return std::make_tuple(access, retSym, true, pops, modSym);
}

}  // namespace gen
