#include "Parser/AST/Statements/Call.hpp"

#include <algorithm>
#include <exception>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Parser.hpp"

namespace {
struct OverloadRetry : public std::exception {
  int nextIndex;
  links::SLinkedList<ast::Function, std::string> *table;
  std::string ident;
  std::string message;
  bool fatal;
  std::optional<int> fallbackIndex;
  bool allowDiscardWarningFallback;
  OverloadRetry(int idx,
                links::SLinkedList<ast::Function, std::string> *sourceTable,
                std::string functionIdent, std::string reason,
                bool fatalError = true,
                std::optional<int> fallback = std::nullopt,
                bool allowDiscardWarning = false)
      : nextIndex(idx), table(sourceTable), ident(std::move(functionIdent)),
        message(std::move(reason)), fatal(fatalError),
        fallbackIndex(std::move(fallback)),
        allowDiscardWarningFallback(allowDiscardWarning) {}
  const char *what() const noexcept override { return message.c_str(); }
};
} // namespace

namespace ast {

gen::GenerationResult const Call::generate(gen::CodeGenerator &generator) {
  std::optional<int> forcedOverloadIndex;
  links::SLinkedList<ast::Function, std::string> *forcedTable = nullptr;
  std::string forcedIdent;
  std::string lastError;
  struct PendingRetryLog {
    std::string reason;
    bool clearOnSuccess = true;
  };
  std::vector<PendingRetryLog> pendingRetryLogs;
  struct PendingFallback {
    int index;
    links::SLinkedList<ast::Function, std::string> *table;
    std::string ident;
    bool allowDiscardWarning;
    size_t retryLogIndex;
  };
  std::optional<PendingFallback> discardWarningFallback;
  bool allowDiscardWarningForNextAttempt = false;
  std::optional<size_t> nextOverloadReasonIndex;

  while (true) {
    if (forcedTable != nullptr && forcedOverloadIndex.has_value()) {
      std::string candidate = forcedIdent;
      if (forcedOverloadIndex.value() > 0) {
        candidate += "_ovl" + std::to_string(forcedOverloadIndex.value());
      }
      if ((*forcedTable)[candidate] == nullptr) {
        if (discardWarningFallback.has_value()) {
          forcedOverloadIndex = discardWarningFallback->index;
          forcedTable = discardWarningFallback->table;
          forcedIdent = discardWarningFallback->ident;
          allowDiscardWarningForNextAttempt =
              discardWarningFallback->allowDiscardWarning;
          if (discardWarningFallback->retryLogIndex < pendingRetryLogs.size()) {
            pendingRetryLogs[discardWarningFallback->retryLogIndex]
                .clearOnSuccess = false;
          }
          nextOverloadReasonIndex = discardWarningFallback->retryLogIndex;
          discardWarningFallback.reset();
          continue;
        }
        generator.alert(lastError.empty()
                            ? "No matching overload for call to " + candidate
                            : lastError,
                        true, __FILE__, __LINE__);
      }
    }

    bool currentAllowDiscardWarning = allowDiscardWarningForNextAttempt;
    Call attempt(*this);
    attempt.allowDiscardWarning = currentAllowDiscardWarning;
    allowDiscardWarningForNextAttempt = false;
    try {
      auto result = attempt.generateAttempt(generator, forcedOverloadIndex,
                                            forcedTable, forcedIdent);
      pendingRetryLogs.clear();
      nextOverloadReasonIndex.reset();
      return result;
    } catch (const OverloadRetry &retry) {
      pendingRetryLogs.push_back(PendingRetryLog{retry.message, true});
      size_t retryLogIndex = pendingRetryLogs.size() - 1;
      if (!retry.fatal && retry.fallbackIndex.has_value() &&
          !discardWarningFallback.has_value()) {
        discardWarningFallback = PendingFallback{
            retry.fallbackIndex.value(), retry.table, retry.ident,
            retry.allowDiscardWarningFallback, retryLogIndex};
      } else if (retry.fatal) {
        lastError = retry.message;
      }
      nextOverloadReasonIndex = retryLogIndex;
      forcedOverloadIndex = retry.nextIndex;
      forcedTable = retry.table;
      forcedIdent = retry.ident;
      allowDiscardWarningForNextAttempt = false;
      continue;
    }
  }
}

gen::GenerationResult Call::generateAttempt(
    gen::CodeGenerator &generator, std::optional<int> forcedOverloadIndex,
    links::SLinkedList<ast::Function, std::string> *forcedTable,
    const std::string &forcedIdent) {
  auto flexFunction = ast::Function();
  auto callFunction = ast::Function();
  auto shiftedFunction = ast::Function();
  auto publifyedFunction = ast::Function();
  bool immutableSymbol = false;

  auto file = asmc::File();
  std::string mod = "";
  ast::Function *func;
  bool checkArgs = true;
  links::SLinkedList<ast::Function, std::string> *overloadTable = nullptr;
  std::string overloadIdent;
  int currentOverloadIndex = -1;
  this->modList.invert();
  this->modList.reset();
  std::string ident = this->ident;

  if (this->imbeddedNamespace.has_value()) {
    ident = this->imbeddedNamespace.value() + "." + this->modList.shift();
  }

  std::string nsp;
  if (generator.nameSpaceTable.contains(ident)) {
    nsp = generator.nameSpaceTable.get(ident) + ".";
    ident = nsp + this->modList.shift();
  }

  links::LinkedList<std::string> mods = links::LinkedList<std::string>();
  // first push rdx
  links::LinkedList<std::string> stack;

  generator.intArgsCounter = 0;
  int argsCounter = 0;
  std::string allMods = "";
  if (this->modList.pos == nullptr) {
    func = findFunctionByOverload(
        generator.nameTable, ident, forcedOverloadIndex, forcedTable,
        forcedIdent, overloadTable, overloadIdent, currentOverloadIndex);
    if (func == nullptr) {
      auto f = findFunctionByOverload(
          generator.genericFunctions, ident, forcedOverloadIndex, forcedTable,
          forcedIdent, overloadTable, overloadIdent, currentOverloadIndex);
      if (f != nullptr) {
        auto junkFile = asmc::File();
        func = dynamic_cast<ast::Function *>(deepCopy(f));
        // get the argument types and create the map...
        std::unordered_map<std::string, std::string> genericMap;
        // loop through func.argTypes
        auto new_ident = func->ident.ident;

        for (int i = 0; i < this->genericTypes.size(); i++) {
          genericMap[func->genericTypes[i]] = this->genericTypes[i];
          new_ident += "." + this->genericTypes[i];
        }

        this->Args.invert();
        for (int i = 0; i < func->argTypes.size(); i++) {
          auto type = func->argTypes[i];

          for (const auto &genericType : func->genericTypes) {
            if (type.typeName == genericType &&
                genericMap.find(genericType) == genericMap.end()) {
              auto exprType =
                  generator.GenExpr(this->Args.get(i), junkFile).type;
              genericMap[genericType] = exprType;
              new_ident += "." + exprType;
            }
          }
        }
        this->Args.invert();

        func->replaceTypes(genericMap);
        func->ident.ident = new_ident;
        func->genericTypes.clear();
        func->scope = ast::ScopeMod::Private;
        func->globalLocked = true;
        func->wasGeneric = true;

        bool generated = false;

        if (file.lambdas == nullptr) {
          file.lambdas = new asmc::File();
          file.hasLambda = true;
        }
        auto saveReturnType = generator.returnType;
        if (generator.generatedFunctionNames.find(new_ident) ==
            generator.generatedFunctionNames.end()) {
          gen::scope::ScopeManager::getInstance()->pushScope(true);
          file.lambdas->operator<<(generator.GenSTMT(func));
          gen::scope::ScopeManager::getInstance()->popScope(&generator, file);
          generator.generatedFunctionNames.insert(new_ident);
          generated = true;
        } else {
          gen::scope::ScopeManager::getInstance()->pushScope(true);
          generator.GenSTMT(func);
          gen::scope::ScopeManager::getInstance()->popScope(&generator, file);
          generator.generatedFunctionNames.insert(new_ident);
          generated = true;
        }
        generator.returnType = saveReturnType;

        this->ident = func->ident.ident;
        // get func from the name table so that it can be used with generated
        // return type
        func = findFunctionByOverload(generator.nameTable, func->ident.ident,
                                      forcedOverloadIndex, forcedTable,
                                      forcedIdent, overloadTable, overloadIdent,
                                      currentOverloadIndex);
        if (func == nullptr) {
          generator.alert(
              "cannot find function after generic generation (this is a bug "
              "please report it): `" +
                  this->ident + "`" + (generated ? " (generated)" : ""),
              true, __FILE__, __LINE__);
        }
        ident = this->ident;
      }
    }
    if (func == nullptr) {
      gen::Symbol *smbl = gen::scope::ScopeManager::getInstance()->get(ident);
      if (smbl == nullptr)
        smbl = generator.GlobalSymbolTable.search<std::string>(
            gen::utils::searchSymbol, ident);
      if (smbl != nullptr) {
        overloadTable = nullptr;
        overloadIdent.clear();
        currentOverloadIndex = -1;
        if (smbl->type.typeName == "adr" ||
            smbl->type.fPointerArgs.isFPointer) {
          ast::Var *var = new ast::Var();
          var->logicalLine = this->logicalLine;
          var->Ident = smbl->symbol;
          var->modList = links::LinkedList<std::string>();

          gen::Expr exp1 = generator.GenExpr(var, file);

          asmc::Mov *mov = new asmc::Mov();
          mov->logicalLine = this->logicalLine;
          mov->size = exp1.size;
          mov->from = exp1.access;
          mov->to = generator.registers["%r11"]->get(exp1.size);
          file.text << mov;

          func = &flexFunction;
          func->logicalLine = this->logicalLine;
          func->ident.ident = '*' + generator.registers["%r11"]->get(exp1.size);
          func->type.arraySize = 0;
          func->type.size = asmc::QWord;
          func->type.typeName = "--std--flex--function";
          func->useType = func->type;
          checkArgs = false;
          func->flex = true;
          if (smbl->type.fPointerArgs.returnType != nullptr) {
            func->type = *smbl->type.fPointerArgs.returnType;
            func->argTypes = smbl->type.fPointerArgs.argTypes;
            func->req = smbl->type.fPointerArgs.requiredArgs;
            func->optConvertionIndices =
                smbl->type.fPointerArgs.optConvertionIndices;
            checkArgs = true;
          }
          func->useType = func->type;
        } else {
          // find the type of the object
          auto type = *generator.getType(smbl->type.typeName, file);
          auto cl = dynamic_cast<gen::Class *>(type);
          if (cl == nullptr)
            generator.alert("type is not a class " + smbl->type.typeName);
          auto f = findFunctionByOverload(
              cl->nameTable, "_call", forcedOverloadIndex, forcedTable,
              forcedIdent, overloadTable, overloadIdent, currentOverloadIndex);
          auto tname = smbl->type.typeName;
          if (f == nullptr) {
            // check the parent
            auto parent = cl->parent;
            if (parent != nullptr) {
              f = findFunctionByOverload(parent->nameTable, "_call",
                                         forcedOverloadIndex, forcedTable,
                                         forcedIdent, overloadTable,
                                         overloadIdent, currentOverloadIndex);
              tname = parent->Ident;
              if (f == nullptr)
                generator.alert("cannot preform this on type " +
                                smbl->type.typeName +
                                " because it does not implement the "
                                "_call function");
            } else {
              generator.alert("cannot preform this on type " +
                              smbl->type.typeName +
                              " because it does not implement the _call "
                              "function");
            }
          }

          overloadTable = nullptr;
          overloadIdent.clear();
          currentOverloadIndex = -1;
          func = &callFunction;
          func->ident.ident = "pub_" + tname + "__call";
          func->type = f->type;
          func->req = f->req;
          func->optConvertionIndices = f->optConvertionIndices;
          func->argTypes = f->argTypes;
          auto var = new ast::Var();
          var->logicalLine = this->logicalLine;
          var->Ident = smbl->symbol;

          auto exp1 = generator.GenExpr(var, file);

          auto mov = new asmc::Mov();
          mov->logicalLine = this->logicalLine;
          mov->size = exp1.size;
          mov->from = exp1.access;
          mov->to = generator.intArgs[argsCounter].get(exp1.size);
          file.text << mov;
          argsCounter++;
        }
      };
    }
  } else {
    std::string pubname;
    gen::Symbol *sym = gen::scope::ScopeManager::getInstance()->get(ident);
    if (sym == nullptr) {
      generator.alert("cannot find object: " + ident);
    } else {
      immutableSymbol = immutableSymbol || sym->readOnly;
      allMods += sym->symbol + ".";
    }

    ast::Type last = sym->type;
    std::string my = sym->symbol;
    // get the type of the original function

    int modCount = 0;
    while (this->modList.pos != nullptr) {
      auto type = *generator.getType(last.typeName, file);
      gen::Class *cl = dynamic_cast<gen::Class *>(type);
      if (cl != nullptr) {
        bool addPub = true;
        pubname = cl->Ident;
        std::string memberIdent = this->modList.touch();
        func = findFunctionByOverload(
            cl->nameTable, memberIdent, forcedOverloadIndex, forcedTable,
            forcedIdent, overloadTable, overloadIdent, currentOverloadIndex);
        gen::Class *parent = cl->parent;
        if (func == nullptr && parent) {
          func = findFunctionByOverload(
              parent->nameTable, memberIdent, forcedOverloadIndex, forcedTable,
              forcedIdent, overloadTable, overloadIdent, currentOverloadIndex);
          if (func != nullptr)
            pubname = parent->Ident;
        }
        bool shift = true;
        if (func == nullptr) {
          std::string id = memberIdent;
          mods.push(id);
          sym = cl->SymbolTable.search<std::string>(gen::utils::searchSymbol,
                                                    memberIdent);
          if (sym != nullptr && (sym->type.typeName == "adr" ||
                                 sym->type.fPointerArgs.isFPointer)) {
            ast::Var *var = new ast::Var();
            var->logicalLine = this->logicalLine;
            var->Ident = ident;
            mods.invert();
            var->modList = mods;

            gen::Expr exp1 = generator.GenExpr(var, file);

            asmc::Mov *mov = new asmc::Mov();
            mov->logicalLine = this->logicalLine;
            mov->size = exp1.size;
            mov->from = exp1.access;
            mov->to = generator.registers["%r11"]->get(exp1.size);
            file.text << mov;

            //

            overloadTable = nullptr;
            overloadIdent.clear();
            currentOverloadIndex = -1;
            func = &flexFunction;
            func->logicalLine = this->logicalLine;
            func->ident.ident =
                '*' + generator.registers["%r11"]->get(exp1.size);
            func->type = sym->type;
            func->type.typeName = "--std--flex--function";
            checkArgs = false;
            func->type.size = asmc::QWord;
            func->flex = true;
            func->useType = func->type;

            if (sym->type.fPointerArgs.returnType != nullptr) {
              func->type = *sym->type.fPointerArgs.returnType;
              func->argTypes = sym->type.fPointerArgs.argTypes;
              func->req = sym->type.fPointerArgs.requiredArgs;
              func->optConvertionIndices =
                  sym->type.fPointerArgs.optConvertionIndices;
              checkArgs = true;
            }
            func->useType = func->type;

            addPub = false;
          } else if (sym == nullptr) {
            generator.alert("cannot find function " + memberIdent, true,
                            __FILE__, __LINE__);
          } else if (this->modList.pos->next == nullptr) {
            // find the type of the object
            auto objectType = *generator.getType(sym->type.typeName, file);
            auto objectClass = dynamic_cast<gen::Class *>(objectType);
            if (objectClass == nullptr)
              generator.alert("type is not a class " + sym->type.typeName);
            auto f = findFunctionByOverload(
                objectClass->nameTable, "_call", forcedOverloadIndex,
                forcedTable, forcedIdent, overloadTable, overloadIdent,
                currentOverloadIndex);
            if (f == nullptr)
              generator.alert("cannot preform this on type " +
                              sym->type.typeName +
                              " because it does not implement the _call "
                              "function");

            overloadTable = nullptr;
            overloadIdent.clear();
            currentOverloadIndex = -1;
            func = &shiftedFunction;
            func->ident = f->ident;
            func->type = f->type;
            func->req = f->req;
            func->argTypes = f->argTypes;
            func->optConvertionIndices = f->optConvertionIndices;
            func->safe = f->safe;
            pubname = sym->type.typeName;
            shift = false;
          }
        }
        if (func != nullptr) {
          this->modList.shift();
          this->modList.invert();
          this->modList.reset();
          ast::Reference *ref = new ast::Reference();
          ref->logicalLine = this->logicalLine;
          ref->Ident = my;
          ref->modList = this->modList;
          if (shift)
            ref->internal = true;
          mod = "pub_" + pubname + "_";
          if (!addPub)
            mod = "";

          if (immutableSymbol && !func->safe) {
            generator.alert("Immutable objects can only call safe functions: " +
                                allMods + mod + func->ident.ident,
                            true, __FILE__, __LINE__);
          }

          gen::Expr exp;
          exp = generator.GenExpr(ref, file);
          asmc::Mov *mov = new asmc::Mov();
          mov->logicalLine = this->logicalLine;
          asmc::Mov *mov2 = new asmc::Mov();
          mov2->logicalLine = this->logicalLine;
          asmc::Push *push = new asmc::Push();
          push->logicalLine = this->logicalLine;

          mov->size = exp.size;
          mov2->size = exp.size;

          mov->from = '(' + exp.access + ')';
          mov->to = generator.registers["%eax"]->get(exp.size);
          mov2->from = generator.registers["%eax"]->get(exp.size);
          mov2->to = generator.intArgs[argsCounter].get(exp.size);
          push->op = generator.intArgs[argsCounter].get(asmc::QWord);
          stack << generator.intArgs[argsCounter].get(asmc::QWord);

          argsCounter++;
          file.text << push;
          file.text << mov;
          file.text << mov2;

          if (!shift) {
            this->modList.shift();
            this->modList.invert();
            this->modList.reset();
            auto var = new ast::Var();
            var->logicalLine = this->logicalLine;
            var->Ident = ident;
            var->modList = this->modList;
            var->internal = true;
            this->Args.invert();
            this->Args.push(var);
            this->Args.invert();
          }
          break;
        }
      }
      mods.push(this->modList.shift());
      immutableSymbol = immutableSymbol || sym->readOnly;
      last = sym->type;
      allMods += sym->symbol + ".";
      modCount++;
    };
  };

  if (this->publify != "") {
    func = &publifyedFunction;
    func->ident.ident = "pub_" + this->publify + "_" + ident;
    func->type.typeName = this->publify;
    func->type.size = asmc::QWord;
    func->scopeName = this->publify;
    if (ident != "init" && ident != "del" && !this->slick) {
      // find the function in the class
      gen::Class *cl =
          dynamic_cast<gen::Class *>(*generator.typeList[this->publify]);
      if (cl != nullptr) {
        ast::Function *f = findFunctionByOverload(
            cl->nameTable, ident, forcedOverloadIndex, forcedTable, forcedIdent,
            overloadTable, overloadIdent, currentOverloadIndex);
        if (f == nullptr) {
          generator.alert("cannot find function: " + ident + " in class " +
                          this->publify);
        };
        func->type = f->type;
        func->scope = f->scope;
        func->scopeName = f->scopeName;
        ast::Type t;
        t.typeName = cl->Ident;
        t.size = asmc::QWord;
        func->argTypes = f->argTypes;
        func->argTypes.push_back(t);
        if (immutableSymbol && !f->safe) {
          generator.alert("Immutable objects can only call safe functions: " +
                              func->ident.ident,
                          true, __FILE__, __LINE__);
        }
      };
    } else {
      argsCounter++;
      stack.push("%rdi");
      // find the class
      gen::Type *type = *generator.getType(this->publify, file);
      gen::Class *cl = dynamic_cast<gen::Class *>(type);
      if (cl == nullptr)
        generator.alert("not a class: " + this->publify);
      ast::Function *f = findFunctionByOverload(
          cl->nameTable, ident, forcedOverloadIndex, forcedTable, forcedIdent,
          overloadTable, overloadIdent, currentOverloadIndex);
      if (f == nullptr)
        generator.alert("cannot find function: " + ident + " in " + cl->Ident);
      func->argTypes = f->argTypes;
      func->req = f->req;
      if (immutableSymbol && !f->safe) {
        generator.alert("Immutable objects can only call safe functions: " +
                            func->ident.ident,
                        true, __FILE__, __LINE__);
      }
    }
    mod = "";
  }

  asmc::Push *push = new asmc::Push();
  push->logicalLine = this->logicalLine;
  push->op = generator.registers["%rdx"]->get(asmc::QWord);
  file.text << push;
  stack << push->op;

  if (func == nullptr) {
    generator.alert("Cannot Find Function: " + ident + allMods, true, __FILE__,
                    __LINE__);
  }

  if (func->scope == ast::Private && func->scopeName != "global") {
    if (generator.scope == nullptr)
      generator.alert("attempt to this private function: " + allMods +
                      func->ident.ident + " from global scope");
    if (func->scopeName != generator.scope->Ident)
      generator.alert("Attempt to this private function " + allMods +
                      func->ident.ident + " from wrong scope: " +
                      generator.scope->Ident + " != " + func->scopeName);
  }

  this->Args.invert();
  this->Args.reset();

  links::LinkedList<ast::Expr *> args;
  std::vector<asmc::Instruction *> overflowArgs;
  int i = 0;
  if (this->Args.trail() < func->req)
    this->requestOverloadRetry(
        generator, overloadTable, overloadIdent, currentOverloadIndex,
        "Too few arguments for function: " + ident +
            " expected: " + std::to_string(func->argTypes.size()) + " got: 1");

  // create a string of argTypes
  std::string argTypesString = "";
  for (auto arg : func->argTypes) {
    argTypesString += arg.typeName + ", ";
  }
  if (argTypesString.size() > 1)
    argTypesString = argTypesString.substr(0, argTypesString.size() - 2);

  while (this->Args.trail() > 0) {
    args.push(this->Args.touch());
    ast::Expr *rem = this->Args.touch();
    ast::Expr *arg = this->Args.shift();
    // check if the argument is a reference
    std::string typeHint = "";
    bool rValue = false;
    if (checkArgs) {
      auto var = dynamic_cast<ast::Var *>(arg);
      if (var != nullptr) {
        auto sym = gen::scope::ScopeManager::getInstance()->get(var->Ident);
        if (!sym) {
          sym = generator.GlobalSymbolTable.search<std::string>(
              gen::utils::searchSymbol, var->Ident);
        }
        if (sym && func) {
          if (sym->readOnly && !func->readOnly[i]) {
            this->requestOverloadRetry(
                generator, overloadTable, overloadIdent, currentOverloadIndex,
                "Cannot pass immutable variable `" + var->Ident +
                    "` to mutable function argument: " + func->ident.ident);
          }
        }
      }
      if (i >= func->argTypes.size()) {
        generator.logicalLine = arg->logicalLine;
        this->requestOverloadRetry(
            generator, overloadTable, overloadIdent, currentOverloadIndex,
            "Too many arguments for function: " + ident +
                " expected: " + std::to_string(func->argTypes.size()) +
                " got: " + std::to_string(i + 1));
      }
      if (func->argTypes.at(i).isReference) {
        auto toReg = new ast::Reference();
        auto var = dynamic_cast<ast::Var *>(arg);
        if (var == nullptr) {
          this->requestOverloadRetry(
              generator, overloadTable, overloadIdent, currentOverloadIndex,
              "Attempted to pass an rvalue to a reference");
        }
        auto sym = gen::scope::ScopeManager::getInstance()->get(var->Ident);
        if (!sym) {
          sym = generator.GlobalSymbolTable.search<std::string>(
              gen::utils::searchSymbol, var->Ident);
        }

        if (!sym) {
          generator.alert("cannot find symbol: " + var->Ident);
        }
        if (sym->mutable_ == false && func->mutability[i]) {
          this->requestOverloadRetry(
              generator, overloadTable, overloadIdent, currentOverloadIndex,
              "cannot pass a const reference to a mutable argument: " +
                  var->Ident);
        } else if (func->mutability[i]) {
          gen::scope::ScopeManager::getInstance()->addAssign(sym->symbol);
        }
        if (!var) {
          this->requestOverloadRetry(generator, overloadTable, overloadIdent,
                                     currentOverloadIndex,
                                     "A reference can only point to an lvalue");
        }
        toReg->Ident = var->Ident;
        toReg->modList = var->modList;
        toReg->logicalLine = var->logicalLine;
        arg = toReg;
      }
      if (func->argTypes.at(i).isRvalue) {
        rValue = true;
        // make sure that its not a var
        auto var = dynamic_cast<ast::Var *>(arg);
        if (var != nullptr && var->Ident != "NULL") {
          this->requestOverloadRetry(
              generator, overloadTable, overloadIdent, currentOverloadIndex,
              "Attempted to pass an lvalue (" + var->Ident + ") to an rvalue");
        }
      }
      typeHint = func->argTypes.at(i).typeName;
    }
    if (arg == nullptr) {
      generator.alert("Argument " + std::to_string(i + 1) +
                      " is null in function call: " + ident);
    }
    gen::Expr exp = generator.GenExpr(arg, file, asmc::AUTO, typeHint);
    if (!exp.passable)
      this->requestOverloadRetry(
          generator, overloadTable, overloadIdent, currentOverloadIndex,
          "Cannot pass an lvalue of safe type " + exp.type + " to a function");
    if (checkArgs && dynamic_cast<ast::CallExpr *>(arg) != nullptr &&
        !func->argTypes.at(i).isRvalue && exp.type != "void" &&
        parse::PRIMITIVE_TYPES.find(exp.type) == parse::PRIMITIVE_TYPES.end()) {
      auto t = generator.typeList[exp.type];
      if (t && (*t)->uniqueType) {
        std::string discardWarning =
            "Discarding non-primitive return value of type `" + exp.type +
            "` that is passed to argument " + std::to_string(i + 1) +
            " of function `" + ident +
            "` without transferring ownership may leak";
        bool hasAlternativeOverload = false;
        if (!this->allowDiscardWarning && overloadTable != nullptr &&
            currentOverloadIndex >= 0 && !overloadIdent.empty()) {
          int nextIndex = currentOverloadIndex + 1;
          if (nextIndex > 0) {
            std::string nextCandidate = overloadIdent;
            nextCandidate += "_ovl" + std::to_string(nextIndex);
            hasAlternativeOverload = (*overloadTable)[nextCandidate] != nullptr;
          }
        }
        if (hasAlternativeOverload) {
          this->requestOverloadRetry(generator, overloadTable, overloadIdent,
                                     currentOverloadIndex, discardWarning,
                                     false, true, true);
        } else {
          generator.alert(discardWarning, false);
        }
      }
    }
    if (!exp.owned && rValue) {
      if (parse::PRIMITIVE_TYPES.find(exp.type) ==
          parse::PRIMITIVE_TYPES.end()) {
        this->requestOverloadRetry(
            generator, overloadTable, overloadIdent, currentOverloadIndex,
            "Attempted to pass an unowned rvalue of type `" + exp.type +
                "` to a function argument that expects to take ownership.  "
                "Consider using the sell operator `$` or ensure that the "
                "value is dynamically allocated (statically allocated "
                "references ar considered to be borrowed from the stack)");
      }
    }

    if (checkArgs) {
      if (i >= func->argTypes.size()) {
        generator.logicalLine = arg->logicalLine;
        this->requestOverloadRetry(
            generator, overloadTable, overloadIdent, currentOverloadIndex,
            "Too many arguments for function: " + ident +
                " expected: " + std::to_string(func->argTypes.size()) +
                " got: " + std::to_string(i + 1));
      };
      if (!generator.canAssign(func->argTypes.at(i), exp.type,
                               "function " + ident +
                                   " expected type `{}` but received type `{}` "
                                   "these types are irreconcilable "
                                   "the argument type(s) are (" +
                                   argTypesString + ")")) {
        ast::Expr *init = generator.imply(rem, func->argTypes.at(i).typeName);
        exp = generator.GenExpr(init, file);
      };
    };
    i++;
    if (exp.op == asmc::Float) {
      ast::Type fl = ast::Type();
      fl.typeName = "float";
      fl.size = asmc::DWord;
      int bytemod =
          gen::scope::ScopeManager::getInstance()->assign("", fl, false, false);
      asmc::Mov *mov = new asmc::Mov();
      mov->op = asmc::Float;
      mov->logicalLine = this->logicalLine;
      mov->size = exp.size;
      mov->from = exp.access;
      mov->to = generator.registers["%xmm0"]->get(exp.size);
      file.text << mov;

      asmc::Mov *mov2 = new asmc::Mov();
      mov2->op = asmc::Float;
      mov2->logicalLine = this->logicalLine;
      mov2->size = exp.size;
      mov2->from = generator.registers["%xmm0"]->get(exp.size);
      mov2->to = "-" + std::to_string(bytemod) + "(%rbp)";
      file.text << mov2;
      // move to eax

      // clear out eax
      asmc::Xor *xory = new asmc::Xor();
      xory->logicalLine = this->logicalLine;
      xory->op1 = generator.registers["%eax"]->get(asmc::QWord);
      xory->op2 = generator.registers["%eax"]->get(asmc::QWord);
      file.text << xory;

      asmc::Mov *mov3 = new asmc::Mov();
      mov3->logicalLine = this->logicalLine;
      mov3->size = asmc::DWord;
      mov3->from = "-" + std::to_string(bytemod) + "(%rbp)";
      mov3->to = generator.registers["%eax"]->get(asmc::DWord);
      file.text << mov3;

      asmc::Push *pushArg = new asmc::Push();
      pushArg->logicalLine = this->logicalLine;

      pushArg->op = generator.intArgs[argsCounter].get(asmc::QWord);

      file.text << pushArg;

      asmc::Mov *mov4 = new asmc::Mov();
      mov4->logicalLine = this->logicalLine;
      mov4->size = asmc::DWord;
      mov4->from = generator.registers["%eax"]->get(asmc::DWord);
      mov4->to = generator.intArgs[argsCounter].get(asmc::DWord);
      file.text << mov4;
    } else {
      asmc::Mov *mov = new asmc::Mov();
      mov->logicalLine = this->logicalLine;
      asmc::Mov *mov2 = new asmc::Mov();
      mov2->logicalLine = this->logicalLine;
      asmc::Push *pushArg = new asmc::Push();
      pushArg->logicalLine = this->logicalLine;
      mov->size = exp.size;
      mov2->size = exp.size;

      mov->from = exp.access;
      mov->to = generator.registers["%eax"]->get(exp.size);
      mov2->from = generator.registers["%eax"]->get(exp.size);
      mov2->to = generator.intArgs[argsCounter].get(exp.size);
      pushArg->op = generator.intArgs[argsCounter].get(asmc::QWord);
      stack << generator.intArgs[argsCounter].get(asmc::QWord);

      file.text << pushArg;
      file.text << mov;
      file.text << mov2;
    };
    argsCounter++;
  };

  int argsUsed = argsCounter;
  if (this->publify != "")
    argsUsed--;

  while (argsUsed < func->argTypes.size()) {
    // if the argument is a float, we need to push a float
    asmc::Mov *move = new asmc::Mov();
    move->logicalLine = this->logicalLine;
    move->size = asmc::QWord;
    move->from = "$0";
    move->to = generator.intArgs[argsCounter].get(asmc::QWord);
    argsCounter++;
    argsUsed++;
    file.text << move;
  }

  // add the overflow arguments
  for (auto inst : overflowArgs) {
    file.text << inst;
  }

  // this->Args = args;
  this->Args.invert();
  asmc::Call *calls = new asmc::Call;
  calls->logicalLine = this->logicalLine;

  calls->function = mod + func->ident.ident;
  file.text << calls;
  // push everything back on the stack
  while (stack.count > 0) {
    asmc::Pop *pop = new asmc::Pop();
    pop->logicalLine = this->logicalLine;
    pop->op = stack.pop();
    file.text << pop;
  }
  generator.intArgsCounter = 0;
  return {file, std::optional<gen::Expr>(func->toExpr(generator))};
}

ast::Function *Call::findFunctionByOverload(
    links::SLinkedList<ast::Function, std::string> &table,
    const std::string &ident, std::optional<int> forcedOverloadIndex,
    links::SLinkedList<ast::Function, std::string> *forcedTable,
    const std::string &forcedIdent,
    links::SLinkedList<ast::Function, std::string> *&activeTable,
    std::string &activeIdent, int &activeIndex) {
  std::string lookupIdent = ident;
  if (forcedTable == &table && forcedOverloadIndex.has_value() &&
      forcedIdent == ident && forcedOverloadIndex.value() > 0) {
    lookupIdent += "_ovl" + std::to_string(forcedOverloadIndex.value());
  }
  ast::Function *resolved = table[lookupIdent];
  if (resolved != nullptr) {
    activeTable = &table;
    activeIdent = ident;
    activeIndex = resolved->overloadIndex;
  }
  return resolved;
}

void Call::requestOverloadRetry(
    gen::CodeGenerator &generator,
    links::SLinkedList<ast::Function, std::string> *table,
    const std::string &ident, int currentIndex, const std::string &message,
    bool fatal, bool allowFallbackToCurrent,
    bool allowDiscardWarningOnFallback) {
  if (table == nullptr || currentIndex < 0) {
    generator.alert(message, true, __FILE__, __LINE__);
  }
  std::optional<int> fallbackIndex = std::nullopt;
  if (allowFallbackToCurrent) {
    fallbackIndex = currentIndex;
  }
  throw OverloadRetry(currentIndex + 1, table, ident, message, fatal,
                      fallbackIndex, allowDiscardWarningOnFallback);
}

std::string Call::toString() {
  std::string result = ident;
  for (int i = 0; i < modList.count; i++) {
    result += "." + modList.get(i);
  }
  result += "(";
  for (int i = 0; i < Args.count; i++) {
    result += Args.get(i)->toString();
    if (i != Args.count - 1) {
      result += ", ";
    }
  }
  result += ")";

  return result;
} // namespace ast

std::string CallExpr::toString() { return call->toString(); }

} // namespace ast
