#include "CodeGenerator/CodeGenerator.hpp"

#include <unistd.h>

#include <chrono>
#include <fstream>
#include <iostream>

#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Lower.hpp"
#include "PreProcessor.hpp"
#include "Scanner.hpp"
#include "Utils.hpp"

using namespace gen::utils;

#pragma region helper functions

typedef gen::Enum Enum;

bool Enum::compareEnum(Enum::EnumValue e, std::string ident) {
  return e.name == ident;
}

Enum::Enum() { this->values.foo = Enum::compareEnum; }

bool gen::Type::compare(gen::Type *t, std::string ident) {
  if (ident == t->Ident) return true;
  return false;
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
    std::cout << "Error: on line " << this->logicalLine << ": ";
    if (this->scope != nullptr) {
      std::cout << "in class " << this->scope->Ident << ": ";
    }
    if (!this->globalScope && this->currentFunction != nullptr) {
      std::cout << "in function " << this->currentFunction->ident.ident << ": ";
    }
    std::cout << message << std::endl;
    throw err::Exception(message);
  } else {
    std::cout << "Warning: on line " << this->logicalLine << ": ";
    if (this->scope != nullptr) {
      std::cout << "in class " << this->scope->Ident << ": ";
    }
    if (!this->globalScope && this->currentFunction != nullptr) {
      std::cout << "in function " << this->currentFunction->ident.ident << ": ";
    }
    std::cout << message << std::endl;
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
  this->registers.foo = asmc::Register::compare;
  this->nameTable.foo = compareFunc;
  this->includedMemo = HashMap<ast::Statement *>();
  this->nameSpaceTable = HashMap<std::string>();
  this->globalScope = true;
  this->typeList.foo = gen::Type::compare;
  this->TypeList.foo = ast::Type::compare;
  auto nullType = new ast::Type("NULLTYPE", asmc::QWord);
  this->TypeList.push(*nullType);
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

  std::string nsp;
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
  if (sym == nullptr) return std::make_tuple("", gen::Symbol(), false, pops);

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
    push->logicalLine = this->logicalLine;
    push->op = this->registers["%r14"]->get(asmc::QWord);
    OutputFile.text << push;
    while (modList.trail() > checkTo) {
      if (this->typeList[last.typeName] == nullptr)
        alert("type not found " + last.typeName);
      gen::Type type = **this->typeList[last.typeName];
      std::string sto = modList.touch();
      if (this->scope == *this->typeList[last.typeName]) {
        // if we are scoped to the type search the symbol in the type
        // symbol table
        modSym =
            type.SymbolTable.search<std::string>(searchSymbol, modList.shift());
      } else {
        // if we are not scoped to the type search the symbol in the
        // public symbol table
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
      access =
          std::to_string(tbyte - (this->getBytes(last.size) * last.arraySize)) +
          '(' + mov->to + ')';
    }

    // pop r14
    asmc::Pop *pop = new asmc::Pop;
    pop->op = this->registers["%r14"]->get(asmc::QWord);
    pop->logicalLine = this->logicalLine;
    pops.text << pop;
  };

  modList.invert();
  modList.reset();

  indicies.reset();
  modSym->type.indices.reset();
  gen::Symbol retSym = *modSym;

  if (indicies.trail() != 0) {
    this->canAssign(modSym->type, "adr",
                    "the given type {} is not subscriptable");

    if (modSym->type.indices.trail() != indicies.trail())
      alert("invalid index count");

    int multiplier = getBytes(modSym->type.typeHint->size);

    int count = 1;

    asmc::Mov *zero = new asmc::Mov();
    zero->from = "$0";
    zero->to = this->registers["%r12"]->get(asmc::QWord);
    zero->logicalLine = this->logicalLine;
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
      xr->logicalLine = this->logicalLine;
      OutputFile.text << xr;

      // move expression access to r13
      asmc::Mov *mov = new asmc::Mov();
      mov->size = asmc::DWord;
      mov->to = this->registers["%r13"]->get(asmc::DWord);
      mov->from = expr.access;
      mov->logicalLine = this->logicalLine;
      OutputFile.text << mov;

      // multiply r13 by count
      asmc::Mul *mul = new asmc::Mul();
      mul->op2 = this->registers["%r13"]->get(asmc::QWord);
      mul->op1 = '$' + std::to_string(count);
      mul->logicalLine = this->logicalLine;
      OutputFile.text << mul;

      // add r13 to r12
      asmc::Add *add = new asmc::Add();
      add->op1 = this->registers["%r13"]->get(asmc::QWord);
      add->op2 = this->registers["%r12"]->get(asmc::QWord);
      add->size = asmc::QWord;
      add->logicalLine = this->logicalLine;

      OutputFile.text << add;
      count = modSym->type.indices.shift();
    };
    // multiply r13 by the size of the type
    asmc::Mul *mul = new asmc::Mul();
    mul->op2 = this->registers["%r12"]->get(asmc::QWord);
    mul->op1 = '$' + std::to_string(multiplier);
    mul->size = asmc::QWord;
    mul->logicalLine = this->logicalLine;

    OutputFile.text << mul;
    // load the address of the acces to rdx
    asmc::Mov *mov = new asmc::Mov();
    mov->size = asmc::QWord;
    mov->to = this->registers["%rdx"]->get(asmc::QWord);
    mov->from = access;
    mov->logicalLine = this->logicalLine;
    OutputFile.text << mov;

    // add rdx to r13
    asmc::Add *add = new asmc::Add();
    add->op1 = this->registers["%rdx"]->get(asmc::QWord);
    add->op2 = this->registers["%r12"]->get(asmc::QWord);
    add->size = asmc::QWord;
    add->logicalLine = this->logicalLine;

    OutputFile.text << add;

    access = '(' + this->registers["%r12"]->get(asmc::QWord) + ')';
    retSym.type = *retSym.type.typeHint;
  };
  return std::make_tuple(access, retSym, true, pops);
};

