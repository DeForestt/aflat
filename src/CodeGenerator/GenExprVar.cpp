#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <iostream>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/GenerationResult.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"

using namespace gen::utils;

namespace gen {

Expr CodeGenerator::genVarExpr(ast::Var var, asmc::File &OutputFile,
                               asmc::Size size) {
  Expr output;
  auto resolved = this->resolveSymbol(var.Ident, var.modList, OutputFile,
                                      var.indices, var.internal);

  if (std::get<2>(resolved) == false) {
    const auto symbol = std::get<1>(resolved);

    std::string ident = var.Ident;
    std::string nsp;
    var.modList.invert();
    var.modList.reset();
    if (this->nameSpaceTable.contains(var.Ident)) {
      nsp = this->nameSpaceTable.get(ident) + ".";
      if (var.modList.trail() == 0)
        alert("NameSpace " + ident + " cannot be used as a variable", true,
              __FILE__, __LINE__);
      ident = nsp + var.modList.shift();
    };

    Type **t = this->typeList[ident];
    if (t != nullptr) {
      Type *type = *t;
      Enum *en = dynamic_cast<Enum *>(type);
      if (en) {
        if (var.modList.trail() == 0) {
          alert("Enum " + ident + " cannot be used as a variable", true,
                __FILE__, __LINE__);
        }
        std::string enumIdent = var.modList.shift();
        Enum::EnumValue *item = en->values[enumIdent];
        if (!item) {
          alert("Enum " + ident + " does not contain " + enumIdent, true,
                __FILE__, __LINE__);
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
            alert("Class " + ident + " does not contain " + functionName, true,
                  __FILE__, __LINE__);
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
    } else if (this->inFunction && var.Ident == "state") {
      auto inScope = gen::scope::ScopeManager::getInstance()->getScope(true);
      gen::Class *cl = new gen::Class();
      cl->Ident = boost::uuids::to_string(boost::uuids::random_generator()());
      int byteMod = 0;
      for (auto sym = inScope.rbegin(); sym != inScope.rend(); ++sym) {
        auto newSym = *sym;
        if (sym->mutable_) {
          gen::scope::ScopeManager::getInstance()->addAssign(sym->symbol);
        }
        byteMod += gen::utils::sizeToInt(sym->type.size);
        newSym.byteMod = byteMod;
        cl->SymbolTable.push(newSym);
        cl->publicSymbols.push(newSym);
      }

      this->typeList.push(cl);
      auto tempDecl = new ast::Declare();
      tempDecl->type = ast::Type(cl->Ident, asmc::QWord);
      tempDecl->ident =
          "***" + boost::uuids::to_string(boost::uuids::random_generator()());
      tempDecl->mut = false;

      auto call = new ast::Call();
      call->ident = cl->Ident;
      call->logicalLine = this->logicalLine;

      auto callExpr = new ast::NewExpr();
      callExpr->type = tempDecl->type;
      callExpr->logicalLine = this->logicalLine;

      auto tempDecAssign = new ast::DecAssign();
      tempDecAssign->declare = tempDecl;
      tempDecAssign->expr = callExpr;
      tempDecAssign->mute = false;
      tempDecAssign->logicalLine = this->logicalLine;

      OutputFile << tempDecAssign->generate(*this).file;

      auto tempVar = new ast::Var();
      tempVar->Ident = tempDecl->ident;
      tempVar->logicalLine = this->logicalLine;

      for (auto sym : inScope) {
        auto assign = new ast::Assign();
        auto v = new ast::Var();
        assign->Ident = tempDecl->ident;
        assign->modList.push(sym.symbol);
        v->Ident = sym.symbol;
        if (sym.type.isReference) v->clean = true;
        assign->expr = v;
        assign->override = true;
        assign->logicalLine = this->logicalLine;
        OutputFile << assign->generate(*this).file;
      }

      output = this->GenExpr(tempVar, OutputFile);
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
      alert("variable not found " + ident, true, __FILE__, __LINE__);
    }
    var.modList.invert();
    var.modList.reset();
  } else {
    gen::Symbol sym = std::get<1>(resolved);
    if (sym.sold != -1) {
      alert("variable " + var.Ident + " was sold on line " +
                std::to_string(sym.sold) + " and cannot be used",
            true, __FILE__, __LINE__);
    }

    if (sym.type.isReference && !var.clean) {
      auto deref = new ast::DeReference();
      deref->Ident = var.Ident;
      deref->modList = var.modList;
      deref->logicalLine = var.logicalLine;
      deref->type = sym.type;
      deref->type.size = sym.type.refSize;
      output = this->GenExpr(deref, OutputFile);
    } else {
      output.size = sym.type.size;
      output.op = sym.type.opType;
      output.type = sym.type.typeName;

      auto cont = true;
      gen::Type **t = this->typeList[sym.type.typeName];
      if (t && !var.selling) {
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
  }

  if (var.typeCast != "") {
    if (output.type != "any" && output.type != "adr" &&
        output.type != "generic")
      this->alert(
          "Can only explicitly cast to a type from any, adr, or generic", true,
          __FILE__, __LINE__);
    output.type = var.typeCast;
  }
  return output;
}

}  // namespace gen
