#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/GenerationResult.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Exceptions.hpp"
#include "Parser/AST/Statements/Call.hpp"
#include "Parser/Parser.hpp"

using namespace gen::utils;

namespace gen {

links::LinkedList<gen::Symbol>
gen::CodeGenerator::GenTable(ast::Statement *STMT,
                             links::LinkedList<gen::Symbol> &table) {
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

    int offset = gen::utils::sizeToInt(arg->type.size);

    if (table.search<std::string>(searchSymbol, arg->ident) != nullptr)
      alert("redefined variable:" + arg->ident, true, __FILE__, __LINE__);

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
    offset = gen::utils::sizeToInt(dec->type.size);

    offset = offset * dec->count;

    if (this->SymbolTable.search<std::string>(searchSymbol, dec->ident) !=
        nullptr)
      alert("redefined variable" + dec->ident, true, __FILE__, __LINE__);

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

asmc::File gen::CodeGenerator::GenArgs(ast::Statement *STMT,
                                       asmc::File &OutputFile,
                                       const ast::Function &func, int &index) {
  asmc::File output;
  if (dynamic_cast<ast::Sequence *>(STMT) != nullptr) {
    ast::Sequence *sequence = dynamic_cast<ast::Sequence *>(STMT);
    output << this->GenArgs(sequence->Statement1, OutputFile, func, index);
    output << this->GenArgs(sequence->Statement2, OutputFile, func, index);
  } else if (dynamic_cast<ast::Declare *>(STMT) != nullptr) {
    /*
        movl $0x0, -[SymbolT + size](rdp)
        **also needs to be added to symbol table**
    */
    ast::Declare *arg = dynamic_cast<ast::Declare *>(STMT);
    if (intArgsCounter > 6) {
      alert("AFlat compiler cannot handle more than 6 int / pointer "
            "arguments.",
            true, __FILE__, __LINE__);
    } else {
      asmc::Size size;
      gen::Symbol symbol;
      asmc::Mov *mov = new asmc::Mov();
      mov->logicalLine = this->logicalLine;

      if (arg->requestType != "") {
        asmc::File dumby;
        auto resolved =
            this->resolveSymbol(arg->requestType, arg->modList, dumby,
                                links::LinkedList<ast::Expr *>());
        if (std::get<2>(resolved)) {
          arg->type = std::get<1>(resolved).type;
        } else if (arg->requestType == "state") {
          gen::Class *cl = new gen::Class();
          auto inScope =
              gen::scope::ScopeManager::getInstance()->getScope(false);
          cl->Ident =
              boost::uuids::to_string(boost::uuids::random_generator()());
          int byteMod = 0;
          for (auto sym = inScope.rbegin(); sym != inScope.rend(); sym++) {
            auto newSym = *sym;

            byteMod += gen::utils::sizeToInt(sym->type.size);
            newSym.byteMod = byteMod;
            cl->SymbolTable.push(newSym);
            cl->publicSymbols.push(newSym);
          }

          this->typeList.push(cl);
          arg->type = ast::Type(cl->Ident, asmc::QWord);
        } else {
          alert(
              "The symbol " + arg->requestType +
                  " is not defined in the current scope so its type cannot be "
                  "resolved",
              true, __FILE__, __LINE__);
        }
      }

      if (parse::PRIMITIVE_TYPES.find(arg->type.typeName) ==
              parse::PRIMITIVE_TYPES.end() &&
          arg->ident != "my" && arg->type.typeName != "any" &&
          arg->type.typeName.find("~") == std::string::npos) {
        this->getType(arg->type.typeName, OutputFile);
      }

      size = arg->type.size;
      arg->type.arraySize = 1;
      mov->from = this->intArgs[intArgsCounter].get(arg->type.size);

      int mod = gen::scope::ScopeManager::getInstance()->assign(
          arg->ident, arg->type, false, arg->mut);

      auto sym = gen::scope::ScopeManager::getInstance()->get(arg->ident);
      if (func.argTypes[index].isRvalue) {
        sym->owned = true;
      }

      mov->size = size;
      mov->to = "-" + std::to_string(mod) + +"(%rbp)";
      OutputFile.text << mov;
      intArgsCounter++;

      // check if the index is in the functions optConvertionIndices
      if (std::find(func.optConvertionIndices.begin(),
                    func.optConvertionIndices.end(),
                    index) != func.optConvertionIndices.end()) {
        auto var = new ast::Var();
        var->Ident = arg->ident;
        var->logicalLine = arg->logicalLine;

        auto call = new ast::CallExpr();
        call->logicalLine = arg->logicalLine;
        call->call = new ast::Call();
        call->call->logicalLine = arg->logicalLine;
        call->call->ident = "option.optionWrapper";
        call->call->Args.push(var);

        auto decAssign = new ast::DecAssign();
        decAssign->logicalLine = arg->logicalLine;
        decAssign->expr = call;
        decAssign->declare = new ast::Declare();
        decAssign->declare->ident = arg->ident;
        decAssign->declare->type =
            ast::Type("option<" + arg->type.typeName + ">", asmc::QWord);
        decAssign->declare->TypeName = "option<" + arg->type.typeName + ">";
        decAssign->declare->mut = arg->mut;
        decAssign->mute = arg->mut;
        decAssign->expr = call;
        decAssign->declare->trust = true;

        output << decAssign->generate(*this).file;
      }
      index++;
    }
  }
  return output;
}

asmc::File gen::CodeGenerator::GenSTMT(ast::Statement *STMT) {
  asmc::File OutputFile = asmc::File();
  this->logicalLine = STMT->logicalLine;

  if (STMT->when && !this->whenSatisfied(*STMT->when)) {
    return OutputFile;
  }

  if (STMT->locked) {
    auto *inst = new asmc::nop();
    inst->logicalLine = this->logicalLine;
    OutputFile.text.push(inst);
  } else
    try {
      if (auto *call = dynamic_cast<ast::Call *>(STMT)) {
        auto result = call->generate(*this);
        OutputFile << result.file;
        if (result.expr.has_value()) {
          const auto &expr = result.expr.value();
          if (expr.type != "void" && parse::PRIMITIVE_TYPES.find(expr.type) ==
                                         parse::PRIMITIVE_TYPES.end()) {
            auto t = this->typeList[expr.type];
            if (t && (*t)->uniqueType) {
              this->alert("Discarding non-primitive return value of type `" +
                              expr.type + "` may leak",
                          false);
              scope::ScopeManager::getInstance()->pushScope(false);
              const auto tempName =
                  "$" + std::to_string(this->tempCount++) + "_unused";
              ast::Type tmpType(expr.type, expr.size);
              tmpType.opType = expr.op;
              auto mod = scope::ScopeManager::getInstance()->assign(
                  tempName, tmpType, false, false);
              if (auto *sym =
                      scope::ScopeManager::getInstance()->get(tempName)) {
                sym->owned = expr.owned;
              }
              auto *mov = new asmc::Mov();
              mov->logicalLine = this->logicalLine;
              mov->size = expr.size;
              mov->op = expr.op;
              mov->from = expr.access;
              mov->to = "-" + std::to_string(mod) + "(%rbp)";
              OutputFile.text << mov;
              scope::ScopeManager::getInstance()->popScope(this, OutputFile);
            }
          }
        }
      } else {
        auto result = STMT->generate(*this);
        OutputFile << result.file;
      }
    } catch (err::Exception &e) {
      this->errorFlag = true;
    }

  return OutputFile;
}

asmc::File gen::CodeGenerator::ImportsOnly(ast::Statement *STMT) {
  asmc::File OutputFile = asmc::File();
  if (STMT->locked) {
    auto *inst = new asmc::nop();
    inst->logicalLine = STMT->logicalLine;
    OutputFile.text.push(inst);
  } else if (dynamic_cast<ast::Sequence *>(STMT) != nullptr) {
    this->ImportsOnly(dynamic_cast<ast::Sequence *>(STMT)->Statement1);
    this->ImportsOnly(dynamic_cast<ast::Sequence *>(STMT)->Statement2);
  } else if (dynamic_cast<ast::Import *>(STMT) != nullptr) {
    auto imp = dynamic_cast<ast::Import *>(STMT);
    if (imp->hasClasses) {
      auto prev = this->cwd;
      if (!imp->cwd.empty())
        this->cwd = imp->cwd;
      if (imp->hasFunctions)
        imp->generateClasses(*this);
      else
        imp->generate(*this);
      this->cwd = prev;
    }
  }
  return OutputFile;
}
} // namespace gen