bool gen::CodeGenerator::canAssign(ast::Type type, std::string typeName,
                                   std::string fmt, bool strict = false) {
  if (type.typeName == typeName) return true;
  if (type.fPointerArgs.returnType != nullptr && typeName == "adr") return true;
  if (type.typeName == "adr" && typeName.find("~") != std::string::npos)
    return true;
  if (typeName == "void" && type.typeName == "NULLTYPE") return true;
  if (typeName == "NULLTYPE" && type.typeName == "void") return true;
  if (typeName == "generic" && type.typeName == "NULLTYPE") return true;
  if (typeName == "NULLTYPE" && type.typeName == "generic") return true;
  if (typeName == "void") this->alert("cannot use void function as value");
  if (typeName == "--std--flex--function" || typeName == "any" ||
      type.typeName == "any")
    return true;
  if (type.typeName == "int" && typeName == "float") return true;
  if (type.typeName == "float" && typeName == "int") return true;
  if (type.typeName == "short" && typeName == "number") return true;
  if (type.typeName == "int" && typeName == "number") return true;
  if (type.typeName == "long" && typeName == "number") return true;
  if (type.typeName == "byte" && typeName == "number") return true;
  if (type.typeName == "number" && typeName == "short") return true;
  if (type.typeName == "number" && typeName == "int") return true;
  if (type.typeName == "number" && typeName == "long") return true;
  if (type.typeName == "number" && typeName == "float") return true;
  if (type.typeName == "number" && typeName == "byte") return true;

  if (typeName != "generic") {
    // search the type list for the type
    gen::Type **udef = this->typeList[typeName];
    if (udef != nullptr) {
      if (type.typeName == "adr") return true;
      // if the type is a class
      gen::Class *cl = dynamic_cast<gen::Class *>(*udef);
      if (cl != nullptr) {
        gen::Class *parent = cl->parent;
        if (parent != nullptr) {
          if (parent->Ident == type.typeName) return true;
        }
      }
    }

    // search if the expected type is the class
    gen::Type **expected = this->typeList[type.typeName];
    if (expected) {
      gen::Class *cl = dynamic_cast<gen::Class *>(*expected);
      if (cl) {
        if (cl->nameTable.count > 0) {
          ast::Function *init = cl->nameTable["init"];
          if (init) {
            if (init->req == 1 &&
                this->canAssign(init->argTypes[0], typeName, fmt)) {
              return false;
            }
          }
        }
      }
    }
  }

  if (type.size == asmc::QWord && (typeName == "adr" || typeName == "generic"))
    return true;
  if (strict && (type.typeName == "adr" || typeName == "generic")) return true;

  // compare two function pointers
  if (type.fPointerArgs.returnType != nullptr &&
      typeName.find("~") != std::string::npos) {
    // we need to check return type and args
    auto type2 = this->TypeList[typeName];
    if (type2 == nullptr) return false;
    if (type2->fPointerArgs.returnType == nullptr) return false;
    this->canAssign(*type2->fPointerArgs.returnType,
                    type.fPointerArgs.returnType->typeName, fmt);
    // type two should have at least as many required args as type
    if (type2->fPointerArgs.requiredArgs < type.fPointerArgs.requiredArgs)
      return false;
    for (int i = 0; i < type.fPointerArgs.requiredArgs; i++) {
      this->canAssign(type.fPointerArgs.argTypes[i],
                      type2->fPointerArgs.argTypes[i].typeName, fmt);
    };

    // now check the types of any extra optional args
    if (type2->fPointerArgs.argTypes.size() > type.fPointerArgs.requiredArgs) {
      for (int i = type.fPointerArgs.requiredArgs - 1;
           i < type2->fPointerArgs.argTypes.size(); i++) {
        this->canAssign(type.fPointerArgs.argTypes[i],
                        type2->fPointerArgs.argTypes[i].typeName, fmt);
      }
    }
    return true;
  };

  alert(format("Type mismatch on line {}: " + fmt, this->logicalLine,
               type.typeName, typeName));
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
  mov1->logicalLine = this->logicalLine;
  if (!isDiv) OutputFile.text << mov1;

  gen::Expr expr1 = this->GenExpr(compound.expr1, OutputFile);
  mov2->op = expr1.op;
  mov2->to = this->registers[r2]->get(expr1.size);
  mov2->from = expr1.access;
  mov2->size = expr1.size;
  mov2->logicalLine = this->logicalLine;
  OutputFile.text << mov2;

  return expr1;
}

