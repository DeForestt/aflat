#include "Parser/AST/Statements/Assign.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Expr.hpp"
#include "CodeGenerator/Scope/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Parser.hpp"

namespace ast {
Assign::Assign(const std::string &ident,
               const links::LinkedList<ast::Expr *> &indices,
               links::LinkedList<std::string> modList,
               links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->logicalLine = lex::tokenLine(tokens.peek());
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto s2 = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (s2->Sym == ':') {
      this->reference = true;
      tokens.pop();
    }
  } else if (dynamic_cast<lex::Symbol *>(tokens.peek()) != nullptr) {
    auto s2 = dynamic_cast<lex::Symbol *>(tokens.peek());
    if (s2->meta == ">") {
      this->to = true;
      tokens.pop();
    }
  }

  this->Ident = ident;
  this->modList = modList;
  this->expr = parser.parseExpr(tokens);
  this->indices = indices;
}

gen::GenerationResult const Assign::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  asmc::File targetFile;
  auto checkLoans = this->modList.count > 0;
  auto resolved = generator.resolveSymbol(this->Ident, this->modList,
                                          targetFile, this->indices);

  if (!std::get<2>(resolved)) {
    generator.alert("undefined variable:" + this->Ident);
  }

  auto symbol = &std::get<1>(resolved);

  auto fin = symbol;
  auto *binding = std::get<4>(resolved);
  const std::string bindingIdent = binding == nullptr ? "" : binding->symbol;
  auto var = dynamic_cast<ast::Var *>(this->expr);

  // A local field is embedded object storage rather than a pointer-sized
  // field. Moving a class value into it must let the source lay out its full
  // state (and invalidate itself), not overwrite the first word of storage.
  if (symbol->local) {
    if (!symbol->mutable_ && !this->override)
      generator.alert("cannot assign to immutable " + symbol->symbol);
    if (this->reference || this->to)
      generator.alert("cannot assign a reference to local field `" +
                      this->Ident + "." + this->modList.peek() + "`");
    if (var == nullptr) {
      // Calls and other owned rvalues need a binding before they can become a
      // transfer receiver. The binding is released below just like an
      // explicitly named source variable.
      auto *temporary = new ast::DecAssign();
      temporary->logicalLine = this->logicalLine;
      temporary->declare = new ast::Declare();
      temporary->declare->logicalLine = this->logicalLine;
      temporary->declare->ident =
          "$" + std::to_string(generator.tempCount()++) + "_local_transfer";
      temporary->declare->type = ast::Type("let", asmc::AUTO);
      temporary->declare->mut = false;
      temporary->expr = this->expr;
      temporary->mute = false;
      file << generator.GenSTMT(temporary);

      // Constructor results are heap wrappers even for non-unique classes.
      // This internal binding owns the wrapper until the transfer releases it.
      if (auto *temporarySymbol = gen::scope::ScopeManager::getInstance()->get(
              temporary->declare->ident))
        temporarySymbol->owned = true;

      auto *temporaryVar = new ast::Var();
      temporaryVar->logicalLine = this->logicalLine;
      temporaryVar->Ident = temporary->declare->ident;

      ast::Assign transferAssignment;
      transferAssignment.logicalLine = this->logicalLine;
      transferAssignment.Ident = this->Ident;
      transferAssignment.modList = this->modList;
      transferAssignment.indices = this->indices;
      transferAssignment.expr = temporaryVar;
      transferAssignment.override = this->override;
      file << generator.GenSTMT(&transferAssignment);
      return {file, std::nullopt};
    }

    auto *fieldEntry = generator.typeList()[symbol->type.typeName];
    auto *fieldClass = fieldEntry == nullptr
                           ? nullptr
                           : dynamic_cast<gen::Class *>(*fieldEntry);
    if (fieldClass == nullptr)
      generator.alert("assignment to local field `" + this->Ident + "." +
                      this->modList.peek() +
                      "` requires a class with `__transfer_to__`");

    auto sourceResolved = generator.resolveSymbol(
        var->Ident, var->modList, file, links::LinkedList<ast::Expr *>());
    if (!std::get<2>(sourceResolved))
      generator.alert("undefined variable:" + var->Ident);
    auto *source = &std::get<1>(sourceResolved);
    if (source->type.typeName != symbol->type.typeName)
      generator.canAssign(symbol->type, source->type.typeName,
                          "symbol of type {} cannot be assigned to type {}");
    const bool releaseSource = var->modList.count == 0 && source->owned &&
                               !generator.suppressOwnershipEffects();
    if (fieldClass->uniqueType && var->modList.count == 0 && !source->owned)
      generator.alert("cannot transfer ownership of unowned value `" +
                      var->Ident + "`");

    auto *destination = new ast::Reference();
    destination->logicalLine = this->logicalLine;
    destination->Ident = this->Ident;
    destination->modList = this->modList;
    // The resolver already materializes an inline field as its address.
    destination->addressOf = false;

    if (fieldClass->uniqueType) {
      auto *transfer = new ast::Call();
      transfer->logicalLine = this->logicalLine;
      transfer->ident = var->Ident;
      transfer->modList = var->modList;
      transfer->modList.push("__transfer_to__");
      transfer->Args.push(destination);
      file << generator.GenSTMT(transfer);
    } else {
      auto *sourceAddress = new ast::Reference();
      sourceAddress->logicalLine = this->logicalLine;
      sourceAddress->Ident = var->Ident;
      sourceAddress->modList = var->modList;
      sourceAddress->addressOf = false;

      auto *size = new ast::IntLiteral();
      size->logicalLine = this->logicalLine;
      size->val = fieldClass->instanceSize;

      auto *copy = new ast::Call();
      copy->logicalLine = this->logicalLine;
      copy->ident = "af_memcpy";
      copy->Args.push(destination);
      copy->Args.push(sourceAddress);
      copy->Args.push(size);
      file << generator.GenSTMT(copy);
    }

    // A direct source owns a separate heap wrapper. The transfer hook has
    // moved its state, so release that wrapper without invoking `del`.
    if (releaseSource) {
      auto *released = new ast::Var();
      released->logicalLine = this->logicalLine;
      released->Ident = var->Ident;
      released->selling = true;
      auto *freeCall = new ast::Call();
      freeCall->logicalLine = this->logicalLine;
      freeCall->ident = "af_free";
      freeCall->Args.push(released);
      file << generator.GenSTMT(freeCall);

      // Field metadata is shared across instances, so only root bindings can
      // carry the per-value sold state.
      auto *liveSource =
          gen::scope::ScopeManager::getInstance()->get(var->Ident);
      if (liveSource != nullptr)
        liveSource->sold = this->logicalLine;
    }
    return {file, std::nullopt};
  }

  if (symbol->type.isReference && !this->override) {
    if (this->to) {
      if (!var) {
        generator.alert("A reference can only point to an lvalue");
      }
      // create a reference rather than a var
      ast::Reference *ref = new ast::Reference();
      ref->Ident = var->Ident;
      ref->modList = var->modList;
      ref->logicalLine = var->logicalLine;
      this->expr = ref;
      this->override = true; // we can set a reference even if it is const
      auto resolve =
          generator.resolveSymbol(var->Ident, var->modList, file, var->indices);
      if (!std::get<2>(resolve)) {
        generator.alert("undefined variable:" + var->Ident);
      } else {
        auto sym = &std::get<1>(resolve);
        if (!sym->mutable_ && symbol->mutable_) {
          generator.alert("A reference to the const variable " + var->Ident +
                          " cannot be assigned to a mutable variable" +
                          "it is important to keep mutability consistent");
        } else if (sym->mutable_ && symbol->mutable_) {
          gen::scope::ScopeManager::getInstance()->addAssign(sym->symbol);
        }
      }
    } else if (!(var && var->clean)) {
      this->reference = true;
    }
  }

  // check if the symbol is a class
  gen::Type **t = generator.typeList()[symbol->type.typeName];
  if (t != nullptr) {
    gen::Class *cl = dynamic_cast<gen::Class *>(*t);
    if (cl != nullptr) {
      if (cl->safeType && symbol->symbol != "my" && !this->override) {
        if (cl->nameTable["_set"] != nullptr) {
          ast::Call *callGet = new ast::Call();
          callGet->ident = this->Ident;
          callGet->modList = this->modList;
          callGet->modList << "_set";
          callGet->logicalLine = this->logicalLine;
          callGet->Args.push(this->expr);
          file << generator.GenSTMT(callGet);
          return {file, std::nullopt};
        }
      }
      if (this->modList.count == 0 && !this->override) {
        // check if the class has an overloaded operator =
        ast::Function *func = cl->overloadTable[ast::Equ];
        if (func != nullptr) {
          // Operator lookup is keyed only by the operator kind and may return
          // any member of the overload family. Start calls at the unsuffixed
          // function so ordinary overload retry can consider every variant.
          const auto overloadSuffix = func->ident.ident.rfind("_ovl");
          if (overloadSuffix != std::string::npos) {
            const auto baseIdent = func->ident.ident.substr(0, overloadSuffix);
            if (auto *base = cl->nameTable[baseIdent])
              func = base;
          }
          // call the overloaded operator =
          ast::Var *v = new ast::Var();
          v->Ident = this->Ident;
          v->modList = this->modList;
          ast::Call *call = new ast::Call();
          call->ident = func->ident.ident;
          call->modList = this->modList;
          call->Args.push(v);
          call->Args.push(this->expr);
          call->publify = cl->Ident;
          ast::CallExpr *callExpr = new ast::CallExpr();
          callExpr->call = call;
          this->expr = callExpr;
        };
      }
    }
  }

  asmc::Mov *mov = new asmc::Mov();
  mov->logicalLine = this->logicalLine;
  asmc::Mov *mov2 = new asmc::Mov();
  mov2->logicalLine = this->logicalLine;
  gen::Expr expr = generator.GenExpr(this->expr, file, symbol->type.size,
                                     symbol->type.typeName);

  auto *fieldType = generator.typeList()[symbol->type.typeName];
  const int fieldDepth =
      this->modList.count -
      (generator.nameSpaceTable().contains(this->Ident) ? 1 : 0);
  const bool assignsOwnershipBearingField =
      fieldDepth > 0 && !symbol->type.isReference && !symbol->type.isLoan &&
      fieldType != nullptr && dynamic_cast<gen::Class *>(*fieldType) != nullptr;
  const auto *sourceVar = dynamic_cast<ast::Var *>(this->expr);
  const bool assignsNull = sourceVar != nullptr && sourceVar->Ident == "NULL" &&
                           sourceVar->modList.count == 0;
  auto *sourceType = generator.typeList()[expr.type];
  const bool plainOwnershipBearingLvalue =
      sourceVar != nullptr && !assignsNull && sourceType != nullptr &&
      dynamic_cast<gen::Class *>(*sourceType) != nullptr;
  if (plainOwnershipBearingLvalue)
    expr.owned = false;

  if (!this->reference || symbol->type.isReference) {
    if (!generator.canAssign(
            symbol->type, expr.type,
            "symbol of type {} cannot be assigned to type {}")) {
      auto prev = expr;
      expr = generator.GenExpr(
          generator.imply(this->expr, symbol->type.typeName), file);
      expr.adoptImmutableRequirement(prev);
    }
  }

  if (assignsOwnershipBearingField && !expr.owned && !assignsNull) {
    generator.alert("cannot bind unowned value of type `" + expr.type +
                        "` to ownership-bearing field `" + this->Ident + "." +
                        this->modList.peek() + "`",
                    true, __FILE__, __LINE__);
  }

  if (symbol->declarationScope >= 0)
    generator.validateLoanAssignment(expr, *fin);

  if (expr.requiresImmutableBinding && !symbol->readOnly) {
    auto source = expr.immutableBindingSource.empty()
                      ? std::string("this expression")
                      : expr.immutableBindingSource;
    generator.alert("value produced by `" + source +
                        "` must be bound to an immutable symbol",
                    true, __FILE__, __LINE__);
  }

  mov->op = expr.op;
  mov2->op = expr.op;
  mov->size = expr.size;
  mov2->size = expr.size;
  mov2->from = expr.access;

  if (expr.op == asmc::Float)
    mov2->to = generator.registers()["%xmm0"]->get(expr.size);
  else
    mov2->to = generator.registers()["%rbx"]->get(expr.size);
  mov->from = mov2->to;

  this->modList.invert();

  asmc::Size size;
  std::string output = std::get<0>(resolved);
  asmc::Pop *pop = nullptr;
  file << targetFile;
  if (this->reference == true) {
    //
    asmc::Mov *m1 = new asmc::Mov;
    m1->logicalLine = this->logicalLine;
    m1->from = output;
    m1->size = asmc::QWord;
    m1->to = generator.registers()["%r9"]->get(asmc::QWord);
    mov->to = "(" + generator.registers()["%r9"]->get(asmc::QWord) + ")";

    asmc::Push *push = new asmc::Push();
    push->logicalLine = this->logicalLine;
    push->size = asmc::QWord;
    push->op = generator.registers()["%r9"]->get(asmc::QWord);
    file.text << push;

    pop = new asmc::Pop();
    pop->logicalLine = this->logicalLine;
    pop->size = asmc::QWord;
    pop->op = generator.registers()["%r11"]->get(asmc::QWord);

    file.text << push;
    file.text << m1;
  } else {
    mov->to = output;
  };

  if ((!fin->mutable_ || fin->readOnly) && !this->override &&
      !(this->reference && !fin->type.isReference)) {
    generator.alert("cannot assign to immutable " + fin->symbol);
  }

  mov2->logicalLine = this->logicalLine;
  mov->logicalLine = this->logicalLine;
  file.text << mov2;
  file.text << mov;
  if (pop != nullptr) {
    file.text << pop;
  }
  file << std::get<3>(resolved);
  if (this->modList.count == 0)
    gen::scope::ScopeManager::getInstance()->addAssign(fin->symbol);

  if (this->modList.count == 0 && this->reference) {
    gen::scope::ScopeManager::getInstance()->get(fin->symbol);
  }
  const bool targetOwnsValue = expr.owned && !symbol->type.isLoan;
  fin->owned = targetOwnsValue;
  fin->sold = -1;
  // Field symbols are shared class-layout metadata, not per-instance runtime
  // bindings. Do not leak assignment ownership state into other methods or
  // instances of the class.
  if (!bindingIdent.empty() && fieldDepth == 0) {
    // Generating the right-hand side may append hidden temporaries to the
    // scope's symbol vector and invalidate the pointer returned by the first
    // resolveSymbol call. Re-resolve the root binding before mutating it.
    auto *liveBinding =
        gen::scope::ScopeManager::getInstance()->get(bindingIdent);
    if (liveBinding == nullptr) {
      liveBinding = generator.GlobalSymbolTable().search<std::string>(
          gen::utils::searchSymbol, bindingIdent);
    }
    if (liveBinding != nullptr) {
      liveBinding->owned = targetOwnsValue;
      liveBinding->sold = -1;
    }
  }

  if (generator.TypeList()[fin->type.typeName] == nullptr) {
    auto t = new ast::Type();
    t->typeName = fin->type.typeName;
    t->size = fin->type.size;
    t->fPointerArgs = fin->type.fPointerArgs;
    generator.TypeList().push(*t);
  }

  return {file, std::nullopt};
}
} // namespace ast
