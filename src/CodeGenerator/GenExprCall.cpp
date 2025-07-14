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

Expr CodeGenerator::genCallExpr(ast::CallExpr *exprCall, asmc::File &OutputFile,
                                asmc::Size size) {
  Expr output;
  auto call = exprCall->call;
  Type **t = this->typeList[call->ident];
  bool genericCall = false;

  if (t == nullptr) {
    auto cls = this->genericTypes[call->ident];
    if (cls != nullptr) {
      genericCall = true;
      std::string new_class_name;
      t = this->instantiateGenericClass(cls, exprCall->templateTypes,
                                        new_class_name, OutputFile);
      call->ident = new_class_name;
      if (t == nullptr) {
        alert("Something went wrong with the generic class " + call->ident +
                  " in " + this->moduleId,
              true, __FILE__, __LINE__);
      }
    }
  }

  if (!genericCall && exprCall->templateTypes.size() != 0) {
    call->genericTypes = exprCall->templateTypes;
  }

  if (t != nullptr) {
    gen::Class *cl = dynamic_cast<gen::Class *>(*t);
    if (cl != nullptr) {
      if (cl->dynamic)
        alert("Dynamic class '" + cl->Ident + "' must be called with new", true,
              __FILE__, __LINE__);
      ast::Type type = ast::Type();
      type.typeName = cl->Ident;
      type.size = asmc::Byte;
      type.arraySize = cl->SymbolTable.head->data.byteMod;
      int bMod =
          gen::scope::ScopeManager::getInstance()->assign("", type, false);

      asmc::Lea *lea = new asmc::Lea();
      lea->to = this->registers["%rax"]->get(asmc::QWord);
      lea->from = '-' + std::to_string(bMod) + "(%rbp)";
      lea->logicalLine = this->logicalLine;
      OutputFile.text << lea;
      std::string pointer = registers["%rax"]->get(asmc::QWord);

      ast::Function *init = cl->nameTable["init"];

      if (init == nullptr) {
        output.access = pointer;
        output.size = asmc::QWord;
        output.type = cl->Ident;
      } else {
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
        Expr afterInit = this->GenExpr(callInit, OutputFile);
        output.access = afterInit.access;
        output.size = asmc::QWord;
        output.type = cl->Ident;
      }
    } else {
      alert("Class " + call->ident + " not found", true, __FILE__, __LINE__);
    }
  } else {
    auto callGen = call->generate(*this);
    OutputFile << callGen.file;
    output = callGen.expr.value();
    if (size != asmc::AUTO &&
        (output.type == "any" || output.type == "--std--flex--function"))
      output.size = size;
    output.access = this->registers["%rax"]->get(output.size);
    if (output.type == "float") {
      output.access = this->registers["%xmm0"]->get(output.size);
      output.op = asmc::Float;
    }
  }
  if (exprCall->typeCast != "") {
    if (output.type != "any" && output.type != "adr" &&
        output.type != "generic")
      this->alert(
          "Can only explicitly cast to a type from any, adr, or generic", true,
          __FILE__, __LINE__);
    output.type = exprCall->typeCast;
  }
  return output;
}

}  // namespace gen
