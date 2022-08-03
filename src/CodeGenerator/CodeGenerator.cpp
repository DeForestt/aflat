#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "PreProcessor.hpp"
#include "Scanner.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <unistd.h>
#pragma region helper functions

bool searchSymbol(gen::Symbol sym, std::string str) {
  if (sym.symbol == str)
    return true;
  else
    return false;
}

std::string getLibPath(std::string lib) {
  char result[200];
  ssize_t count = readlink("/proc/self/exe", result, 200);
  std::string filename = std::string(result, (count > 0) ? count : 0);
  std::string exepath = filename.substr(0, filename.find_last_of("/"));
  std::string libPath = exepath.substr(0, exepath.find_last_of("/")) +
                        "/libraries/std/" + lib + "/";
  return libPath;
}

bool compairFunc(ast::Function F, std::string input) {
  if (input == F.ident.ident) {
    return true;
  }
  return false;
}

bool gen::Type::compair(gen::Type *t, std::string ident) {
  if (ident == t->Ident)
    return true;
  return false;
}

void shellStatment(ast::Statment *stmt) {
  if (dynamic_cast<ast::Sequence *>(stmt)) {
    ast::Sequence *seq = dynamic_cast<ast::Sequence *>(stmt);
    shellStatment(seq->Statment1);
    shellStatment(seq->Statment2);
  } else if (dynamic_cast<ast::Function *>(stmt)) {
    ast::Function *func = dynamic_cast<ast::Function *>(stmt);
    delete func->statment;
    func->statment = nullptr;
  }
}

ast::Statment *extract(std::string ident, ast::Statment *stmt,
                       std::string id = "") {
  // traverse the statment tree and return the statment with the ident
  if (stmt == nullptr)
    return nullptr;
  if (dynamic_cast<ast::Sequence *>(stmt) != nullptr) {
    ast::Sequence *seq = dynamic_cast<ast::Sequence *>(stmt);
    ast::Statment *temp = extract(ident, seq->Statment1, id);
    if (ident != "*" & temp != nullptr) {
      return temp;
    } else if (ident != "*") {
      return extract(ident, seq->Statment2, id);
    } else {
      extract(ident, seq->Statment2, id);
    }
  } else if (dynamic_cast<ast::Class *>(stmt)) {
    ast::Class *cls = dynamic_cast<ast::Class *>(stmt);
    if (cls->ident.ident == ident) {
      shellStatment(cls->statment);
      return stmt;
    }
  } else if (dynamic_cast<ast::Function *>(stmt)) {
    ast::Function *func = dynamic_cast<ast::Function *>(stmt);
    if (func->ident.ident == ident || ident == "*") {
      func->ident.ident = id + '.' + func->ident.ident;
      func->statment = nullptr;
      if (func->scope != ast::Export)
        func->locked = true;
      return func;
    }
  } else
    stmt->locked = true;
  if (ident == "*") {
    return stmt;
  };
  return nullptr;
}
#pragma endregion

int gen::CodeGenerator::getBytes(asmc::Size size) {
  switch (size) {
  case asmc::QWord:
    return 8;
    break;
  case asmc::Word:
    return 2;
    break;
  case asmc::Byte:
    return 1;
    break;
  case asmc::DWord:
    return 4;
    break;
  default:
    alert("Unknown Size");
  }
  return 0;
}

std::string getUUID() {
  std::string uuid = "";
  for (int i = 0; i < 16; i++) {
    uuid += std::to_string(rand() % 10);
  }
  return uuid;
};

void gen::CodeGenerator::alert(std::string message, bool error = true) {
  if (error) {
    std::cout << "Error: ";
    if (this->scope != nullptr) {
      std::cout << "in class " << this->scope->Ident << ": ";
    }
    if (!this->globalScope && this->currentFunction != nullptr) {
      std::cout << "in function " << this->currentFunction->ident.ident << ": ";
    }
    std::cout << message << std::endl;
    exit(1);
  } else {
    std::cout << "Warning: " << message << std::endl;
  }
};

gen::CodeGenerator::CodeGenerator(std::string moduleId) {
  this->registers << asmc::Register("rax", "eax", "ax", "al");
  this->registers << asmc::Register("rcx", "ecx", "cx", "cl");
  this->registers << asmc::Register("rdx", "edx", "dx", "dl");
  this->registers << asmc::Register("rbx", "ebx", "bx", "bl");
  this->registers << asmc::Register("rsi", "esi", "si", "sil");
  this->registers << asmc::Register("rdi", "edi", "di", "dil");
  this->registers << asmc::Register("rsp", "esp", "sp", "spl");
  this->registers << asmc::Register("rbp", "ebp", "bp", "bpl");
  this->registers << asmc::Register("r11", "r11d", "r11w", "r11b");
  this->registers << asmc::Register("r12", "r12d", "r12w", "r12b");
  this->registers << asmc::Register("r13", "r13d", "r13w", "r13b");
  this->registers << asmc::Register("r14", "r14d", "r14w", "r14b");
  this->registers << asmc::Register("r15", "r15d", "r15w", "r15b");
  this->registers << asmc::Register("xmm0", "xmm0", "xmm0", "xmm0");
  this->registers << asmc::Register("xmm1", "xmm1", "xmm1", "xmm1");
  this->registers << asmc::Register("xmm2", "xmm2", "xmm2", "xmm2");
  this->registers << asmc::Register("xmm3", "xmm3", "xmm3", "xmm3");
  this->registers.foo = asmc::Register::compair;
  this->nameTable.foo = compairFunc;
  this->includedMemo = HashMap<ast::Statment *>();
  this->nameSpaceTable = HashMap<std::string>();
  this->globalScope = true;
  this->typeList.foo = gen::Type::compair;
  this->moduleId = moduleId;
  this->scope = nullptr;
}