gen::Expr gen::CodeGenerator::genArithmetic(asmc::ArithInst *inst,
                                            ast::Compound compound,
                                            asmc::File &OutputFile) {
  gen::Expr expr = this->prepareCompound(compound, OutputFile);
  gen::Expr output;

  inst->opType = expr.op;
  inst->size = expr.size;

  std::string to1 = this->registers["%rdx"]->get(expr.size);
  std::string to2 = this->registers["%rdi"]->get(expr.size);

  output.access = this->registers["%rax"]->get(expr.size);

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
  mov->logicalLine = this->logicalLine;

  asmc::Pop *pop1 = new asmc::Pop();
  pop1->op = this->registers[to1]->get(asmc::QWord);
  pop1->logicalLine = this->logicalLine;

  asmc::Pop *pop2 = new asmc::Pop();
  pop2->op = this->registers[to2]->get(asmc::QWord);
  pop2->logicalLine = this->logicalLine;

  inst->op2 = to2;
  inst->op1 = to1;
  inst->logicalLine = this->logicalLine;
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
  this->logicalLine = expr->logicalLine;

  if (dynamic_cast<ast::IntLiteral *>(expr) != nullptr) {
    ast::IntLiteral *intLit = dynamic_cast<ast::IntLiteral *>(expr);

    output.access = '$' + std::to_string(intLit->val);
    output.size = asmc::DWord;
    if (size != asmc::AUTO) output.size = size;

    output.type = "number";
  } else if (dynamic_cast<ast::LongLiteral *>(expr) != nullptr) {
    ast::LongLiteral *intLit = dynamic_cast<ast::LongLiteral *>(expr);
    output.type = "long";
    output.access = '$' + std::to_string(intLit->val);
    output.size = asmc::QWord;
  } else if (dynamic_cast<ast::CharLiteral *>(expr) != nullptr) {
    ast::CharLiteral *intLit = dynamic_cast<ast::CharLiteral *>(expr);
    output.type = "char";
    output.access = '$' + std::to_string(intLit->value);
    output.size = asmc::Byte;
  } else if (dynamic_cast<ast::CallExpr *>(expr) != nullptr) {
    ast::CallExpr *exprCall = dynamic_cast<ast::CallExpr *>(expr);
    ast::Call *call = exprCall->call;

    // check if the call ident is a class name
    Type **t = this->typeList[call->ident];
    if (t != nullptr) {
      gen::Class *cl = dynamic_cast<gen::Class *>(*t);
      if (cl != nullptr) {
        if (cl->dynamic)
          alert("Dynamic class '" + cl->Ident + "' must be called with new");
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
        lea->logicalLine = this->logicalLine;
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
          push->logicalLine = this->logicalLine;
          OutputFile.text << push;
          mov->size = asmc::QWord;
          mov->from = this->registers["%eax"]->get(asmc::QWord);
          mov->to = this->intArgs[0].get(asmc::QWord);
          mov->logicalLine = this->logicalLine;
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
      if (size != asmc::AUTO && (func.flex || func.type.typeName == "any"))
        output.size = size;
      output.access = this->registers["%rax"]->get(output.size);
      if (func.type.typeName == "float") {
        output.access = this->registers["%xmm0"]->get(output.size);
        output.op = asmc::Float;
      }
    }
    if (exprCall->typeCast != "") {
      if (output.type != "any" && output.type != "adr" &&
          output.type != "generic")
        this->alert(
            "Can only explicitly cast to a type from any, adr, "
            "or generic");
      output.type = exprCall->typeCast;
    }
  } else if (dynamic_cast<ast::Var *>(expr) != nullptr) {
    ast::Var var = *dynamic_cast<ast::Var *>(expr);

    std::tuple<std::string, gen::Symbol, bool, asmc::File> resolved =
        this->resolveSymbol(var.Ident, var.modList, OutputFile, var.indices,
                            var.internal);

    if (std::get<2>(resolved) == false) {
      std::string ident = var.Ident;
      std::string nsp;
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
        // check if t is an enum
        Enum *en = dynamic_cast<Enum *>(type);
        if (en) {
          if (var.modList.trail() == 0) {
            alert("Enum " + ident + " cannot be used as a variable");
          }
          std::string enumIdent = var.modList.shift();
          Enum::EnumValue *item = en->values[enumIdent];
          if (!item) {
            alert("Enum " + ident + " does not contain " + enumIdent);
          }
          output.access = '$' + std::to_string(item->value);
          output.type = en->Ident;
          output.size = asmc::DWord;
        } else {
          auto cl = dynamic_cast<gen::Class *>(type);
          if (var.modList.trail() == 1 && cl) {
            std::string functionName = var.modList.shift();
            auto func = cl->nameTable[functionName];
            if (func) {
              output.access = "$pub_" + cl->Ident + "_" + func->ident.ident;
              output.type = "adr";
              output.size = asmc::QWord;
            } else {
              alert("Class " + ident + " does not contain " + functionName);
            }
          } else {
            output.access =
                '$' + std::to_string(type->SymbolTable.head->data.byteMod);
            output.type = "int";
            output.size = asmc::DWord;
          }
        };
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
        output.type = "generic";
      } else if (var.Ident == "**void_type**") {
        output.size = asmc::QWord;
        output.access = "$0";
        output.type = "void";
      } else if (var.Ident == "true") {
        output.size = asmc::Byte;
        output.access = "$1";
        output.type = "bool";
      } else if (var.Ident == "false") {
        output.size = asmc::Byte;
        output.access = "$0";
        output.type = "bool";
      } else if (this->nameTable[ident] != nullptr) {
        auto func = this->nameTable[ident];
        auto typeName = func->type.typeName + "~";
        for (auto arg : func->argTypes) {
          typeName += arg.typeName + ",";
        }
        if (func->argTypes.size() > 0) typeName.pop_back();
        typeName += "~";

        auto newType = new ast::Type(typeName, asmc::QWord);
        newType->fPointerArgs.returnType = &func->type;
        newType->fPointerArgs.argTypes = func->argTypes;
        newType->fPointerArgs.isFPointer = true;
        newType->fPointerArgs.requiredArgs = func->req;

        this->TypeList.push(*newType);

        output.size = asmc::QWord;
        output.access = '$' + this->nameTable[ident]->ident.ident;
        output.type = typeName;
      } else if (this->scope != nullptr &&
                 this->scope->nameTable[ident] != nullptr) {
        output.size = asmc::QWord;
        output.access = "$pub_" + this->scope->Ident + "_" + ident;
        output.type = "adr";
      } else if (this->scope != nullptr && var.Ident == "__typeName__") {
        ast::StringLiteral *strLit = new ast::StringLiteral();
        strLit->val = this->scope->Ident;
        strLit->logicalLine = this->logicalLine;
        output = this->GenExpr(strLit, OutputFile);
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

      // check if the symbol type is a class
      auto cont = true;
      gen::Type **t = this->typeList[sym.type.typeName];
      if (t) {
        gen::Class *cl = dynamic_cast<gen::Class *>(*t);
        if (cl) {
          if (cl->safeType && sym.symbol != "my") {
            output.passable = false;
            if (cl->nameTable["get"] != nullptr) {
              ast::Call *callGet = new ast::Call();
              callGet->ident = var.Ident;
              callGet->modList = var.modList;
              callGet->modList << "get";
              callGet->logicalLine = var.logicalLine;
              ast::CallExpr *callExpr = new ast::CallExpr();
              callExpr->call = callGet;
              callExpr->logicalLine = var.logicalLine;
              output = this->GenExpr(callExpr, OutputFile, size);
              callGet->modList.pop();
              cont = false;
            }
          }
        }
      }

      // mov output to r15
      if (cont) {
        asmc::Mov *mov = new asmc::Mov();
        std::string move2 = (output.op == asmc::Float)
                                ? this->registers["%xmm0"]->get(asmc::QWord)
                                : this->registers["%r15"]->get(output.size);

        mov->to = move2;
        mov->from = std::get<0>(resolved);
        mov->size = output.size;
        mov->op = output.op;
        mov->logicalLine = this->logicalLine;
        OutputFile.text << mov;
        output.access = mov->to;
        OutputFile << std::get<3>(resolved);
      };
    }

    if (var.typeCast != "") {
      if (output.type != "any" && output.type != "adr" &&
          output.type != "generic")
        this->alert(
            "Can only explicitly cast to a type from any, adr, "
            "or generic");
      output.type = var.typeCast;
    }
  } else if (dynamic_cast<ast::Reference *>(expr) != nullptr) {
    ast::Reference ref = *dynamic_cast<ast::Reference *>(expr);

    std::tuple<std::string, gen::Symbol, bool, asmc::File> resolved =
        this->resolveSymbol(ref.Ident, ref.modList, OutputFile,
                            links::LinkedList<ast::Expr *>(), ref.internal);

    asmc::Lea *lea = new asmc::Lea();
    lea->logicalLine = this->logicalLine;

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
    asmc::StringLiteral *strLit = new asmc::StringLiteral();
    asmc::Label *label = new asmc::Label();
    if (this->scope == nullptr)
      label->label = ".str" + this->nameTable.head->data.ident.ident +
                     std::to_string(this->labelCount);
    else
      label->label = ".str" + scope->Ident + '.' +
                     scope->nameTable.head->data.ident.ident +
                     std::to_string(this->labelCount);
    this->labelCount++;
    strLit->value = str.val;
    OutputFile.data << label;
    OutputFile.data << strLit;
    output.access = "$" + label->label;
    output.size = asmc::QWord;
    output.type = "adr";
  } else if (dynamic_cast<ast::FStringLiteral *>(expr) != nullptr) {
    ast::FStringLiteral str = *dynamic_cast<ast::FStringLiteral *>(expr);

    auto call = new ast::CallExpr();
    call->call = new ast::Call();
    call->call->ident = "_fCstr";
    call->call->logicalLine = this->logicalLine;
    call->logicalLine = this->logicalLine;

    ast::StringLiteral *strLit = new ast::StringLiteral();
    strLit->val = str.val;
    strLit->logicalLine = this->logicalLine;
    call->call->Args << strLit;

    ast::StructList *list = new ast::StructList();
    list->logicalLine = this->logicalLine;

    // swap the placeholders with correct wildcard
    std::string::size_type pos;
    for (auto expr : str.args) {
      pos = strLit->val.find("%%%", 0);
      if (pos == std::string::npos)
        this->alert("too many arguments for format string");

      asmc::File file;

      auto exp = this->GenExpr(expr, file);

      gen::Type **t = this->typeList[exp.type];
      if (t) {
        gen::Class *cl = dynamic_cast<gen::Class *>(*t);
        if (cl && cl->Ident != "string") {
          if (cl->nameTable["toString"] == nullptr) {
            if (cl->parent != nullptr) {
              if (cl->parent->nameTable["toString"] == nullptr) {
                this->alert("class " + cl->Ident +
                            " does not contain a toString method");
              } else {
                cl = cl->parent;
              }
            } else {
              this->alert("class " + cl->Ident +
                          " does not contain a toString method");
            }
          }

          ast::CallExpr *call = new ast::CallExpr();
          call->call = new ast::Call();
          call->call->ident = "toString";
          call->call->logicalLine = this->logicalLine;
          call->logicalLine = this->logicalLine;
          call->call->Args << expr;
          call->call->publify = cl->Ident;
          expr = call;
        }
        exp = this->GenExpr(expr, file);
      }

      if (exp.type == "adr")
        strLit->val.replace(pos, 3, "%a");
      else if (exp.type == "int" || exp.type == "number")
        strLit->val.replace(pos, 3, "%d");
      else if (exp.type == "string")
        strLit->val.replace(pos, 3, "%s");
      else if (exp.type == "bool")
        strLit->val.replace(pos, 3, "%b");
      else if (exp.type == "char")
        strLit->val.replace(pos, 3, "%c");
      else {
        this->alert("unable to format type of " + exp.type);
      }

      list->args << expr;
    };

    call->call->Args << list;
    output = this->GenExpr(call, OutputFile);
    output.type = "string";
  } else if (dynamic_cast<ast::FloatLiteral *>(expr) != nullptr) {
    ast::FloatLiteral *floatLit = dynamic_cast<ast::FloatLiteral *>(expr);
    asmc::FloatLiteral *fltLit = new asmc::FloatLiteral();
    asmc::Label *label = new asmc::Label();
    if (this->scope == nullptr)
      label->label = ".float" + this->nameTable.head->data.ident.ident +
                     std::to_string(this->labelCount);
    else
      label->label = ".float" + scope->Ident + '.' +
                     scope->nameTable.head->data.ident.ident +
                     std::to_string(this->labelCount);
    this->labelCount++;
    fltLit->value = floatLit->val;

    // move value to the xmm0 register
    asmc::Mov *mov = new asmc::Mov();
    mov->size = asmc::DWord;
    mov->op = asmc::Float;
    mov->to = this->registers["%xmm0"]->get(asmc::DWord);
    mov->from = label->label;

    output.op = asmc::Float;
    mov->logicalLine = this->logicalLine;

    OutputFile.text << mov;
    OutputFile.data << label;
    OutputFile.data << fltLit;
    output.access = this->registers["%xmm0"]->get(asmc::DWord);
    output.size = asmc::DWord;
    output.type = "int";
  } else if (dynamic_cast<ast::DeReference *>(expr)) {
    ast::DeReference deRef = *dynamic_cast<ast::DeReference *>(expr);
    gen::Symbol *sym =
        gen::scope::ScopeManager::getInstance()->get(deRef.Ident);

    asmc::Mov *mov = new asmc::Mov();
    asmc::Mov *mov2 = new asmc::Mov();

    mov->logicalLine = this->logicalLine;
    mov2->logicalLine = this->logicalLine;

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
    asmc::File Dummy = asmc::File();
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
      call->logicalLine = this->logicalLine;
      call->call = new ast::Call;
      call->call->logicalLine = comp.logicalLine;
      call->call->ident = opor->ident.ident;
      call->call->Args = links::LinkedList<ast::Expr *>();
      call->call->Args.push(comp.expr1);
      call->call->Args.push(comp.expr2);
      call->call->publify = tname;

      gen::Expr afterCall = this->GenExpr(call, OutputFile);
      output.access = afterCall.access;
      output.size = afterCall.size;
      output.type = opor->type.typeName;
    } else {
      // push rdi and rdx to stack
      asmc::Push *push1 = new asmc::Push();
      push1->logicalLine = this->logicalLine;
      push1->op = this->registers["%rdi"]->get(asmc::QWord);
      OutputFile.text << push1;
      asmc::Push *push2 = new asmc::Push();
      push2->logicalLine = this->logicalLine;
      push2->op = this->registers["%rdx"]->get(asmc::QWord);
      OutputFile.text << push2;
      output.op = asmc::Hard;
      output.type = "--std--flex--function";
      switch (comp.op) {
        case ast::Plus: {
          asmc::Add *add = new asmc::Add();
          output = this->genArithmetic(add, comp, OutputFile);
          break;
        }
        case ast::Minus: {
          asmc::Sub *sub = new asmc::Sub();
          output = this->genArithmetic(sub, comp, OutputFile);
          break;
        }
        case ast::AndBit: {
          asmc::And *andBit = new asmc::And();
          output = this->genArithmetic(andBit, comp, OutputFile);
          break;
        }
        case ast::OrBit: {
          asmc::Or *orBit = new asmc::Or();
          output = this->genArithmetic(orBit, comp, OutputFile);
          break;
        }
        case ast::Less: {
          asmc::Sal *andBit = new asmc::Sal();
          gen::Expr expr1 = this->GenExpr(comp.expr1, Dummy);
          gen::Expr expr2 = this->GenExpr(comp.expr2, Dummy);

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
          mov->logicalLine = this->logicalLine;
          mov->to = to1;
          mov->from = this->registers["%rdx"]->get(expr1.size);
          mov->size = expr1.size;

          OutputFile.text << mov;

          andBit->op2 = to2;
          andBit->op1 = "%cl";
          andBit->size = expr1.size;
          andBit->logicalLine = this->logicalLine;
          OutputFile.text << andBit;
          output.access = this->registers["%rdi"]->get(expr1.size);

          // move access to rax
          asmc::Mov *mov2 = new asmc::Mov();
          mov2->logicalLine = this->logicalLine;
          mov2->to = this->registers["%rax"]->get(expr1.size);
          mov2->from = output.access;
          mov2->size = expr1.size;
          OutputFile.text << mov2;
          output.access = this->registers["%rax"]->get(expr1.size);

          break;
        }
        case ast::Great: {
          asmc::Sar *andBit = new asmc::Sar();
          gen::Expr expr1 = this->GenExpr(comp.expr1, Dummy);
          gen::Expr expr2 = this->GenExpr(comp.expr2, Dummy);

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
          mov->logicalLine = this->logicalLine;

          OutputFile.text << mov;

          andBit->op2 = to2;
          andBit->op1 = "%cl";
          andBit->size = expr1.size;
          andBit->logicalLine = this->logicalLine;

          OutputFile.text << andBit;

          output.size = expr1.size;
          output.type = expr1.type;
          output.access = to2;

          // move access to rax
          asmc::Mov *mov2 = new asmc::Mov();
          mov2->to = this->registers["%rax"]->get(expr1.size);
          mov2->from = output.access;
          mov2->size = expr1.size;
          mov2->logicalLine = this->logicalLine;
          OutputFile.text << mov2;
          output.access = this->registers["%rax"]->get(expr1.size);
          break;
        }
        case ast::Mul: {
          asmc::Mul *mul = new asmc::Mul();
          output = this->genArithmetic(mul, comp, OutputFile);
          break;
        }
        case ast::Div: {
          asmc::Div *div = new asmc::Div();
          div->logicalLine = this->logicalLine;

          gen::Expr expr1 = this->GenExpr(comp.expr1, Dummy);
          gen::Expr expr2 = this->GenExpr(comp.expr2, Dummy, expr1.size);

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
            div->logicalLine = this->logicalLine;
            this->prepareCompound(comp, OutputFile);
            output.op = asmc::Float;
          } else {
            asmc::Mov *mov = new asmc::Mov;
            mov->logicalLine = this->logicalLine;
            asmc::Mov *eax = new asmc::Mov;
            eax->logicalLine = this->logicalLine;
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
          div->logicalLine = this->logicalLine;

          this->selectReg = 0;
          gen::Expr expr1 = this->GenExpr(comp.expr1, Dummy);
          this->selectReg = 1;
          gen::Expr expr2 = this->GenExpr(comp.expr2, Dummy);

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
            mov->logicalLine = this->logicalLine;
            asmc::Mov *eax = new asmc::Mov;
            eax->logicalLine = this->logicalLine;
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
          mov->logicalLine = this->logicalLine;
          mov->to = output.access;
          mov->from = this->registers["%rdx"]->get(expr1.size);
          mov->size = expr1.size;

          OutputFile.text << div;
          if (expr1.op != asmc::Float) OutputFile.text << mov;
          output.size = asmc::DWord;
          output.type = "int";
          break;
        }
        case ast::CompEqu: {
          asmc::Cmp *cmp = new asmc::Cmp();
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Sete *sete = new asmc::Sete();
          sete->logicalLine = this->logicalLine;
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
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Setl *setl = new asmc::Setl();
          setl->logicalLine = this->logicalLine;
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
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Setg *setg = new asmc::Setg();
          setg->logicalLine = this->logicalLine;
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
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Setle *setle = new asmc::Setle();
          setle->logicalLine = this->logicalLine;
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
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Setge *setge = new asmc::Setge();
          setge->logicalLine = this->logicalLine;
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
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Setne *setne = new asmc::Setne();
          setne->logicalLine = this->logicalLine;
          setne->op = this->registers["%rax"]->get(asmc::Byte);

          OutputFile.text << cmp;
          OutputFile.text << setne;

          output.size = asmc::Byte;
          output.type = "bool";
          output.access = "%al";
          break;
        }
        default: {
          alert("Unhandled operator in comparison");
          break;
        }
      }
      // pop rdx and rdi
      asmc::Pop *pop = new asmc::Pop();
      pop->logicalLine = this->logicalLine;
      pop->op = this->registers["%rdx"]->get(asmc::QWord);
      OutputFile.text << pop;
      asmc::Pop *pop2 = new asmc::Pop();
      pop2->logicalLine = this->logicalLine;
      pop2->op = this->registers["%rdi"]->get(asmc::QWord);
      OutputFile.text << pop2;
    }

    if (comp.typeCast != "") {
      if (output.type != "any" && output.type != "adr" &&
          output.type != "generic")
        this->alert(
            "Can only explicitly cast to a type from any, adr, "
            "or generic");
      output.type = comp.typeCast;
    }
  } else if (dynamic_cast<ast::Lambda *>(expr) != nullptr) {
    ast::Lambda lambda = *dynamic_cast<ast::Lambda *>(expr);
    ast::Function *func = lambda.function;
    bool inFunc = this->inFunction;
    bool gScope = this->globalScope;

    auto mills = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();
    std::string id =
        "lambda_" + std::to_string(mills) + "_" + std::to_string(labelCount);
    func->ident.ident = id;
    labelCount++;
    func->scopeName = "global";
    func->isLambda = true;

    if (OutputFile.lambdas == nullptr) OutputFile.lambdas = new asmc::File;
    OutputFile.hasLambda = true;

    ast::Type Adr = ast::Type();
    Adr.typeName = "adr";
    Adr.opType = asmc::Hard;
    Adr.size = asmc::QWord;

    func->type = Adr;

    this->nameTable.push(*func);
    ast::Type saveRetType = this->returnType;

    this->returnType.typeName = "--std--flex--function";
    auto saveLambdaReturns = this->lambdaReturns = "void";
    auto saveLambdaSize = this->lambdaSize = asmc::QWord;
    OutputFile.lambdas->operator<<(this->GenSTMT(func));

    this->returnType = saveRetType;

    this->nameTable.pop();

    auto typeName = lambdaReturns + "~";
    for (auto arg : func->argTypes) {
      typeName += arg.typeName;
      typeName += ",";
    }
    // remove the last comma if it exists
    if (typeName[typeName.size() - 1] == ',') typeName.pop_back();
    typeName += "~";
    auto type = new ast::Type(typeName, asmc::QWord);
    type->fPointerArgs.argTypes = func->argTypes;
    type->fPointerArgs.returnType =
        new ast::Type(lambdaReturns, this->lambdaSize);
    type->fPointerArgs.isFPointer = true;
    type->fPointerArgs.requiredArgs = func->req;
    this->TypeList.push(*type);

    this->inFunction = inFunc;
    this->globalScope = gScope;
    this->lambdaReturns = saveLambdaReturns;
    this->lambdaSize = saveLambdaSize;

    output.access = "$" + id;
    output.size = asmc::QWord;
    output.type = typeName;
  } else if (dynamic_cast<ast::NewExpr *>(expr) != nullptr) {
    ast::NewExpr newExpr = *dynamic_cast<ast::NewExpr *>(expr);
    ast::Function *malloc = this->nameTable["malloc"];
    if (malloc == nullptr)
      alert(
          "Please import std library in order to use new operator.\n\n -> "
          ".needs <std> \n\n");
    gen::Type **type = this->typeList[newExpr.type.typeName];
    if (type == nullptr) alert("Type" + newExpr.type.typeName + " not found");
    // check if the function is a class
    gen::Class *cl = dynamic_cast<gen::Class *>(*type);
    if (cl == nullptr)
      alert("The new operator can only be used with classes. Type " +
            newExpr.type.typeName + " is not a class");
    // check if the class has a constructor
    ast::Function *init = cl->nameTable["init"];

    // first call malloc with the size of the class
    ast::CallExpr *callMalloc = new ast::CallExpr();
    callMalloc->logicalLine = newExpr.logicalLine;
    callMalloc->call = new ast::Call;
    callMalloc->call->logicalLine = newExpr.logicalLine;
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
      callInit->logicalLine = newExpr.logicalLine;
      callInit->call = new ast::Call;
      callInit->call->logicalLine = newExpr.logicalLine;
      callInit->call->ident = "init";
      callInit->call->Args = newExpr.args;
      callInit->call->modList = links::LinkedList<std::string>();
      callInit->call->publify = cl->Ident;
      asmc::Mov *mov = new asmc::Mov();
      mov->logicalLine = this->logicalLine;
      asmc::Push *push = new asmc::Push();
      push->logicalLine = this->logicalLine;
      push->op = mov->to = this->registers["%rdi"]->get(asmc::QWord);
      OutputFile.text << push;
      mov->size = asmc::QWord;
      mov->from = this->registers["%eax"]->get(asmc::QWord);
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
    movzbl->logicalLine = this->logicalLine;
    movzbl->from = expr.access;
    movzbl->to = this->registers["%eax"]->get(asmc::DWord);

    asmc::Xor *xr = new asmc::Xor();
    xr->logicalLine = this->logicalLine;
    xr->op1 = "$1";
    xr->op2 = this->registers["%eax"]->get(asmc::DWord);

    ast::Type boolType = ast::Type();

    boolType.typeName = "bool";
    boolType.opType = asmc::Hard;
    boolType.size = asmc::Byte;
    this->canAssign(boolType, expr.type,
                    "the not operator can only be used with bool types. "
                    "Cannot resolve {} and {}");

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

    auto mills = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();
    std::string uniqueIdent = std::to_string(mills);
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
      mov->logicalLine = this->logicalLine;
      mov->from = genExpr.access;
      mov->to = this->registers["%eax"]->get(genExpr.size);
      mov->size = genExpr.size;
      OutputFile.text << mov;
      asmc::Mov *mov2 = new asmc::Mov();
      mov2->logicalLine = this->logicalLine;
      mov2->from = this->registers["%eax"]->get(genExpr.size);
      mov2->to = "-" + std::to_string(bMod - offset) + "(%rbp)";
      mov2->size = genExpr.size;
      OutputFile.text << mov2;
      offset += this->getBytes(genExpr.size);
    };

    // create a pointer to the struct
    asmc::Lea *lea = new asmc::Lea();
    lea->logicalLine = this->logicalLine;
    lea->from = "-" + std::to_string(bMod) + "(%rbp)";
    lea->to = this->registers["%rax"]->get(asmc::QWord);
    OutputFile.text << lea;

    output.access = this->registers["%rax"]->get(asmc::QWord);
    output.size = asmc::QWord;
    output.type = "adr";
  } else if (dynamic_cast<ast::IfExpr *>(expr) != nullptr) {
    auto ifExpr = dynamic_cast<ast::IfExpr *>(expr);

    asmc::Label *label1 = new asmc::Label();
    label1->logicalLine = ifExpr->logicalLine;
    label1->label = ".L" + this->nameTable.head->data.ident.ident +
                    std::to_string(this->labelCount);
    this->labelCount++;
    asmc::Label *label2 = new asmc::Label();
    label2->logicalLine = ifExpr->logicalLine;
    label2->label = ".L" + this->nameTable.head->data.ident.ident +
                    std::to_string(this->labelCount);
    this->labelCount++;

    gen::Expr cond = this->GenExpr(ifExpr->expr, OutputFile);
    ast::Type boolType = ast::Type();
    boolType.typeName = "bool";
    boolType.opType = asmc::Hard;
    boolType.size = asmc::Byte;
    this->canAssign(boolType, cond.type,
                    "if condition must be a bool cannot resolve {} and {}");

    asmc::Mov *mov = new asmc::Mov();
    mov->logicalLine = this->logicalLine;
    mov->from = cond.access;
    mov->to = this->registers["%eax"]->get(asmc::Byte);
    mov->size = asmc::Byte;
    OutputFile.text << mov;

    asmc::Cmp *cmp = new asmc::Cmp();
    cmp->logicalLine = this->logicalLine;
    cmp->from = "$0";
    cmp->to = this->registers["%eax"]->get(asmc::Byte);
    cmp->size = asmc::Byte;

    OutputFile.text << cmp;

    asmc::Je *je = new asmc::Je();
    je->logicalLine = this->logicalLine;
    je->to = label1->label;
    OutputFile.text << je;

    gen::Expr trueExpr = this->GenExpr(ifExpr->trueExpr, OutputFile);
    asmc::Mov *mov2 = new asmc::Mov();
    mov2->logicalLine = this->logicalLine;
    mov2->from = trueExpr.access;
    mov2->to = this->registers["%eax"]->get(trueExpr.size);
    mov2->size = trueExpr.size;
    OutputFile.text << mov2;

    asmc::Jmp *jmp = new asmc::Jmp();
    jmp->logicalLine = this->logicalLine;
    jmp->to = label2->label;
    OutputFile.text << jmp;

    OutputFile.text << label1;
    if (!ifExpr->falseExpr)
      this->alert("if expression must have a false branch");
    gen::Expr falseExpr = this->GenExpr(ifExpr->falseExpr, OutputFile);
    asmc::Mov *mov3 = new asmc::Mov();
    mov3->logicalLine = this->logicalLine;
    mov3->from = falseExpr.access;
    mov3->to = this->registers["%eax"]->get(falseExpr.size);
    mov3->size = falseExpr.size;
    OutputFile.text << mov3;

    OutputFile.text << label2;

    ast::Type type = ast::Type();
    type.typeName = trueExpr.type;
    this->canAssign(
        type, falseExpr.type,
        "if expression must return the same type cannot resolve {} and {}");

    output.access = this->registers["%eax"]->get(trueExpr.size);
    output.size = trueExpr.size;
    output.type = trueExpr.type;
  } else {
    this->alert("Unhandled expression");
  }

  if (expr->extention != nullptr) {
    auto type = ast::Type();
    type.typeName = output.type;
    type.size = output.size;

    const auto tempName = "$" + std::to_string(this->tempCount++) + "_temp";

    auto mod =
        gen::scope::ScopeManager::getInstance()->assign(tempName, type, false);
    auto mov2 = new asmc::Mov();
    mov2->logicalLine = this->logicalLine;
    mov2->from = output.access;
    mov2->to = "-" + std::to_string(mod) + "(%rbp)";
    mov2->size = output.size;
    mov2->op = output.op;
    OutputFile.text << mov2;

    auto call = dynamic_cast<ast::CallExpr *>(expr->extention);
    auto var = dynamic_cast<ast::Var *>(expr->extention);

    if (call != nullptr) {
      call->call->modList.invert();
      call->call->modList.push(call->call->ident);
      call->call->modList.invert();
      call->call->ident = tempName;
    } else if (var != nullptr) {
      var->modList.invert();
      var->modList.push(var->Ident);
      var->modList.invert();
      var->Ident = tempName;
    } else {
      this->alert("Cannot extend an expression of this type");
    }

    output = this->GenExpr(expr->extention, OutputFile, size);
  }

  return output;
}