std::tuple<std::string, gen::Symbol, bool, asmc::File>
gen::CodeGenerator::resolveSymbol(std::string ident,
                                  links::LinkedList<std::string> modList,
                                  asmc::File &OutputFile,
                                  links::LinkedList<ast::Expr *> indicies,
                                  bool internal = false) {
  asmc::File pops;
  modList.invert();
  modList.reset();

  std::string nsp = "";
  if (this->nameSpaceTable.contains(ident)) {
    nsp = this->nameSpaceTable.get(ident) + ".";
    if (modList.count == 0)
      alert("NameSpace " + ident + " cannot be used as a variable");
    ident = nsp + modList.shift();
  };

  bool global = false;
  gen::Symbol *sym = gen::scope::ScopeManager::getInstance()->get(ident);

  if (sym == nullptr) {
    sym = this->GlobalSymbolTable.search<std::string>(searchSymbol, ident);
    global = true;
  }
  if (sym == nullptr)
    return std::make_tuple("", gen::Symbol(), false, pops);

  std::string access = "";
  if (global)
    access = sym->symbol;
  else
    access = '-' + std::to_string(sym->byteMod) + "(%rbp)";
  ast::Type last = sym->type;
  gen::Symbol *modSym = sym;
  const int checkTo = (internal) ? 1 : 0;
  if (modList.trail() > checkTo) {
    // push r14
    asmc::Push *push = new asmc::Push;
    push->op = this->registers["%r14"]->get(asmc::QWord);
    OutputFile.text << push;
    while (modList.trail() > checkTo) {
      if (this->typeList[last.typeName] == nullptr)
        alert("type not found " + last.typeName);
      gen::Type type = **this->typeList[last.typeName];
      std::string sto = modList.touch();
      if (this->scope == *this->typeList[last.typeName]) {
        // if we are scoped to the type seache the symbol in the type symbol
        // table
        modSym =
            type.SymbolTable.search<std::string>(searchSymbol, modList.shift());
      } else {
        // if we are not scoped to the type search the symbol in the public
        // symbol table
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
      OutputFile.text << mov;
      access =
          std::to_string(tbyte - (this->getBytes(last.size) * last.arraySize)) +
          '(' + mov->to + ')';
    }

    // pop r14
    asmc::Pop *pop = new asmc::Pop;
    pop->op = this->registers["%r14"]->get(asmc::QWord);
    pops.text << pop;
  };

  modList.invert();
  modList.reset();

  indicies.reset();
  modSym->type.indecies.reset();
  gen::Symbol retSym = *modSym;

  if (indicies.trail() != 0) {
    this->canAssign(modSym->type, "adr");

    if (modSym->type.indecies.trail() != indicies.trail())
      alert("invalid index count");

    int multiplyer = getBytes(modSym->type.typeHint->size);

    int count = 1;

    asmc::Mov *zero = new asmc::Mov();
    zero->from = "$0";
    zero->to = this->registers["%r12"]->get(asmc::QWord);
    zero->size = asmc::QWord;

    OutputFile.text << zero;

    while (indicies.trail() > 0) {
      // generate the expression
      ast::Expr *index = indicies.shift();
      gen::Expr expr = this->GenExpr(index, OutputFile);
      // clear r13
      asmc::Xor *xr = new asmc::Xor();
      xr->op1 = this->registers["%r13"]->get(asmc::QWord);
      xr->op2 = this->registers["%r13"]->get(asmc::QWord);
      OutputFile.text << xr;

      // move expression access to r13
      asmc::Mov *mov = new asmc::Mov();
      mov->size = asmc::DWord;
      mov->to = this->registers["%r13"]->get(asmc::DWord);
      mov->from = expr.access;

      OutputFile.text << mov;

      // multiply r13 by count
      asmc::Mul *mul = new asmc::Mul();
      mul->op2 = this->registers["%r13"]->get(asmc::QWord);
      mul->op1 = '$' + std::to_string(count);

      OutputFile.text << mul;

      // add r13 to r12
      asmc::Add *add = new asmc::Add();
      add->op1 = this->registers["%r13"]->get(asmc::QWord);
      add->op2 = this->registers["%r12"]->get(asmc::QWord);
      add->size = asmc::QWord;

      OutputFile.text << add;
      count = modSym->type.indecies.shift();
    };
    // multiply r13 by the size of the type
    asmc::Mul *mul = new asmc::Mul();
    mul->op2 = this->registers["%r12"]->get(asmc::QWord);
    mul->op1 = '$' + std::to_string(multiplyer);
    mul->size = asmc::QWord;

    OutputFile.text << mul;
    // load the address of the acces to rdx
    asmc::Mov *mov = new asmc::Mov();
    mov->size = asmc::QWord;
    mov->to = this->registers["%rdx"]->get(asmc::QWord);
    mov->from = access;
    OutputFile.text << mov;

    // add rdx to r13
    asmc::Add *add = new asmc::Add();
    add->op1 = this->registers["%rdx"]->get(asmc::QWord);
    add->op2 = this->registers["%r12"]->get(asmc::QWord);
    add->size = asmc::QWord;

    OutputFile.text << add;

    access = '(' + this->registers["%r12"]->get(asmc::QWord) + ')';
    retSym.type = *retSym.type.typeHint;
  };

  return std::make_tuple(access, retSym, true, pops);
};

bool gen::CodeGenerator::canAssign(ast::Type type, std::string typeName,
                                   bool strict = false) {
  if (type.typeName == typeName)
    return true;
  if (typeName == "--std--flex--function")
    return true;
  if (type.typeName == "int" && typeName == "float")
    return true;
  if (type.typeName == "float" && typeName == "int")
    return true;
  if (type.typeName == "short" && typeName == "number")
    return true;
  if (type.typeName == "int" && typeName == "number")
    return true;
  if (type.typeName == "long" && typeName == "number")
    return true;
  if (type.size == asmc::QWord && typeName == "adr")
    return true;
  if (strict && type.typeName == "adr")
    return true;
  // search the type list for the type
  gen::Type **udef = this->typeList[typeName];
  if (udef != nullptr) {
    if (type.typeName == "adr")
      return true;
    // if the type is a class
    gen::Class *cl = dynamic_cast<gen::Class *>(*udef);
    if (cl != nullptr) {
      gen::Class *parent = cl->parent;

      if (parent != nullptr) {
        if (parent->Ident == type.typeName)
          return true;
      };
    };
  };
  if (!strict)
    alert("Cannot assign type " + type.typeName + " to " + typeName);
  alert("Cannot return type " + typeName + " from " + type.typeName);
  return false;
}

gen::Expr gen::CodeGenerator::prepareCompound(ast::Compound compound,
                                              asmc::File &OutputFile,
                                              bool isDiv = false) {
  asmc::Mov *mov1 = new asmc::Mov();
  asmc::Mov *mov2 = new asmc::Mov();
  std::string r1 = "%edx", r2 = "%rdi";
  // if expr1 op is Float set to the float registers

  gen::Expr expr2 = this->GenExpr(compound.expr2, OutputFile);

  if (expr2.op == asmc::Float) {
    r1 = "%xmm1";
    r2 = "%xmm0";
  }

  mov1->op = expr2.op;
  mov1->to = this->registers[r1]->get(expr2.size);
  mov1->from = expr2.access;
  mov1->size = expr2.size;
  if (!isDiv)
    OutputFile.text << mov1;

  gen::Expr expr1 = this->GenExpr(compound.expr1, OutputFile);
  mov2->op = expr1.op;
  mov2->to = this->registers[r2]->get(expr1.size);
  mov2->from = expr1.access;
  mov2->size = expr1.size;
  OutputFile.text << mov2;

  return expr1;
}

gen::Expr gen::CodeGenerator::genArithmatic(asmc::ArithInst *inst,
                                            ast::Compound compound,
                                            asmc::File &OutputFile) {
  gen::Expr expr = this->prepareCompound(compound, OutputFile);
  gen::Expr output;

  inst->opType = expr.op;
  inst->size = expr.size;

  std::string to1 = this->registers["%rdx"]->get(expr.size);
  std::string to2 = this->registers["%rdi"]->get(expr.size);

  output.access = this->registers["%rax"]->get(expr.size);
  std::string retTo = this->registers["%rdi"]->get(expr.size);

  if (expr.op == asmc::Float) {
    to1 = this->registers["%xmm1"]->get(asmc::DWord);
    to2 = this->registers["%xmm0"]->get(asmc::DWord);
    output.access = "%xmm2";
    output.op = asmc::Float;
  }

  asmc::Mov *mov = new asmc::Mov();
  mov->from = to2;
  mov->to = output.access;
  mov->op = output.op;
  mov->size = expr.size;

  asmc::Pop *pop1 = new asmc::Pop();
  pop1->op = this->registers[to1]->get(asmc::QWord);

  asmc::Pop *pop2 = new asmc::Pop();
  pop2->op = this->registers[to2]->get(asmc::QWord);

  inst->op2 = to2;
  inst->op1 = to1;
  OutputFile.text << inst;
  OutputFile.text << mov;
  output.size = expr.size;
  output.type = expr.type;
  return output;
}

gen::Expr gen::CodeGenerator::GenExpr(ast::Expr *expr, asmc::File &OutputFile,
                                      asmc::Size size = asmc::AUTO) {
  gen::Expr output;
  output.op = asmc::Hard;

  if (dynamic_cast<ast::IntLiteral *>(expr) != nullptr) {
    ast::IntLiteral *intlit = dynamic_cast<ast::IntLiteral *>(expr);

    output.access = '$' + std::to_string(intlit->val);
    output.size = asmc::DWord;
    if (size != asmc::AUTO)
      output.size = size;

    output.type = "number";
  } else if (dynamic_cast<ast::LongLiteral *>(expr) != nullptr) {
    ast::LongLiteral *intlit = dynamic_cast<ast::LongLiteral *>(expr);
    output.type = "long";
    output.access = '$' + std::to_string(intlit->val);
    output.size = asmc::QWord;
  } else if (dynamic_cast<ast::CharLiteral *>(expr) != nullptr) {
    ast::CharLiteral *intlit = dynamic_cast<ast::CharLiteral *>(expr);
    output.type = "char";
    output.access = '$' + std::to_string(intlit->value);
    output.size = asmc::Byte;
  } else if (dynamic_cast<ast::CallExpr *>(expr) != nullptr) {
    ast::CallExpr *exprCall = dynamic_cast<ast::CallExpr *>(expr);
    ast::Call *call = exprCall->call;

    // check if the call ident is a class name
    Type **t = this->typeList[call->ident];
    if (t != nullptr) {
      gen::Class *cl = dynamic_cast<gen::Class *>(*t);
      if (cl != nullptr) {
        // allocate space for the object
        ast::Type type = ast::Type();
        type.typeName = cl->Ident;
        type.size = asmc::Byte;
        type.arraySize = cl->SymbolTable.head->data.byteMod;
        int bMod =
            gen::scope::ScopeManager::getInstance()->assign("", type, false);

        //
        asmc::Lea *lea = new asmc::Lea();
        lea->to = this->registers["%rax"]->get(asmc::QWord);
        lea->from = '-' + std::to_string(bMod) + "(%rbp)";
        // ASMC::Mov * mov = new ASMC::Mov();
        OutputFile.text << lea;
        std::string pointer = registers["%rax"]->get(asmc::QWord);

        // check if the class has a constructor
        ast::Function *init = cl->nameTable["init"];

        if (init == nullptr) {
          output.access = pointer;
          output.size = asmc::QWord;
          output.type = cl->Ident;
        } else {
          // creat the call STMT
          ast::CallExpr *callInit = new ast::CallExpr();
          callInit->call = new ast::Call;
          callInit->call->ident = "init";
          callInit->call->Args = call->Args;
          callInit->call->modList = links::LinkedList<std::string>();
          callInit->call->publify = cl->Ident;
          asmc::Mov *mov = new asmc::Mov();

          asmc::Push *push = new asmc::Push();
          push->op = mov->to = this->intArgs[0].get(asmc::QWord);
          OutputFile.text << push;
          mov->size = asmc::QWord;
          mov->from = this->registers["%eax"]->get(asmc::QWord);
          mov->to = this->intArgs[0].get(asmc::QWord);
          OutputFile.text << mov;
          gen::Expr afterInit = this->GenExpr(callInit, OutputFile);
          output.access = afterInit.access;
          output.size = asmc::QWord;
          output.type = cl->Ident;
        }
      } else {
        alert("Class " + call->ident + " not found");
      }
    } else {
      ast::Function func = this->GenCall(call, OutputFile);
      output.type = func.type.typeName;
      output.size = func.type.size;
      if (size != asmc::AUTO && func.flex)
        output.size = size;
      output.access = this->registers["%rax"]->get(output.size);
    }
  } else if (dynamic_cast<ast::Var *>(expr) != nullptr) {
    ast::Var var = *dynamic_cast<ast::Var *>(expr);

    std::tuple<std::string, gen::Symbol, bool, asmc::File> resolved =
        this->resolveSymbol(var.Ident, var.modList, OutputFile, var.indecies);

    if (std::get<2>(resolved) == false) {
      std::string ident = var.Ident;
      std::string nsp = "";
      var.modList.invert();
      var.modList.reset();
      if (this->nameSpaceTable.contains(var.Ident)) {
        nsp = this->nameSpaceTable.get(ident) + ".";
        if (var.modList.trail() == 0)
          alert("NameSpace " + ident + " cannot be used as a variable");
        ident = nsp + var.modList.shift();
      };

      Type **t = this->typeList[ident];
      if (t != nullptr) {
        Type *type = *t;
        output.size = asmc::DWord;
        output.access =
            '$' + std::to_string(type->SymbolTable.head->data.byteMod);
        output.type = "int";
      } else if (ident == "int") {
        output.size = asmc::DWord;
        output.access = "$4";
        output.type = "int";
      } else if (ident == "char" || ident == "bool" || ident == "byte") {
        output.size = asmc::DWord;
        output.access = "$1";
        output.type = "int";
      } else if (ident == "adr") {
        output.size = asmc::DWord;
        output.access = "$8";
        output.type = "int";
      } else if (var.Ident == "byte") {
        output.size = asmc::DWord;
        output.access = "$1";
        output.type = "int";
      } else if (var.Ident == "float") {
        output.size = asmc::DWord;
        output.access = "$4";
        output.type = "int";
      } else if (var.Ident == "NULL") {
        output.size = asmc::QWord;
        output.access = "$0";
        output.type = "adr";
      } else if (var.Ident == "true") {
        output.size = asmc::Byte;
        output.access = "$1";
        output.type = "bool";
      } else if (var.Ident == "false") {
        output.size = asmc::Byte;
        output.access = "$0";
        output.type = "bool";
      } else if (this->nameTable[ident] != nullptr) {
        output.size = asmc::QWord;
        output.access = '$' + this->nameTable[ident]->ident.ident;
        output.type = "adr";
      } else if (this->scope != nullptr &&
                 this->scope->nameTable[ident] != nullptr) {
        output.size = asmc::QWord;
        output.access = "$pub_" + this->scope->Ident + "_" + ident;
        output.type = "adr";
      } else {
        alert("variable not found " + ident);
      }
      var.modList.invert();
      var.modList.reset();
    } else {
      gen::Symbol sym = std::get<1>(resolved);
      output.size = sym.type.size;
      output.op = sym.type.opType;
      output.type = sym.type.typeName;
      // mov output to r15
      asmc::Mov *mov = new asmc::Mov();
      std::string move2 = (output.op == asmc::Float)
                              ? this->registers["%xmm0"]->get(asmc::QWord)
                              : this->registers["%r15"]->get(output.size);

      mov->to = move2;
      mov->from = std::get<0>(resolved);
      mov->size = output.size;
      mov->op = output.op;
      OutputFile.text << mov;
      output.access = mov->to;
      OutputFile << std::get<3>(resolved);
    }

  } else if (dynamic_cast<ast::Refrence *>(expr) != nullptr) {
    ast::Refrence ref = *dynamic_cast<ast::Refrence *>(expr);

    std::tuple<std::string, gen::Symbol, bool, asmc::File> resolved =
        this->resolveSymbol(ref.Ident, ref.modList, OutputFile,
                            links::LinkedList<ast::Expr *>(), ref.internal);

    asmc::Lea *lea = new asmc::Lea();

    if (std::get<2>(resolved)) {
      lea->from = std::get<0>(resolved);
    } else
      alert("variable not found " + ref.Ident);
    lea->to = this->registers["%rax"]->get(asmc::QWord);

    output.access = registers["%rax"]->get(asmc::QWord);
    output.access = registers["%rax"]->get(asmc::QWord);
    output.size = asmc::QWord;
    output.op = asmc::OpType::Hard;
    output.type = "adr";
    OutputFile.text << lea;
    OutputFile << std::get<3>(resolved);
  } else if (dynamic_cast<ast::StringLiteral *>(expr) != nullptr) {
    ast::StringLiteral str = *dynamic_cast<ast::StringLiteral *>(expr);
    asmc::StringLiteral *strlit = new asmc::StringLiteral();
    asmc::Lable *lable = new asmc::Lable();
    if (this->scope == nullptr)
      lable->lable = ".str" + this->nameTable.head->data.ident.ident +
                     std::to_string(this->lablecount);
    else
      lable->lable = ".str" + scope->Ident + '.' +
                     scope->nameTable.head->data.ident.ident +
                     std::to_string(this->lablecount);
    this->lablecount++;
    strlit->value = str.val;
    OutputFile.data << lable;
    OutputFile.data << strlit;
    output.access = "$" + lable->lable;
    output.size = asmc::QWord;
    output.type = "adr";
  } else if (dynamic_cast<ast::FloatLiteral *>(expr) != nullptr) {
    ast::FloatLiteral *floatlit = dynamic_cast<ast::FloatLiteral *>(expr);
    asmc::FloatLiteral *fltlit = new asmc::FloatLiteral();
    asmc::Lable *lable = new asmc::Lable();
    if (this->scope == nullptr)
      lable->lable = ".float" + this->nameTable.head->data.ident.ident +
                     std::to_string(this->lablecount);
    else
      lable->lable = ".float" + scope->Ident + '.' +
                     scope->nameTable.head->data.ident.ident +
                     std::to_string(this->lablecount);
    this->lablecount++;
    fltlit->value = floatlit->val;

    // move value to the xmm0 register
    asmc::Mov *mov = new asmc::Mov();
    mov->size = asmc::DWord;
    mov->op = asmc::Float;
    mov->to = this->registers["%xmm0"]->get(asmc::DWord);
    mov->from = lable->lable;

    output.op = asmc::Float;
    OutputFile.text << mov;
    OutputFile.data << lable;
    OutputFile.data << fltlit;
    output.access = this->registers["%xmm0"]->get(asmc::DWord);
    output.size = asmc::DWord;
    output.type = "int";
  } else if (dynamic_cast<ast::DeRefence *>(expr)) {

    ast::DeRefence deRef = *dynamic_cast<ast::DeRefence *>(expr);
    gen::Symbol *sym =
        gen::scope::ScopeManager::getInstance()->get(deRef.Ident);

    asmc::Mov *mov = new asmc::Mov();
    asmc::Mov *mov2 = new asmc::Mov();

    mov->size = asmc::QWord;
    mov->from = '-' + std::to_string(sym->byteMod) + "(%rbp)";
    mov->to = this->registers["%rax"]->get(asmc::QWord);

    mov2->from = "(" + this->registers["%rax"]->get(asmc::QWord) + ")";
    mov2->size = deRef.type.size;
    mov2->to = this->registers["%rax"]->get(deRef.type.size);

    output.access = mov2->to;
    output.size = mov2->size;
    output.type = deRef.type.typeName;

    OutputFile.text << mov;
    OutputFile.text << mov2;

  } else if (dynamic_cast<ast::Compound *>(expr) != nullptr) {
    ast::Compound comp = *dynamic_cast<ast::Compound *>(expr);
    asmc::File Dummby = asmc::File();
    ast::Function *opor = nullptr;
    std::string tname = "";
    // gen expr 1 and check if it is a class
    asmc::File dd = asmc::File();
    std::string optn = this->GenExpr(comp.expr1, dd).type;
    gen::Type **type = this->typeList[optn];
    if (type != nullptr) {
      gen::Class *cls = dynamic_cast<gen::Class *>(*type);
      if (cls != nullptr) {
        tname = optn;
        opor = cls->overloadTable[comp.op];
      }
    }

    if (opor != nullptr) {
      ast::CallExpr *call = new ast::CallExpr();
      call->call = new ast::Call;
      call->call->ident = opor->ident.ident;
      call->call->Args = links::LinkedList<ast::Expr *>();
      call->call->Args.push(comp.expr1);
      call->call->Args.push(comp.expr2);
      call->call->publify = tname;

      gen::Expr aftercall = this->GenExpr(call, OutputFile);
      output.access = aftercall.access;
      output.size = aftercall.size;
      output.type = opor->type.typeName;

    } else {
      // push rdi and rdx to stack
      asmc::Push *push1 = new asmc::Push();
      push1->op = this->registers["%rdi"]->get(asmc::QWord);
      OutputFile.text << push1;
      asmc::Push *push2 = new asmc::Push();
      push2->op = this->registers["%rdx"]->get(asmc::QWord);
      OutputFile.text << push2;
      output.op = asmc::Hard;
      output.type = "--std--flex--function";
      switch (comp.op) {
      case ast::Plus: {
        asmc::Add *add = new asmc::Add();
        output = this->genArithmatic(add, comp, OutputFile);
        break;
      }
      case ast::Minus: {
        asmc::Sub *sub = new asmc::Sub();
        output = this->genArithmatic(sub, comp, OutputFile);
        break;
      }
      case ast::AndBit: {
        asmc::And *andBit = new asmc::And();
        output = this->genArithmatic(andBit, comp, OutputFile);
        break;
      }
      case ast::OrBit: {
        asmc::Or *orBit = new asmc::Or();
        output = this->genArithmatic(orBit, comp, OutputFile);
        break;
      }
      case ast::Less: {
        asmc::Sal *andBit = new asmc::Sal();
        gen::Expr expr1 = this->GenExpr(comp.expr1, Dummby);
        gen::Expr expr2 = this->GenExpr(comp.expr2, Dummby);

        this->prepareCompound(comp, OutputFile);

        std::string to1 = this->registers["%cl"]->get(expr1.size);
        std::string to2 = this->registers["%rdi"]->get(expr1.size);
        output.access = this->registers["%rdi"]->get(expr1.size);

        if (expr1.op == asmc::Float) {
          to1 = this->registers["%xmm1"]->get(asmc::DWord);
          to2 = this->registers["%xmm0"]->get(asmc::DWord);
          output.access = "%xmm0";
          output.op = asmc::Float;
        }
        // Move the value from edx to ecx
        asmc::Mov *mov = new asmc::Mov();
        mov->to = to1;
        mov->from = this->registers["%rdx"]->get(expr1.size);
        mov->size = expr1.size;

        OutputFile.text << mov;

        andBit->op2 = to2;
        andBit->op1 = "%cl";
        andBit->size = expr1.size;
        OutputFile.text << andBit;
        output.access = this->registers["%rdi"]->get(expr1.size);

        // move access to rax
        asmc::Mov *mov2 = new asmc::Mov();
        mov2->to = this->registers["%rax"]->get(expr1.size);
        mov2->from = output.access;
        mov2->size = expr1.size;
        OutputFile.text << mov2;
        output.access = this->registers["%rax"]->get(expr1.size);

        break;
      }
      case ast::Great: {
        asmc::Sar *andBit = new asmc::Sar();
        gen::Expr expr1 = this->GenExpr(comp.expr1, Dummby);
        gen::Expr expr2 = this->GenExpr(comp.expr2, Dummby);

        this->prepareCompound(comp, OutputFile);

        std::string to1 = this->registers["%cl"]->get(expr1.size);
        std::string to2 = this->registers["%rdi"]->get(expr1.size);
        output.access = this->registers["%rdi"]->get(expr1.size);

        if (expr1.op == asmc::Float) {
          to1 = this->registers["%xmm1"]->get(asmc::DWord);
          to2 = this->registers["%xmm0"]->get(asmc::DWord);
          output.access = "%xmm0";
          output.op = asmc::Float;
        }

        // Move the value from edx to ecx
        asmc::Mov *mov = new asmc::Mov();
        mov->to = to1;
        mov->from = this->registers["%rdx"]->get(expr1.size);
        mov->size = expr1.size;

        OutputFile.text << mov;

        andBit->op2 = to2;
        andBit->op1 = "%cl";
        andBit->size = expr1.size;

        OutputFile.text << andBit;

        output.size = expr1.size;
        output.type = expr1.type;
        output.access = to2;

        // move access to rax
        asmc::Mov *mov2 = new asmc::Mov();
        mov2->to = this->registers["%rax"]->get(expr1.size);
        mov2->from = output.access;
        mov2->size = expr1.size;
        OutputFile.text << mov2;
        output.access = this->registers["%rax"]->get(expr1.size);
        break;
      }
      case ast::Mul: {
        asmc::Mul *mul = new asmc::Mul();
        output = this->genArithmatic(mul, comp, OutputFile);
        break;
      }
      case ast::Div: {

        asmc::Div *div = new asmc::Div();

        gen::Expr expr1 = this->GenExpr(comp.expr1, Dummby);
        gen::Expr expr2 = this->GenExpr(comp.expr2, Dummby, expr1.size);

        div->op1 = expr2.access;
        div->opType = expr1.op;
        div->size = expr1.size;

        std::string to1 = this->registers["%rdx"]->get(expr1.size);
        std::string to2 = this->registers["%rax"]->get(expr1.size);
        output.access = this->registers["%rax"]->get(expr1.size);

        if (expr1.op == asmc::Float) {
          to1 = this->registers["%xmm1"]->get(asmc::DWord);
          to2 = this->registers["%xmm0"]->get(asmc::DWord);
          output.access = "%xmm0";
          div->op1 = to1;
          div->op2 = to2;
          this->prepareCompound(comp, OutputFile);
          output.op = asmc::Float;
        } else {
          asmc::Mov *mov = new asmc::Mov;
          asmc::Mov *eax = new asmc::Mov;
          expr1 = this->GenExpr(comp.expr1, OutputFile);

          eax->from = expr1.access;
          eax->to = this->registers["%rax"]->get(expr1.size);
          eax->size = expr1.size;
          OutputFile.text << eax;
          mov->op = expr1.op;

          expr2 = this->GenExpr(comp.expr2, OutputFile, expr1.size);
          mov->from = expr2.access;
          mov->to = this->registers["%rcx"]->get(expr1.size);
          mov->size = expr1.size;
          mov->op = expr1.op;
          div->op1 = mov->to;
          output.access = this->registers["%rax"]->get(expr1.size);

          OutputFile.text << mov;
        }

        OutputFile.text << div;
        output.size = expr1.size;
        output.type = expr1.type;
        break;
      }
      case ast::Mod: {
        asmc::Div *div = new asmc::Div();

        this->selectReg = 0;
        gen::Expr expr1 = this->GenExpr(comp.expr1, Dummby);
        this->selectReg = 1;
        gen::Expr expr2 = this->GenExpr(comp.expr2, Dummby);

        div->op1 = expr2.access;

        if (expr1.op == asmc::Float) {
          std::string to1 = this->registers["%xmm1"]->get(asmc::DWord);
          std::string to2 = this->registers["%xmm0"]->get(asmc::DWord);
          output.access = "%xmm1";
          output.op = asmc::Float;
          div->op1 = to1;
          div->op2 = to2;
          this->prepareCompound(comp, OutputFile);
        } else {
          asmc::Mov *mov = new asmc::Mov;
          asmc::Mov *eax = new asmc::Mov;
          expr1 = this->GenExpr(comp.expr1, OutputFile);

          eax->from = expr1.access;
          eax->to = this->registers["%rax"]->get(expr1.size);
          eax->size = expr1.size;
          OutputFile.text << eax;
          mov->op = expr1.op;

          expr2 = this->GenExpr(comp.expr2, OutputFile, expr1.size);
          mov->from = expr2.access;
          mov->to = this->registers["%rcx"]->get(expr1.size);
          mov->size = expr1.size;
          mov->op = expr1.op;
          div->op1 = mov->to;
          output.access = this->registers["%rax"]->get(expr1.size);

          OutputFile.text << mov;
        }

        // Move the value from edx to eax
        asmc::Mov *mov = new asmc::Mov();
        mov->to = output.access;
        mov->from = this->registers["%rdx"]->get(expr1.size);
        mov->size = expr1.size;

        OutputFile.text << div;
        if (expr1.op != asmc::Float)
          OutputFile.text << mov;
        output.size = asmc::DWord;
        output.type = "int";
        break;
      }
      case ast::CompEqu: {
        asmc::Cmp *cmp = new asmc::Cmp();
        gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

        cmp->size = expr1.size;
        cmp->to = this->registers["%rdi"]->get(expr1.size);
        cmp->from = this->registers["%rdx"]->get(expr1.size);

        asmc::Sete *sete = new asmc::Sete();
        sete->op = this->registers["%rax"]->get(asmc::Byte);

        OutputFile.text << cmp;
        OutputFile.text << sete;

        output.size = asmc::Byte;
        output.type = "bool";
        output.access = "%al";
        break;
      }
      case ast::LessCmp: {
        asmc::Cmp *cmp = new asmc::Cmp();
        gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

        cmp->size = expr1.size;
        cmp->to = this->registers["%rdi"]->get(expr1.size);
        cmp->from = this->registers["%rdx"]->get(expr1.size);

        asmc::Setl *setl = new asmc::Setl();
        setl->op = this->registers["%rax"]->get(asmc::Byte);

        OutputFile.text << cmp;
        OutputFile.text << setl;

        output.size = asmc::Byte;
        output.type = "bool";
        output.access = "%al";
        break;
      }
      case ast::GreatCmp: {
        asmc::Cmp *cmp = new asmc::Cmp();
        gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

        cmp->size = expr1.size;
        cmp->to = this->registers["%rdi"]->get(expr1.size);
        cmp->from = this->registers["%rdx"]->get(expr1.size);

        asmc::Setg *setg = new asmc::Setg();
        setg->op = this->registers["%rax"]->get(asmc::Byte);

        OutputFile.text << cmp;
        OutputFile.text << setg;

        output.size = asmc::Byte;
        output.type = "bool";
        output.access = "%al";
        break;
      }
      case ast::Leq: {
        asmc::Cmp *cmp = new asmc::Cmp();
        gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

        cmp->size = expr1.size;
        cmp->to = this->registers["%rdi"]->get(expr1.size);
        cmp->from = this->registers["%rdx"]->get(expr1.size);

        asmc::Setle *setle = new asmc::Setle();
        setle->op = this->registers["%rax"]->get(asmc::Byte);

        OutputFile.text << cmp;
        OutputFile.text << setle;

        output.size = asmc::Byte;
        output.type = "bool";
        output.access = "%al";
        break;
      }
      case ast::Geq: {
        asmc::Cmp *cmp = new asmc::Cmp();
        gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

        cmp->size = expr1.size;
        cmp->to = this->registers["%rdi"]->get(expr1.size);
        cmp->from = this->registers["%rdx"]->get(expr1.size);

        asmc::Setge *setge = new asmc::Setge();
        setge->op = this->registers["%rax"]->get(asmc::Byte);

        OutputFile.text << cmp;
        OutputFile.text << setge;

        output.size = asmc::Byte;
        output.type = "bool";
        output.access = "%al";
        break;
      }
      case ast::NotEqu: {
        asmc::Cmp *cmp = new asmc::Cmp();
        gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

        cmp->size = expr1.size;
        cmp->to = this->registers["%rdi"]->get(expr1.size);
        cmp->from = this->registers["%rdx"]->get(expr1.size);

        asmc::Setne *setne = new asmc::Setne();
        setne->op = this->registers["%rax"]->get(asmc::Byte);

        OutputFile.text << cmp;
        OutputFile.text << setne;

        output.size = asmc::Byte;
        output.type = "bool";
        output.access = "%al";
        break;
      }
      default: {
        alert("Unhandled oporator");
        break;
      }
      }
      // pop rdx and rdi
      asmc::Pop *pop = new asmc::Pop();
      pop->op = this->registers["%rdx"]->get(asmc::QWord);
      OutputFile.text << pop;
      asmc::Pop *pop2 = new asmc::Pop();
      pop2->op = this->registers["%rdi"]->get(asmc::QWord);
      OutputFile.text << pop2;
    }
  } else if (dynamic_cast<ast::Lambda *>(expr) != nullptr) {
    ast::Lambda lambda = *dynamic_cast<ast::Lambda *>(expr);
    ast::Function *func = lambda.function;
    bool inFunc = this->inFunction;
    bool gscop = this->globalScope;

    auto millies = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                       .count();
    std::string id =
        "lambda_" + std::to_string(millies) + "_" + std::to_string(lablecount);
    func->ident.ident = id;
    lablecount++;
    func->scopeName = "global";
    func->isLambda = true;

    if (OutputFile.lambdas == nullptr)
      OutputFile.lambdas = new asmc::File;
    OutputFile.hasLambda = true;

    ast::Type Adr = ast::Type();
    Adr.typeName = "adr";
    Adr.opType = asmc::Hard;
    Adr.size = asmc::QWord;

    func->type = Adr;

    this->nameTable.push(*func);
    ast::Type saveRetType = this->returnType;

    this->returnType.typeName = "--std--flex--function";

    OutputFile.lambdas->operator<<(this->GenSTMT(func));

    this->returnType = saveRetType;

    this->nameTable.pop();

    this->inFunction = inFunc;
    this->globalScope = gscop;

    output.access = "$" + id;
    output.size = asmc::QWord;
    output.type = "adr";
  } else if (dynamic_cast<ast::NewExpr *>(expr) != nullptr) {
    ast::NewExpr newExpr = *dynamic_cast<ast::NewExpr *>(expr);
    ast::Function *malloc = this->nameTable["malloc"];
    if (malloc == nullptr)
      alert("Please import std library in order to use new operator.\n\n -> "
            ".needs <std> \n\n");
    gen::Type **type = this->typeList[newExpr.type.typeName];
    if (type == nullptr)
      alert("Type" + newExpr.type.typeName + " not found");
    // check if the function is a class
    gen::Class *cl = dynamic_cast<gen::Class *>(*type);
    if (cl == nullptr)
      alert("The new operator can only be used with classes. Type " +
            newExpr.type.typeName + " is not a class");
    // check if the class has a constructor
    ast::Function *init = cl->nameTable["init"];

    // first call malloc with the size of the class
    ast::CallExpr *callMalloc = new ast::CallExpr();
    callMalloc->call = new ast::Call;
    callMalloc->call->ident = malloc->ident.ident;
    callMalloc->call->Args = links::LinkedList<ast::Expr *>();
    ast::IntLiteral *size = new ast::IntLiteral();
    size->val = cl->SymbolTable.head->data.byteMod;
    callMalloc->call->Args.push(size);
    gen::Expr afterMalloc = this->GenExpr(callMalloc, OutputFile);

    if (init == nullptr) {
      output.access = afterMalloc.access;
      output.size = asmc::QWord;
      output.type = newExpr.type.typeName;
    } else {
      // creat the call STMT
      ast::CallExpr *callInit = new ast::CallExpr();
      callInit->call = new ast::Call;
      callInit->call->ident = "init";
      callInit->call->Args = newExpr.args;
      callInit->call->modList = links::LinkedList<std::string>();
      callInit->call->publify = cl->Ident;
      asmc::Mov *mov = new asmc::Mov();

      asmc::Push *push = new asmc::Push();
      push->op = mov->to = this->intArgs[0].get(asmc::QWord);
      OutputFile.text << push;
      mov->size = asmc::QWord;
      mov->from = this->registers["%eax"]->get(asmc::QWord);
      mov->to = this->intArgs[0].get(asmc::QWord);
      OutputFile.text << mov;
      gen::Expr afterInit = this->GenExpr(callInit, OutputFile);
      output.access = afterInit.access;
      output.size = asmc::QWord;
      output.type = newExpr.type.typeName;
    };
  } else if (dynamic_cast<ast::ParenExpr *>(expr) != nullptr) {
    ast::ParenExpr parenExpr = *dynamic_cast<ast::ParenExpr *>(expr);
    output = this->GenExpr(parenExpr.expr, OutputFile);
  } else if (dynamic_cast<ast::Not *>(expr) != nullptr) {
    ast::Not no = *dynamic_cast<ast::Not *>(expr);
    gen::Expr expr = this->GenExpr(no.expr, OutputFile);

    asmc::Movzbl *movzbl = new asmc::Movzbl();
    movzbl->from = expr.access;
    movzbl->to = this->registers["%eax"]->get(asmc::DWord);

    asmc::Xor *xr = new asmc::Xor();
    xr->op1 = "$1";
    xr->op2 = this->registers["%eax"]->get(asmc::DWord);

    ast::Type boolType = ast::Type();

    boolType.typeName = "bool";
    boolType.opType = asmc::Hard;
    boolType.size = asmc::Byte;
    this->canAssign(boolType, expr.type);

    OutputFile.text << movzbl;
    OutputFile.text << xr;
    output.size = asmc::Byte;
    output.type = boolType.typeName;
    output.access = this->registers["%eax"]->get(asmc::Byte);

  } else if (dynamic_cast<ast::StructList *>(expr) != nullptr) {
    ast::StructList structList = *dynamic_cast<ast::StructList *>(expr);
    structList.args.invert();
    structList.args.reset();

    // calculate the size of the struct
    int size = 0;
    while (structList.args.trail() > 0) {
      asmc::File tempFile;
      gen::Expr expr = this->GenExpr(structList.args.shift(), tempFile);
      size += this->getBytes(expr.size);
    };
    structList.args.reset();

    auto millies = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                       .count();
    std::string uniqueIdent = std::to_string(millies);
    uniqueIdent = uniqueIdent + getUUID();

    ast::Type structType = ast::Type();
    structType.size = asmc::Byte;
    structType.arraySize = size;
    structType.typeName = "struct";
    // save a symbol
    int bMod = gen::scope::ScopeManager::getInstance()->assign(
        uniqueIdent, structType, false, false);

    // load
    int offset = 0;

    while (structList.args.trail() > 0) {
      ast::Expr *expr = structList.args.shift();
      gen::Expr genExpr = this->GenExpr(expr, OutputFile);
      asmc::Mov *mov = new asmc::Mov();
      mov->from = genExpr.access;
      mov->to = this->registers["%eax"]->get(genExpr.size);
      mov->size = genExpr.size;
      OutputFile.text << mov;
      asmc::Mov *mov2 = new asmc::Mov();
      mov2->from = this->registers["%eax"]->get(genExpr.size);
      mov2->to = "-" + std::to_string(bMod - offset) + "(%rbp)";
      mov2->size = genExpr.size;
      OutputFile.text << mov2;
      offset += this->getBytes(genExpr.size);
    };

    // create a pointer to the struct
    asmc::Lea *lea = new asmc::Lea();
    lea->from = "-" + std::to_string(bMod) + "(%rbp)";
    lea->to = this->registers["%rax"]->get(asmc::QWord);
    OutputFile.text << lea;

    output.access = this->registers["%rax"]->get(asmc::QWord);
    output.size = asmc::QWord;
    output.type = "adr";
  } else {
    this->alert("Unhandled expression");
  }
  return output;
};

links::LinkedList<gen::Symbol>
gen::CodeGenerator::GenTable(ast::Statment *STMT,
                             links::LinkedList<gen::Symbol> &table) {
  if (dynamic_cast<ast::Sequence *>(STMT) != nullptr) {
    ast::Sequence *sequence = dynamic_cast<ast::Sequence *>(STMT);
    this->GenTable(sequence->Statment1, table);
    this->GenTable(sequence->Statment2, table);
  } else if (dynamic_cast<ast::Declare *>(STMT) != nullptr) {
    /*
        movl $0x0, -[SymbolT + size](rdp)
        **also needs to be added to symbol table**
    */

    ast::Declare *arg = dynamic_cast<ast::Declare *>(STMT);
    gen::Symbol symbol;

    int offset = this->getBytes(arg->type.size);

    if (table.search<std::string>(searchSymbol, arg->Ident) != nullptr)
      alert("redefined variable:" + arg->Ident);

    symbol.symbol = arg->Ident;
    if (table.head == nullptr) {
      symbol.byteMod = offset;
    } else {
      symbol.byteMod = table.peek().byteMod + offset;
    }
    symbol.type = arg->type;
    table << symbol;
  } else if (dynamic_cast<ast::DecArr *>(STMT) != nullptr) {
    /*
       movl $0x0, -[SymbolT + size](rdp)
       **also needs to be added to symbol table**
   */
    ast::DecArr *dec = dynamic_cast<ast::DecArr *>(STMT);
    int offset = 0;
    offset = this->getBytes(dec->type.size);

    offset = offset * dec->count;

    if (this->SymbolTable.search<std::string>(searchSymbol, dec->ident) !=
        nullptr)
      alert("redefined veriable:" + dec->ident);

    gen::Symbol Symbol;
    if (this->SymbolTable.head == nullptr) {
      Symbol.byteMod = offset;
    } else {
      Symbol.byteMod = table.head->data.byteMod + offset;
    }
    Symbol.type = dec->type;
    Symbol.symbol = dec->ident;
    table << Symbol;
  }

  return table;
}

void gen::CodeGenerator::GenArgs(ast::Statment *STMT, asmc::File &OutputFile) {
  if (dynamic_cast<ast::Sequence *>(STMT) != nullptr) {
    ast::Sequence *sequence = dynamic_cast<ast::Sequence *>(STMT);
    this->GenArgs(sequence->Statment1, OutputFile);
    this->GenArgs(sequence->Statment2, OutputFile);
  } else if (dynamic_cast<ast::Declare *>(STMT) != nullptr) {
    /*
        movl $0x0, -[SymbolT + size](rdp)
        **also needs to be added to symbol table**
    */
    if (intArgsCounter > 6)
      alert("AFlat compiler cannot handle more than 6 int arguments.");
    ast::Declare *arg = dynamic_cast<ast::Declare *>(STMT);
    asmc::Size size;
    int offset = 0;
    gen::Symbol symbol;
    asmc::Mov *mov = new asmc::Mov();

    links::LinkedList<gen::Symbol> *Table = &this->SymbolTable;

    offset = this->getBytes(arg->type.size);
    size = arg->type.size;
    arg->type.arraySize = 1;
    mov->from = this->intArgs[intArgsCounter].get(arg->type.size);

    int mod = gen::scope::ScopeManager::getInstance()->assign(
        arg->Ident, arg->type, false, arg->mut);

    mov->size = size;
    mov->to = "-" + std::to_string(mod) + +"(%rbp)";
    OutputFile.text << mov;
    intArgsCounter++;
  }
}

ast::Function gen::CodeGenerator::GenCall(ast::Call *call,
                                          asmc::File &OutputFile) {
  std::string mod = "";
  ast::Function *func;
  bool checkArgs = true;
  call->modList.invert();
  call->modList.reset();
  std::string ident = call->ident;

  std::string nsp = "";
  if (this->nameSpaceTable.contains(ident)) {
    nsp = this->nameSpaceTable.get(ident) + ".";
    ident = nsp + call->modList.shift();
  };

  links::LinkedList<gen::Symbol> *Table;
  Table = &this->SymbolTable;
  links::LinkedList<std::string> mods = links::LinkedList<std::string>();
  // first push rdx
  links::LinkedList<std::string> stack;
  asmc::Push *push = new asmc::Push();
  push->op = this->registers["%rdx"]->get(asmc::QWord);
  stack << push->op;
  OutputFile.text << push;

  this->intArgsCounter = 0;
  int argsCounter = 0;
  std::string allMods = "";
  if (call->modList.pos == nullptr) {
    func = this->nameTable[ident];
    if (func == nullptr) {
      gen::Symbol *smbl = gen::scope::ScopeManager::getInstance()->get(ident);
      if (smbl == nullptr)
        smbl = this->GlobalSymbolTable.search<std::string>(searchSymbol, ident);
      if (smbl != nullptr) {
        ast::Var *var = new ast::Var();
        var->Ident = smbl->symbol;
        var->modList = links::LinkedList<std::string>();

        gen::Expr exp1 = this->GenExpr(var, OutputFile);

        asmc::Mov *mov = new asmc::Mov();
        mov->size = exp1.size;
        mov->from = exp1.access;
        mov->to = this->registers["%r11"]->get(exp1.size);
        OutputFile.text << mov;

        func = new ast::Function();
        func->ident.ident = '*' + this->registers["%r11"]->get(exp1.size);
        func->type.arraySize = 0;
        func->type.size = asmc::QWord;
        func->type.typeName = "--std--flex--function";
        func->flex = true;
        checkArgs = false;
      };
    }
  } else {
    std::string pubname;
    gen::Symbol *sym = gen::scope::ScopeManager::getInstance()->get(ident);
    if (sym == nullptr)
      alert("cannot find object: " + ident);
    allMods += sym->symbol + ".";

    ast::Type last = sym->type;
    std::string my = sym->symbol;
    // get the type of the original function
    gen::Type *type = *this->typeList[last.typeName];

    while (call->modList.pos != nullptr) {
      bool addpub = true;
      if (this->typeList[last.typeName] == nullptr)
        alert("type not found " + last.typeName);
      type = *this->typeList[last.typeName];
      gen::Class *cl = dynamic_cast<gen::Class *>(type);
      if (cl != nullptr) {
        pubname = cl->Ident;
        func = cl->nameTable[call->modList.touch()];
        gen::Class *parent = cl->parent;
        if (func == nullptr && parent) {
          func = parent->nameTable[call->modList.touch()];
          if (func != nullptr)
            pubname = parent->Ident;
        }
        if (func == nullptr) {
          // search the class symbol table for a pointer
          std::string id = call->modList.touch();
          mods.push(id);
          sym = cl->SymbolTable.search<std::string>(searchSymbol,
                                                    call->modList.touch());
          if (sym != nullptr && sym->type.typeName == "adr") {
            call->modList.shift();
            ast::Var *var = new ast::Var();
            var->Ident = ident;
            mods.invert();
            var->modList = mods;

            gen::Expr exp1 = this->GenExpr(var, OutputFile);

            asmc::Mov *mov = new asmc::Mov();
            mov->size = exp1.size;
            mov->from = exp1.access;
            mov->to = this->registers["%r11"]->get(exp1.size);
            OutputFile.text << mov;

            func = new ast::Function();
            func->ident.ident = '*' + this->registers["%r11"]->get(exp1.size);
            func->type = sym->type;
            func->type.typeName = "--std--flex--function";
            checkArgs = false;
            func->type.size = asmc::QWord;
            func->flex = true;
            addpub = false;
          }
        } else {
          call->modList.shift();
          call->modList.invert();
          call->modList.reset();
          ast::Refrence *ref = new ast::Refrence();
          ref->Ident = my;
          ref->modList = call->modList;
          ref->internal = true;
          mod = "pub_" + pubname + "_";
          if (!addpub)
            mod = "";
          gen::Expr exp;
          exp = this->GenExpr(ref, OutputFile);
          asmc::Mov *mov = new asmc::Mov();
          asmc::Mov *mov2 = new asmc::Mov();
          asmc::Push *push = new asmc::Push();

          mov->size = exp.size;
          mov2->size = exp.size;

          mov->from = '(' + exp.access + ')';
          mov->to = this->registers["%eax"]->get(exp.size);
          mov2->from = this->registers["%eax"]->get(exp.size);
          mov2->to = this->intArgs[argsCounter].get(exp.size);
          push->op = this->intArgs[argsCounter].get(asmc::QWord);
          stack << this->intArgs[argsCounter].get(asmc::QWord);

          argsCounter++;
          OutputFile.text << push;
          OutputFile.text << mov;
          OutputFile.text << mov2;
          break;
        }
      }
      mods.push(call->modList.shift());
      last = sym->type;
      allMods += sym->symbol + ".";
    };
  };

  if (call->publify != "") {

    func = new ast::Function();
    func->ident.ident = "pub_" + call->publify + "_" + ident;
    func->type.typeName = call->publify;
    func->type.size = asmc::QWord;
    func->scopeName = call->publify;
    if (ident != "init" && ident != "del") {
      // find the function in the class
      gen::Class *cl =
          dynamic_cast<gen::Class *>(*this->typeList[call->publify]);
      if (cl != nullptr) {
        ast::Function *f = cl->nameTable[ident];
        if (f == nullptr) {
          alert("cannot find function: " + ident + " in class " +
                call->publify);
        };
        func->type = f->type;
        func->scope = f->scope;
        func->scopeName = f->scopeName;
        ast::Type t;
        t.typeName = cl->Ident;
        t.size = asmc::QWord;
        func->argTypes = f->argTypes;
        func->argTypes.push_back(t);
      };
    } else {
      argsCounter++;
      stack.push("%rdi");
      // find the class
      gen::Type *type = *this->typeList[call->publify];
      if (type == nullptr)
        alert("type not found " + call->publify);
      gen::Class *cl = dynamic_cast<gen::Class *>(type);
      if (cl == nullptr)
        alert("not a class: " + call->publify);
      ast::Function *f = cl->nameTable[ident];
      if (f == nullptr)
        alert("cannot find function: " + ident + " in " + cl->Ident);
      func->argTypes = f->argTypes;
    }
    mod = "";
  }

  if (func == nullptr)
    alert("Cannot Find Function: " + ident + allMods);

  if (func->scope == ast::Private && func->scopeName != "global") {
    if (this->scope == nullptr)
      alert("attempt to call private function: " + allMods + func->ident.ident +
            " from global scope");
    if (func->scopeName != this->scope->Ident)
      alert("Attempt to call private function " + allMods + func->ident.ident +
            " from wrong scope: " + this->scope->Ident +
            " != " + func->scopeName);
  }

  call->Args.invert();
  call->Args.reset();
  links::LinkedList<ast::Expr *> args;
  int i = 0;
  if (call->Args.trail() < func->req)
    alert("Too few arguments for function: " + ident +
          " expected: " + std::to_string(func->argTypes.size()) +
          " got: " + std::to_string(i + 1));

  while (call->Args.trail() > 0) {
    args.push(call->Args.touch());
    gen::Expr exp = this->GenExpr(call->Args.shift(), OutputFile);
    if (checkArgs) {
      if (i >= func->argTypes.size()) {
        alert("Too many arguments for function: " + ident +
              " expected: " + std::to_string(func->argTypes.size()) +
              " got: " + std::to_string(i + 1));
      };
      canAssign(func->argTypes.at(i), exp.type);
    };
    i++;
    asmc::Mov *mov = new asmc::Mov();
    asmc::Mov *mov2 = new asmc::Mov();
    asmc::Push *push = new asmc::Push();
    mov->size = exp.size;
    mov2->size = exp.size;

    mov->from = exp.access;
    mov->to = this->registers["%eax"]->get(exp.size);
    mov2->from = this->registers["%eax"]->get(exp.size);
    mov2->to = this->intArgs[argsCounter].get(exp.size);
    push->op = this->intArgs[argsCounter].get(asmc::QWord);
    stack << this->intArgs[argsCounter].get(asmc::QWord);

    argsCounter++;
    OutputFile.text << push;
    OutputFile.text << mov;
    OutputFile.text << mov2;
  };

  while (argsCounter < func->argTypes.size()) {
    asmc::Mov *move = new asmc::Mov();
    move->size = asmc::QWord;
    move->from = "$0";
    move->to = this->intArgs[argsCounter].get(asmc::QWord);
    argsCounter++;
    OutputFile.text << move;
  }

  // call->Args = args;
  call->Args.invert();
  asmc::Call *calls = new asmc::Call;

  calls->function = mod + func->ident.ident;
  OutputFile.text << calls;
  // push everything back on the stack
  while (stack.count > 0) {
    asmc::Pop *pop = new asmc::Pop();
    pop->op = stack.pop();
    OutputFile.text << pop;
  }
  intArgsCounter = 0;
  ast::Function ret = *func;
  return ret;
};

asmc::File gen::CodeGenerator::GenSTMT(ast::Statment *STMT) {

  asmc::File OutputFile = asmc::File();

  if (STMT->locked)
    OutputFile.text.push(new asmc::nop());
  else if (dynamic_cast<ast::Sequence *>(STMT) != nullptr) {
    ast::Sequence *sequence = dynamic_cast<ast::Sequence *>(STMT);
    OutputFile << this->GenSTMT(sequence->Statment1);
    OutputFile << this->GenSTMT(sequence->Statment2);
  } else if (dynamic_cast<ast::Function *>(STMT)) {
    /*
        ident:
            push rbp
            mov  rsp, rbp
            this->GenSTMT()
    */

    gen::scope::ScopeManager::getInstance()->pushScope();
    ast::Function *saveFunc = this->currentFunction;
    ast::Function *func = dynamic_cast<ast::Function *>(STMT);
    int saveIntArgs = intArgsCounter;
    bool isLambda = func->isLambda;

    // std::cout << "Generating Function: " << func->ident.ident << " type " <<
    // func->type.typeName << std::endl;
    if (this->scope == nullptr) {
      if (!func->isLambda)
        this->nameTable << *func;
    } else {
      // add the function to the class name table
      if (!func->isLambda)
        func->scopeName = this->scope->Ident;
      this->scope->nameTable << *func;
      // if the function has an overload opperator, add it to the overload table
      if (func->op != ast::None)
        if (!func->isLambda)
          func->scopeName = this->scope->Ident;
      this->scope->overloadTable << *func;
      // if the function is public add it to the public name table
      if (func->scope == ast::Public)
        if (!func->isLambda)
          this->scope->publicNameTable << *func;
    }

    if (func->statment != nullptr) {
      this->currentFunction = func;
      bool saveIn = this->inFunction;
      this->inFunction = true;
      gen::Class *saveScope = this->scope;
      bool saveGlobal = this->globalScope;
      this->globalScope = false;

      asmc::Lable *lable = new asmc::Lable;
      if (this->scope == nullptr || func->isLambda)
        lable->lable = func->ident.ident;
      else
        lable->lable = "pub_" + scope->Ident + "_" + func->ident.ident;
      if (func->scopeName != "global") {
        lable->lable = "pub_" + func->scopeName + "_" + func->ident.ident;
        gen::Type *tscope = *this->typeList[func->scopeName];
        if (tscope == nullptr)
          alert("Failed to locate function Scope");
        if (dynamic_cast<gen::Class *>(tscope) == nullptr)
          alert("Can only scope to  a class");
        this->scope = dynamic_cast<gen::Class *>(tscope);
      }

      asmc::Push *push = new asmc::Push();
      push->op = "%rbp";
      asmc::Mov *mov = new asmc::Mov();
      mov->size = asmc::QWord;
      mov->from = "%rsp";
      mov->to = "%rbp";

      if (func->scope == ast::Export) {
        asmc::LinkTask *link = new asmc::LinkTask();
        link->command = "global";
        link->operand = this->moduleId + '.' + lable->lable;
        OutputFile.linker.push(link);
        asmc::Lable *lable2 = new asmc::Lable();
        lable2->lable = this->moduleId + '.' + lable->lable;
        OutputFile.text << lable2;
      }

      OutputFile.text.push(lable);
      OutputFile.text.push(push);
      OutputFile.text.push(mov);
      // push the callee preserved registers
      asmc::Push *push2 = new asmc::Push();
      push2->op = "%rbx";
      OutputFile.text.push(push2);
      asmc::Push *push3 = new asmc::Push();

      int AlignmentLoc = OutputFile.text.count;
      this->intArgsCounter = 0;
      this->returnType = func->type;
      asmc::LinkTask *link = new asmc::LinkTask();
      link->command = "global";
      link->operand = lable->lable;

      if (this->scope != nullptr && !func->isLambda) {
        // add the opject to the arguments of the function
        int offset = this->getBytes(asmc::QWord);
        int size = asmc::QWord;
        gen::Symbol symbol;
        asmc::Mov *movy = new asmc::Mov();
        movy->from = this->intArgs[intArgsCounter].get(asmc::QWord);

        symbol.symbol = "my";
        symbol.mutable_ = false;

        auto ty = ast::Type();
        ty.typeName = scope->Ident;
        ty.size = asmc::QWord;
        symbol.type = ty;

        int byteMod =
            gen::scope::ScopeManager::getInstance()->assign("my", ty, false);

        movy->size = asmc::QWord;
        movy->to = "-" + std::to_string(byteMod) + +"(%rbp)";
        OutputFile.text << movy;
        this->intArgsCounter++;
      };
      this->GenArgs(func->args, OutputFile);
      if (!isLambda && func->scope == ast::Public)
        OutputFile.linker.push(link);

      // if the function is 'init' and scope is a class, add the default value
      if (func->ident.ident == "init" && this->scope != nullptr) {
        // add all of the default values from the scopes list
        for (ast::DecAssign it : this->scope->defaultValues) {
          ast::Assign assign = ast::Assign();
          assign.Ident = ("my");
          assign.override = true;
          assign.expr = it.expr;
          assign.modList = LinkedList<std::string>();
          assign.modList.push(it.declare->Ident);
          OutputFile << this->GenSTMT(&assign);
        }
      }

      asmc::File file = this->GenSTMT(func->statment);
      // check if the last statement is a return statement
      if (file.text.count > 0) {
        asmc::Instruction *last = file.text.peek();
        while (dynamic_cast<asmc::nop *>(last) != nullptr) {
          file.text.pop();
          last = file.text.peek();
        };
        if (dynamic_cast<asmc::Return *>(last) == nullptr) {
          // if the function name is init then we need to alert to return 'my'
          if (func->ident.ident == "init") {
            gen::Symbol *my =
                gen::scope::ScopeManager::getInstance()->get("my");
            asmc::Mov *mov = new asmc::Mov();
            mov->size = asmc::QWord;
            mov->from = '-' + std::to_string(my->byteMod) + "(%rbp)";
            ;
            mov->to = this->registers["%rax"]->get(asmc::QWord);
            // pop rbx
            asmc::Pop *pop = new asmc::Pop();
            pop->op = "%rbx";
            file.text.push(pop);
            file.text.push(mov);
          }
          asmc::Return *ret = new asmc::Return();
          file.text.push(ret);
        }
      } else {
        asmc::Pop *pop = new asmc::Pop();
        pop->op = "%rbx";
        file.text.push(pop);
        asmc::Return *ret = new asmc::Return();
        file.text.push(ret);
      }
      OutputFile << file;

      asmc::Subq *sub = new asmc::Subq;
      sub->op1 =
          "$" +
          std::to_string(
              gen::scope::ScopeManager::getInstance()->getStackAlignment());
      sub->op2 = this->registers["%rsp"]->get(asmc::QWord);
      OutputFile.text.insert(sub, AlignmentLoc + 1);

      this->scope = saveScope;
      this->globalScope = saveGlobal;
      this->inFunction = saveIn;
    }

    this->intArgsCounter = saveIntArgs;
    this->currentFunction = saveFunc;

    gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile, true);
  } else if (dynamic_cast<ast::Declare *>(STMT) != nullptr) {
    /*
        movl $0x0, -[SymbolT + size](rdp)
        **also needs to be added to symbol table**
    */

    ast::Declare *dec = dynamic_cast<ast::Declare *>(STMT);
    int offset = this->getBytes(dec->type.size);
    links::LinkedList<gen::Symbol> *Table;

    if (!this->globalScope) {
      // if the there  is no scope use the scope manager otherwise use the scope
      if (this->scope == nullptr || this->inFunction) {
        gen::scope::ScopeManager::getInstance()->assign(dec->Ident, dec->type,
                                                        dec->mask, dec->mut);
      } else {
        // add the symbol to the class symbol table
        Table = &this->scope->SymbolTable;

        if (Table->search<std::string>(searchSymbol, dec->Ident) != nullptr)
          alert("redefined veriable: " + dec->Ident);
        gen::Symbol Symbol;
        if (Table->head == nullptr) {
          Symbol.byteMod = offset;
        } else {
          Symbol.byteMod = Table->head->data.byteMod + offset;
        }
        Symbol.type = dec->type;
        Symbol.symbol = dec->Ident;
        Symbol.mutable_ = dec->mut;
        Table->push(Symbol);
        // if the symbol is public add it to the public symbol table
        if (dec->scope == ast::Public && this->scope != nullptr)
          this->scope->publicSymbols.push(Symbol);
      };
    } else {
      Table = &this->GlobalSymbolTable;
      asmc::LinkTask *var = new asmc::LinkTask();
      asmc::Lable *lable = new asmc::Lable();
      if (Table->search<std::string>(searchSymbol, dec->Ident) != nullptr)
        alert("redefined global veriable: " + dec->Ident);

      lable->lable = dec->Ident;
      if (dec->type.size = asmc::QWord) {
        var->command = "quad";
      };
      gen::Symbol Symbol;

      Symbol.type = dec->type;
      Symbol.symbol = dec->Ident;
      Symbol.mutable_ = dec->mut;
      OutputFile.bss << lable;
      OutputFile.bss << var;
      Table->push(Symbol);
    }

  } else if (dynamic_cast<ast::DecArr *>(STMT) != nullptr) {
    /*
    movl $0x0, -[SymbolT + size](rdp)
    **also needs to be added to symbol table**
    */
    ast::DecArr *dec = dynamic_cast<ast::DecArr *>(STMT);
    int offset = this->getBytes(dec->type.size);

    int index = 1;
    dec->indices.reset();
    links::LinkedList<int> typeHolder;
    while (dec->indices.pos != nullptr) {
      ast::IntLiteral *lit =
          dynamic_cast<ast::IntLiteral *>(dec->indices.shift());
      if (lit == nullptr)
        alert("array index must be an integer");
      index *= lit->val;
      typeHolder.push(lit->val);
    }
    typeHolder.invert();

    offset *= index;

    ast::Type type = dec->type;
    type.arraySize = index;
    dec->type.arraySize = index;
    links::LinkedList<gen::Symbol> *Table;
    if (this->scope == nullptr || this->inFunction) {
      int bMod = gen::scope::ScopeManager::getInstance()->assign(
          "." + dec->ident, type, false);
      // create a pointer to the array
      ast::Type adr;
      adr.arraySize = 1;
      adr.opType = asmc::Hard;
      adr.typeName = "adr";
      adr.typeHint = &dec->type;
      adr.size = asmc::QWord;
      adr.indecies = typeHolder;
      ast::Refrence *ref = new ast::Refrence();
      ref->Ident = "." + dec->ident;

      ast::DecAssign *assign = new ast::DecAssign();
      assign->declare = &ast::Declare();
      assign->declare->Ident = dec->ident;
      assign->declare->type = adr;
      assign->expr = ref;
      assign->mute = dec->mut;
      assign->declare->scope = dec->scope;

      OutputFile << this->GenSTMT(assign);

    } else {
      Table = &this->scope->SymbolTable;

      if (Table->search<std::string>(searchSymbol, dec->ident) != nullptr)
        alert("redefined veriable:" + dec->ident);

      gen::Symbol Symbol;

      if (Table->head == nullptr) {
        Symbol.byteMod = offset;
      } else {
        Symbol.byteMod = Table->head->data.byteMod + offset;
      }
      Symbol.type = dec->type;
      Symbol.symbol = "." + dec->ident;
      Table->push(Symbol);

      // create a pointer to the array
      ast::Type adr;
      adr.arraySize = 1;
      adr.opType = asmc::Hard;
      adr.typeName = "adr";
      adr.typeHint = &dec->type;
      adr.indecies = typeHolder;
      adr.size = asmc::QWord;
      ast::Refrence *ref = new ast::Refrence();
      ref->Ident = "my";
      ref->modList.push("." + dec->ident);

      ast::DecAssign *assign = new ast::DecAssign();
      assign->declare = &ast::Declare();
      assign->declare->Ident = dec->ident;
      assign->declare->type = adr;
      assign->expr = ref;
      assign->mute = dec->mut;
      assign->declare->scope = dec->scope;
      OutputFile << this->GenSTMT(assign);
    }

  } else if (dynamic_cast<ast::DecAssign *>(STMT) != nullptr) {
    /*
        movl $0x0, -[SymbolT + size](rdp)
        **also needs to be added to symbol table**
    */
    ast::DecAssign *decAssign = dynamic_cast<ast::DecAssign *>(STMT);
    ast::Declare *dec = decAssign->declare;
    int offset = this->getBytes(dec->type.size);

    links::LinkedList<gen::Symbol> *Table;
    Table = &this->SymbolTable;

    if (!this->globalScope) {
      if (this->scope == nullptr || this->inFunction) {
        int byteMod = gen::scope::ScopeManager::getInstance()->assign(
            dec->Ident, dec->type, dec->mask, decAssign->mute);

        asmc::Mov *mov = new asmc::Mov();
        gen::Expr expr =
            this->GenExpr(decAssign->expr, OutputFile, dec->type.size);

        this->canAssign(dec->type, expr.type);

        asmc::Mov *mov2 = new asmc::Mov();
        mov2->size = dec->type.size;
        mov2->from = expr.access;
        if (expr.op == asmc::Float)
          mov2->to = this->registers["%xmm0"]->get(expr.size);
        else
          mov2->to = this->registers["%rbx"]->get(dec->type.size);

        mov->op = expr.op;

        mov2->op = expr.op;
        mov->size = dec->type.size;
        mov->from = this->registers["%rbx"]->get(dec->type.size);
        mov->to = "-" + std::to_string(byteMod) + "(%rbp)";

        mov->from = mov2->to;

        OutputFile.text << mov2;
        OutputFile.text << mov;
      } else {
        // add the decAssign to the class default list
        this->scope->defaultValues.push_back(*decAssign);
        // genorate the declare
        dec->mut = decAssign->mute;
        OutputFile << this->GenSTMT(dec);
      }
    } else {
      gen::Symbol Symbol;

      Symbol.type = dec->type;
      Symbol.symbol = dec->Ident;
      Symbol.mutable_ = decAssign->mute;
      Table = &this->GlobalSymbolTable;

      asmc::LinkTask *var = new asmc::LinkTask();
      asmc::Lable *lable = new asmc::Lable();

      lable->lable = dec->Ident;
      if (dec->type.size = asmc::QWord) {
        var->command = "quad";
      }
      gen::Expr exp = this->GenExpr(decAssign->expr, OutputFile);
      this->canAssign(dec->type, exp.type);
      var->operand = exp.access.erase(0, 1);
      Symbol.type.opType = exp.op;
      OutputFile.data << lable;
      OutputFile.data << var;
      if (Table->search<std::string>(searchSymbol, dec->Ident) != nullptr)
        alert("redefined veriable:" + dec->Ident);
      Table->push(Symbol);
    };

  } else if (dynamic_cast<ast::DecAssignArr *>(STMT)) {
    ast::DecAssignArr *decAssign = dynamic_cast<ast::DecAssignArr *>(STMT);
    ast::DecArr *dec = decAssign->declare;
    ast::Type adr;
    adr.arraySize = 1;
    dec->indices.reset();
    while (dec->indices.pos != nullptr) {
      ast::IntLiteral *lit =
          dynamic_cast<ast::IntLiteral *>(dec->indices.shift());
      if (lit == nullptr)
        alert("array index must be an integer");
      adr.indecies.push(lit->val);
    }
    adr.indecies.invert();
    adr.opType = asmc::Hard;
    adr.size = asmc::QWord;
    adr.typeName = "adr";
    adr.typeHint = &dec->type;

    ast::DecAssign *assign = new ast::DecAssign();
    assign->declare = new ast::Declare();
    assign->declare->Ident = dec->ident;
    assign->declare->type = adr;
    assign->expr = decAssign->expr;
    assign->mute = decAssign->mute;
    assign->declare->scope = dec->scope;
    OutputFile << this->GenSTMT(assign);
  } else if (dynamic_cast<ast::Return *>(STMT) != nullptr) {
    /*
        mov [this.GenExpr(ret.value)]
        pop rbp
        ret
    */

    ast::Return *ret = dynamic_cast<ast::Return *>(STMT);

    asmc::Mov *mov = new asmc::Mov();

    gen::Expr from = this->GenExpr(ret->expr, OutputFile);
    std::string move2 = (from.op == asmc::Float)
                            ? this->registers["%xmm0"]->get(from.size)
                            : this->registers["%rax"]->get(from.size);
    mov->from = from.access;
    mov->to = move2;
    mov->size = from.size;
    mov->op = from.op;
    OutputFile.text << mov;
    this->canAssign(this->returnType, from.type, true);
    asmc::Return *re = new asmc::Return();
    OutputFile.text << re;

  } else if (dynamic_cast<ast::Assign *>(STMT) != nullptr) {
    ast::Assign *assign = dynamic_cast<ast::Assign *>(STMT);
    bool global = false;

    links::LinkedList<gen::Symbol> *Table = &this->SymbolTable;

    std::tuple<std::string, gen::Symbol, bool, asmc::File> resolved =
        this->resolveSymbol(assign->Ident, assign->modList, OutputFile,
                            assign->indices);

    if (!std::get<2>(resolved)) {
      alert("undefined veriable:" + assign->Ident);
    }

    Symbol *symbol = &std::get<1>(resolved);

    Symbol *fin = symbol;

    // check if the symbol is a class
    gen::Type **t = this->typeList[symbol->type.typeName];
    if (t != nullptr && assign->modList.count == 0) {
      gen::Class *cl = dynamic_cast<gen::Class *>(*t);
      if (cl != nullptr) {
        // check if the class has an overloaded operator =
        ast::Function *func = cl->overloadTable[ast::Equ];
        if (func != nullptr) {
          // call the overloaded operator =
          ast::Var *v = new ast::Var();
          v->Ident = assign->Ident;
          v->modList = assign->modList;
          ast::Call *call = new ast::Call();
          call->ident = func->ident.ident;
          call->modList = assign->modList;
          call->Args.push(v);
          call->Args.push(assign->expr);
          call->publify = cl->Ident;
          ast::CallExpr *callExpr = new ast::CallExpr();
          callExpr->call = call;
          assign->expr = callExpr;
        };
      }
    }

    asmc::Mov *mov = new asmc::Mov();
    asmc::Mov *mov2 = new asmc::Mov();
    gen::Expr expr = this->GenExpr(assign->expr, OutputFile, symbol->type.size);
    mov->op = expr.op;
    mov2->op = expr.op;
    mov->size = expr.size;
    mov2->size = expr.size;
    mov2->from = expr.access;

    if (expr.op == asmc::Float)
      mov2->to = this->registers["%xmm0"]->get(expr.size);
    else
      mov2->to = this->registers["%rbx"]->get(expr.size);
    mov->from = mov2->to;

    assign->modList.invert();
    int tbyte = 0;

    asmc::Size size;
    std::string output = std::get<0>(resolved);
    if (assign->refrence == true) {
      asmc::Mov *m1 = new asmc::Mov;
      m1->from = output;
      m1->size = asmc::QWord;
      m1->to = this->registers["%eax"]->get(asmc::QWord);
      mov->to = "(" + this->registers["%eax"]->get(asmc::QWord) + ")";
      OutputFile.text << m1;
    } else {
      this->canAssign(symbol->type, expr.type);
      mov->to = output;
    };

    if (!assign->refrence && !fin->mutable_ && !assign->override) {
      alert("cannot assign to const " + fin->symbol);
    }

    OutputFile.text << mov2;
    OutputFile.text << mov;
    OutputFile << std::get<3>(resolved);
  } else if (dynamic_cast<ast::Call *>(STMT) != nullptr) {
    ast::Call *call = dynamic_cast<ast::Call *>(STMT);
    this->GenCall(call, OutputFile);
  } else if (dynamic_cast<ast::Push *>(STMT) != nullptr) {
    ast::Push *push = dynamic_cast<ast::Push *>(STMT);
    asmc::Mov *count = new asmc::Mov;

    count->size = asmc::QWord;
    count->to = this->registers["%rdx"]->get(count->size);
    count->from = "$1";
    asmc::Mov *pointer = new asmc::Mov;
    pointer->size = asmc::QWord;
    pointer->to = this->registers["%rsi"]->get(pointer->size);
    pointer->from = this->GenExpr(push->expr, OutputFile).access;

    asmc::Mov *callnum = new asmc::Mov;
    callnum->size = asmc::QWord;
    callnum->to = this->registers["%rax"]->get(callnum->size);
    callnum->from = "$1";

    asmc::Mov *rdi = new asmc::Mov;
    rdi->size = asmc::QWord;
    rdi->from = "$1";
    rdi->to = this->registers["%rdi"]->get(rdi->size);

    OutputFile.text << rdi;
    OutputFile.text << pointer;
    OutputFile.text << count;
    OutputFile.text << callnum;

    OutputFile.text << new asmc::SysCall;
  } else if (dynamic_cast<ast::Pull *>(STMT) != nullptr) {
    ast::Pull *pull = dynamic_cast<ast::Pull *>(STMT);
    asmc::Mov *count = new asmc::Mov;
    count->size = asmc::QWord;
    count->to = this->registers["%rdx"]->get(count->size);
    count->from = "$1";
    asmc::Mov *pointer = new asmc::Mov;
    pointer->size = asmc::QWord;
    pointer->to = this->registers["%rsi"]->get(pointer->size);
    pointer->from = this->GenExpr(pull->expr, OutputFile).access;
    asmc::Mov *callnum = new asmc::Mov;
    callnum->size = asmc::QWord;
    callnum->to = this->registers["%rax"]->get(callnum->size);
    callnum->from = "$0";
    asmc::Mov *rdi = new asmc::Mov;
    rdi->size = asmc::QWord;
    rdi->from = "$1";
    rdi->to = this->registers["%rdi"]->get(rdi->size);

    OutputFile.text << rdi;
    OutputFile.text << pointer;
    OutputFile.text << count;
    OutputFile.text << callnum;

    OutputFile.text << new asmc::SysCall;
  } else if (dynamic_cast<ast::If *>(STMT) != nullptr) {
    // push a new scope
    gen::scope::ScopeManager::getInstance()->pushScope();
    ast::If ifStmt = *dynamic_cast<ast::If *>(STMT);

    asmc::Lable *lable1 = new asmc::Lable();
    lable1->lable = ".L" + this->nameTable.head->data.ident.ident +
                    std::to_string(this->lablecount);
    this->lablecount++;

    gen::Expr expr = this->GenExpr(ifStmt.expr, OutputFile);

    ast::Type t = ast::Type();
    t.typeName = "bool";
    t.size = asmc::Byte;
    this->canAssign(t, expr.type);

    asmc::Mov *mov1 = new asmc::Mov();

    mov1->size = asmc::Byte;

    mov1->from = expr.access;
    ;

    mov1->to = this->registers["%rax"]->get(mov1->size);

    asmc::Cmp *cmp = new asmc::Cmp();
    asmc::Je *je = new asmc::Je();

    cmp->from = "$0";
    cmp->to = mov1->to;
    cmp->size = expr.size;

    je->to = lable1->lable;

    OutputFile.text << mov1;
    OutputFile.text << cmp;
    OutputFile.text << je;
    OutputFile << this->GenSTMT(ifStmt.statment);
    if (ifStmt.elseStatment != nullptr) {
      asmc::Lable *end = new asmc::Lable();
      end->lable = ".L" + this->nameTable.head->data.ident.ident +
                   std::to_string(this->lablecount);
      this->lablecount++;
      asmc::Jmp *jmp = new asmc::Jmp();
      jmp->to = end->lable;
      OutputFile.text << jmp;
      OutputFile.text << lable1;

      gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile);
      gen::scope::ScopeManager::getInstance()->pushScope();
      OutputFile << this->GenSTMT(ifStmt.elseStatment);
      OutputFile.text << end;
    } else
      OutputFile.text << lable1;

    gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile);
  } else if (dynamic_cast<ast::While *>(STMT) != nullptr) {
    gen::scope::ScopeManager::getInstance()->pushScope();
    ast::While *loop = dynamic_cast<ast::While *>(STMT);

    asmc::Lable *lable1 = new asmc::Lable();
    lable1->lable = ".L" + this->nameTable.head->data.ident.ident +
                    std::to_string(this->lablecount);
    this->lablecount++;

    asmc::Lable *lable2 = new asmc::Lable();
    lable2->lable = ".L" + this->nameTable.head->data.ident.ident +
                    std::to_string(this->lablecount);
    this->lablecount++;

    asmc::Jmp *jmp = new asmc::Jmp();
    jmp->to = lable2->lable;
    OutputFile.text << jmp;

    OutputFile.text << lable1;

    OutputFile << this->GenSTMT(loop->stmt);

    OutputFile.text << lable2;

    gen::Expr expr = this->GenExpr(loop->expr, OutputFile);

    ast::Type t = ast::Type();
    t.typeName = "bool";
    t.size = asmc::Byte;
    this->canAssign(t, expr.type);

    asmc::Mov *mov = new asmc::Mov();

    mov->size = expr.size;

    mov->from = expr.access;

    mov->to = this->registers["%eax"]->get(mov->size);

    asmc::Cmp *cmp = new asmc::Cmp();
    asmc::Je *je = new asmc::Je();

    cmp->from = "$1";
    cmp->to = mov->to;
    cmp->size = expr.size;
    je->to = lable1->lable;

    OutputFile.text << mov;
    OutputFile.text << cmp;
    gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile);
    OutputFile.text << je;

  } else if (dynamic_cast<ast::For *>(STMT) != nullptr) {
    gen::scope::ScopeManager::getInstance()->pushScope();
    ast::For *loop = dynamic_cast<ast::For *>(STMT);

    asmc::Lable *lable1 = new asmc::Lable();
    lable1->lable = ".L" + this->nameTable.head->data.ident.ident +
                    std::to_string(this->lablecount);
    this->lablecount++;

    asmc::Lable *lable2 = new asmc::Lable();
    lable2->lable = ".L" + this->nameTable.head->data.ident.ident +
                    std::to_string(this->lablecount);
    this->lablecount++;
    OutputFile << this->GenSTMT(loop->declare);
    asmc::Jmp *jmp = new asmc::Jmp();
    jmp->to = lable2->lable;
    OutputFile.text << jmp;

    OutputFile.text << lable1;

    OutputFile << this->GenSTMT(loop->Run);
    OutputFile << this->GenSTMT(loop->increment);

    OutputFile.text << lable2;

    gen::Expr expr = this->GenExpr(loop->expr, OutputFile);

    ast::Type t = ast::Type();
    t.typeName = "bool";
    t.size = asmc::Byte;
    this->canAssign(t, expr.type);

    asmc::Mov *mov = new asmc::Mov();

    mov->size = expr.size;

    mov->from = expr.access;

    mov->to = this->registers["%eax"]->get(mov->size);

    asmc::Cmp *cmp = new asmc::Cmp();
    asmc::Je *je = new asmc::Je();
    je->to = lable1->lable;

    cmp->from = "$1";
    cmp->to = mov->to;
    cmp->size = expr.size;

    OutputFile.text << mov;
    OutputFile.text << cmp;
    OutputFile.text << je;

    gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile);
  } else if (dynamic_cast<ast::UDeffType *>(STMT) != nullptr) {
    ast::UDeffType *udef = dynamic_cast<ast::UDeffType *>(STMT);
    gen::Type *type = new gen::Type();
    bool saveScope = this->globalScope;
    this->globalScope = false;
    type->Ident = udef->ident.ident;
    type->SymbolTable = this->GenTable(udef->statment, type->SymbolTable);
    this->typeList.push(type);
    this->globalScope = saveScope;
  } else if (dynamic_cast<ast::Class *>(STMT) != nullptr) {
    ast::Class *deff = dynamic_cast<ast::Class *>(STMT);
    gen::Class *type = new gen::Class();
    bool saveScope = this->globalScope;
    this->globalScope = false;
    type->Ident = deff->ident.ident;
    type->nameTable.foo = compairFunc;
    this->scope = type;
    type->overloadTable.foo = [](ast::Function func, ast::Op op) {
      if (func.op == op) {
        return true;
      }
      return false;
    };
    type->SymbolTable;
    this->typeList.push(type);
    // write any signed contracts
    if (deff->base != "") {
      gen::Type **T = this->typeList[deff->base];
      if (T != nullptr) {
        gen::Class *base = dynamic_cast<gen::Class *>(*T);
        asmc::File contractFile;
        if (base != nullptr) {
          // check if the base class has a contract
          if (base->contract == nullptr)
            err::Exception("Base class does not have a contract");
          // if my contact is not nullptr stitch it with the base
          if (deff->contract != nullptr) {
            ast::Sequence *seq = new ast::Sequence();
            seq->Statment1 = deff->contract;
            seq->Statment2 = base->contract;
            type->contract = seq;
            type->parent = base;
            contractFile = this->GenSTMT(seq);
            OutputFile << contractFile;
          } else {
            type->contract = base->contract;
            type->parent = base;
            contractFile = this->GenSTMT(base->contract);
            OutputFile << contractFile;
            type->contract = deff->contract;
          }
        } else
          alert("Base class is not a class");
      } else
        alert("Base class not found");
    } else if (deff->contract != nullptr) {
      asmc::File contractFile = this->GenSTMT(deff->contract);
      OutputFile << contractFile;
      type->contract = deff->contract;
    }
    asmc::File file = this->GenSTMT(deff->statment);
    if (extract("init", deff->statment) == nullptr &&
        this->scope->defaultValues.size() > 0) {
      ast::Function *func = new ast::Function();
      ast::Return *ret = new ast::Return();
      ast::Var *var = new ast::Var();
      var->Ident = "my";
      var->modList = links::LinkedList<std::string>();
      ret->expr = var;
      func->ident.ident = "init";
      func->scope = ast::Private;
      func->statment = ret;
      func->args = nullptr;
      ast::Type t;
      t.typeName = "adr";
      t.size = asmc::QWord;
      func->type = t;
      file << this->GenSTMT(func);
    }
    OutputFile << file;
    this->globalScope = saveScope;
    this->scope = nullptr;
  } else if (dynamic_cast<ast::Inc *>(STMT) != nullptr) {
    ast::Inc *inc = dynamic_cast<ast::Inc *>(STMT);
    gen::Symbol *sym = gen::scope::ScopeManager::getInstance()->get(inc->ident);
    if (sym == nullptr)
      this->alert("Identifier not found to increment");
    this->canAssign(sym->type, "int");

    asmc::Add *add = new asmc::Add();
    add->op1 = "$1";
    add->op2 = "-" + std::to_string(sym->byteMod) + "(%rbp)";
    OutputFile.text << add;
  } else if (dynamic_cast<ast::Dec *>(STMT) != nullptr) {
    ast::Dec *inc = dynamic_cast<ast::Dec *>(STMT);
    gen::Symbol *sym = gen::scope::ScopeManager::getInstance()->get(inc->ident);
    if (sym == nullptr)
      this->alert("Identifier not found to increment");
    this->canAssign(sym->type, "int");

    asmc::Sub *sub = new asmc::Sub();
    sub->op1 = "$-1";
    sub->op2 = "-" + std::to_string(sym->byteMod) + "(%rbp)";
    OutputFile.text << sub;
  } else if (dynamic_cast<ast::Import *>(STMT) != nullptr) {
    ast::Import *imp = dynamic_cast<ast::Import *>(STMT);
    bool standard = false;
    if (imp->path.find("./") == std::string::npos) {
      imp->path = getLibPath("src") + imp->path;
    };

    if (imp->path.substr(imp->path.length() - 3, 3) != ".af") {
      imp->path = imp->path + ".af";
    };

    std::string id = imp->path.substr(imp->path.find_last_of("/") + 1);
    // remove the .af extension
    id = id.substr(0, id.find_last_of("."));
    ast::Statment *added;
    if (this->includedMemo.contains(imp->path))
      added = this->includedMemo.get(imp->path);
    else {
      // scan the file
      std::ifstream file(imp->path);
      std::string text = std::string((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());
      lex::Lexer l = lex::Lexer();
      PreProcessor pp = PreProcessor();
      auto tokens = l.Scan(pp.PreProcess(text, getLibPath("head")));
      tokens.invert();
      // parse the file
      parse::Parser p = parse::Parser();
      ast::Statment *statment = p.parseStmt(tokens);
      added = statment;
    }
    for (std::string ident : imp->imports) {
      ast::Statment *statment = extract(ident, added, id);
      if (statment == nullptr)
        this->alert("Identifier " + ident + " not found to import");
      OutputFile << this->GenSTMT(statment);
    }
    this->nameSpaceTable.insert(imp->nameSpace, id);
  } else if (dynamic_cast<ast::Delete *>(STMT) != nullptr) {
    ast::Delete *del = dynamic_cast<ast::Delete *>(STMT);

    std::tuple<std::string, gen::Symbol, bool, asmc::File> resolved =
        this->resolveSymbol(del->ident, del->modList, OutputFile,
                            links::LinkedList<ast::Expr *>());
    if (!std::get<2>(resolved))
      this->alert("Identifier " + del->ident + " not found to delete");

    gen::Symbol *sym = &std::get<1>(resolved);

    ast::Function *free = this->nameTable["free"];
    if (free == nullptr)
      alert("Please import std library in order to use delete operator.\n\n -> "
            ".needs <std> \n\n");

    gen::Type **type = this->typeList[sym->type.typeName];
    if (type != nullptr) {
      gen::Class *classType = dynamic_cast<gen::Class *>(*type);
      if (classType != nullptr) {

        // check if the class has a destructor
        ast::Function *destructor = classType->nameTable["del"];

        if (destructor != nullptr) {
          ast::Call *call = new ast::Call();
          call->ident = del->ident;
          call->modList = LinkedList<std::string>();
          call->modList.push("del");
          call->Args = LinkedList<ast::Expr *>();

          OutputFile << this->GenSTMT(call);
        };
      }
    };
    // call free
    ast::Var *var = new ast::Var();
    var->Ident = del->ident;
    var->modList = LinkedList<std::string>();

    ast::Call *freeCall = new ast::Call();
    freeCall->ident = "free";
    freeCall->modList = LinkedList<std::string>();
    freeCall->Args = LinkedList<ast::Expr *>();
    freeCall->Args.push(var);
    OutputFile << this->GenSTMT(freeCall);
    OutputFile << std::get<3>(resolved);
  } else {
    OutputFile.text.push(new asmc::nop());
  }

  return OutputFile;
}

asmc::File gen::CodeGenerator::deScope(gen::Symbol sym) {
  asmc::File file;
  gen::Type **type = this->typeList[sym.type.typeName];
  if (type == nullptr)
    this->alert("Type" + sym.type.typeName + " not found to delete");

  gen::Class *classType = dynamic_cast<gen::Class *>(*type);
  if (classType == nullptr)
    this->alert("Type" + sym.type.typeName + " is not a class");

  // check if the class has a destructor
  ast::Function *destructor = classType->nameTable["del"];

  // get the address of the object to delete
  asmc::Lea *lea = new asmc::Lea();
  lea->to = this->registers["%rax"]->get(asmc::QWord);
  lea->from = '-' + std::to_string(sym.byteMod) + "(%rbp)";
  // ASMC::Mov * mov = new ASMC::Mov();
  file.text << lea;
  std::string pointer = registers["%rax"]->get(asmc::QWord);

  // call the destructor
  if (destructor != nullptr) {
    ast::Call *callDel = new ast::Call();
    callDel->ident = "del";
    callDel->Args = LinkedList<ast::Expr *>();
    callDel->modList = links::LinkedList<std::string>();
    callDel->publify = classType->Ident;
    asmc::Mov *mov = new asmc::Mov();

    asmc::Push *push = new asmc::Push();
    push->op = mov->to = this->intArgs[0].get(asmc::QWord);
    file.text << push;
    mov->size = asmc::QWord;
    mov->from = this->registers["%eax"]->get(asmc::QWord);
    mov->to = this->intArgs[0].get(asmc::QWord);
    file.text << mov;
    file << this->GenSTMT(callDel);
  };

  return file;
}