ast::Expr *gen::CodeGenerator::imply(ast::Expr *expr, std::string typeName) {
  // find the type
  auto type = this->typeList[typeName];
  if (type != nullptr) {
    auto cl = dynamic_cast<gen::Class *>(*type);
    if (cl != nullptr) {
      if (cl->dynamic) {
        ast::NewExpr *newExpr = new ast::NewExpr();
        newExpr->logicalLine = this->logicalLine;
        newExpr->type.typeName = typeName;
        newExpr->type.safeType = cl->safeType;
        newExpr->type.opType = asmc::Hard;
        newExpr->type.size = asmc::QWord;
        newExpr->args.push(expr);
        return newExpr;
      } else {
        ast::Call *init = new ast::Call();
        init->logicalLine = this->logicalLine;
        init->ident = typeName;
        init->Args.push(expr);
        ast::CallExpr *call = new ast::CallExpr;
        call->logicalLine = this->logicalLine;
        call->call = init;
        return call;
      }
    }
  }
  this->alert("Cannot imply type " + typeName +
              " from expression of type at line " +
              std::to_string(expr->logicalLine));
}

links::LinkedList<gen::Symbol> gen::CodeGenerator::GenTable(
    ast::Statement *STMT, links::LinkedList<gen::Symbol> &table) {
  if (dynamic_cast<ast::Sequence *>(STMT) != nullptr) {
    ast::Sequence *sequence = dynamic_cast<ast::Sequence *>(STMT);
    this->GenTable(sequence->Statement1, table);
    this->GenTable(sequence->Statement2, table);
  } else if (dynamic_cast<ast::Declare *>(STMT) != nullptr) {
    /*
        movl $0x0, -[SymbolT + size](rdp)
        **also needs to be added to symbol table**
    */

    ast::Declare *arg = dynamic_cast<ast::Declare *>(STMT);
    gen::Symbol symbol;

    int offset = this->getBytes(arg->type.size);

    if (table.search<std::string>(searchSymbol, arg->ident) != nullptr)
      alert("redefined variable:" + arg->ident);

    symbol.symbol = arg->ident;
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
      alert("redefined variable" + dec->ident);

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

void gen::CodeGenerator::GenArgs(ast::Statement *STMT, asmc::File &OutputFile) {
  if (dynamic_cast<ast::Sequence *>(STMT) != nullptr) {
    ast::Sequence *sequence = dynamic_cast<ast::Sequence *>(STMT);
    this->GenArgs(sequence->Statement1, OutputFile);
    this->GenArgs(sequence->Statement2, OutputFile);
  } else if (dynamic_cast<ast::Declare *>(STMT) != nullptr) {
    /*
        movl $0x0, -[SymbolT + size](rdp)
        **also needs to be added to symbol table**
    */
    ast::Declare *arg = dynamic_cast<ast::Declare *>(STMT);
    if (intArgsCounter > 6) {
      alert(
          "AFlat compiler cannot handle more than 6 int / pointer "
          "arguments.");
    } else {
      asmc::Size size;
      gen::Symbol symbol;
      asmc::Mov *mov = new asmc::Mov();
      mov->logicalLine = this->logicalLine;

      size = arg->type.size;
      arg->type.arraySize = 1;
      mov->from = this->intArgs[intArgsCounter].get(arg->type.size);

      int mod = gen::scope::ScopeManager::getInstance()->assign(
          arg->ident, arg->type, false, arg->mut);

      mov->size = size;
      mov->to = "-" + std::to_string(mod) + +"(%rbp)";
      OutputFile.text << mov;
      intArgsCounter++;
    }
  }
}

asmc::File gen::CodeGenerator::GenSTMT(ast::Statement *STMT) {
  asmc::File OutputFile = asmc::File();
  this->logicalLine = STMT->logicalLine;

  if (STMT->locked)
    OutputFile.text.push(new asmc::nop());
  else if (dynamic_cast<ast::Return *>(STMT) != nullptr) {
    this->genReturn(dynamic_cast<ast::Return *>(STMT), OutputFile);
    this->currentFunction->has_return = true;
  } else if (dynamic_cast<ast::Assign *>(STMT) != nullptr) {
    this->genAssign(dynamic_cast<ast::Assign *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Call *>(STMT) != nullptr) {
    this->GenCall(dynamic_cast<ast::Call *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Push *>(STMT) != nullptr) {
    this->genPush(dynamic_cast<ast::Push *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Pull *>(STMT) != nullptr) {
    this->genPull(dynamic_cast<ast::Pull *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::If *>(STMT) != nullptr) {
    this->genIf(dynamic_cast<ast::If *>(STMT), OutputFile);
    this->currentFunction->has_return = false;
  } else if (dynamic_cast<ast::While *>(STMT) != nullptr) {
    this->genWhile(dynamic_cast<ast::While *>(STMT), OutputFile);
    this->currentFunction->has_return = false;
  } else if (dynamic_cast<ast::For *>(STMT) != nullptr) {
    this->genFor(dynamic_cast<ast::For *>(STMT), OutputFile);
    this->currentFunction->has_return = false;
  } else if (dynamic_cast<ast::Struct *>(STMT) != nullptr) {
    this->genUDef(dynamic_cast<ast::Struct *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Class *>(STMT) != nullptr) {
    this->genClass(dynamic_cast<ast::Class *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Enum *>(STMT)) {
    this->genEnum(dynamic_cast<ast::Enum *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Inc *>(STMT) != nullptr) {
    this->genInc(dynamic_cast<ast::Inc *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Dec *>(STMT) != nullptr) {
    this->genDec(dynamic_cast<ast::Dec *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Import *>(STMT) != nullptr) {
    this->genImport(dynamic_cast<ast::Import *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Delete *>(STMT) != nullptr) {
    this->genDelete(dynamic_cast<ast::Delete *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Continue *>(STMT)) {
    this->genContinue(dynamic_cast<ast::Continue *>(STMT), OutputFile);
  } else if (dynamic_cast<ast::Break *>(STMT)) {
    this->genBreak(dynamic_cast<ast::Break *>(STMT), OutputFile);
  } else {
    OutputFile << STMT->generate(*this);
  }

  return OutputFile;
}

asmc::File gen::CodeGenerator::ImportsOnly(ast::Statement *STMT) {
  asmc::File OutputFile = asmc::File();
  if (STMT->locked)
    OutputFile.text.push(new asmc::nop());
  else if (dynamic_cast<ast::Sequence *>(STMT) != nullptr) {
    this->ImportsOnly(dynamic_cast<ast::Sequence *>(STMT)->Statement1);
    this->ImportsOnly(dynamic_cast<ast::Sequence *>(STMT)->Statement2);
  } else if (dynamic_cast<ast::Import *>(STMT) != nullptr) {
    auto imp = dynamic_cast<ast::Import *>(STMT);
    if (imp->classes)
      this->genImport(dynamic_cast<ast::Import *>(STMT), OutputFile);
  }
  return OutputFile;
}

asmc::File *gen::CodeGenerator::deScope(gen::Symbol &sym) {
  asmc::File *file = new asmc::File();
  gen::Type **type = this->typeList[sym.type.typeName];
  if (type == nullptr) return nullptr;

  gen::Class *classType = dynamic_cast<gen::Class *>(*type);
  if (classType == nullptr) return nullptr;

  // check if the class has a destructor
  ast::Function *endScope = classType->nameTable["endScope"];
  if (endScope == nullptr) return nullptr;

  // push rax to preserve return value
  asmc::Push *push = new asmc::Push();
  push->logicalLine = this->logicalLine;
  push->op = this->registers["%rax"]->get(asmc::QWord);
  file->text << push;

  // call the destructor
  ast::Call *callDel = new ast::Call();
  callDel->logicalLine = this->logicalLine;
  callDel->ident = sym.symbol;
  callDel->Args = LinkedList<ast::Expr *>();
  callDel->modList = links::LinkedList<std::string>();
  callDel->modList.push("endScope");
  file->operator<<(this->GenSTMT(callDel));

  // pop rax
  asmc::Pop *pop = new asmc::Pop();
  pop->logicalLine = this->logicalLine;
  pop->op = this->registers["%rax"]->get(asmc::QWord);
  file->text << pop;

  return file;
}