#include "Parser/Lower.hpp"
#include "Parser/AST/Statements.hpp"
#include "memory.h"
#include <iostream>

typedef parse::lower::Lowerer Lower;

links::LinkedList<ast::Expr *> convertsArgs(ast::Statement* STMT) {
  links::LinkedList<ast::Expr *> args;
  if (dynamic_cast<ast::Sequence*>(STMT) != nullptr) {
    ast::Sequence* sequence = dynamic_cast<ast::Sequence*>(STMT);
    args.istitch(convertsArgs(sequence->Statement1));
    args.istitch(convertsArgs(sequence->Statement2));
  } else if (dynamic_cast<ast::Declare*>(STMT) != nullptr) {
    auto declare = dynamic_cast<ast::Declare*>(STMT);
    auto var = new ast::Var;
    var->Ident = declare->ident;
    var->logicalLine = declare->logicalLine;

    args.push(var);
  }
  return args;
}

Lower::Lowerer(ast::Statement *root) {
  this->root = root;
  this->curr = root;
  this->root = this->lower(root);
}

ast::Statement *Lower::lower(ast::Statement *stmt) {
  if (dynamic_cast<ast::Sequence *>(stmt) != nullptr) {
    auto seq = dynamic_cast<ast::Sequence *>(stmt);
    this->curr = seq;
    seq->Statement1 = this->lower(seq->Statement1);
    seq->Statement2 = this->lower(seq->Statement2);
  } else if (dynamic_cast<ast::Function *>(stmt) != nullptr) {
    auto func = dynamic_cast<ast::Function *>(stmt);
    stmt = this->lowerFunction(func);
  } else if (dynamic_cast<ast::Class *>(stmt) != nullptr) {
    auto cl = dynamic_cast<ast::Class *>(stmt);
    this->inclass = true;
    this->className = cl->ident.ident;
    cl->statement = this->lower(cl->statement);
    this->inclass = false;
    stmt = cl;
  };

  return stmt;
}

ast::Statement *Lower::lowerFunction(ast::Function *func) {
  ast::Statement * res = func;
  if (func->decorator != "") {
    auto newFunc = new ast::Function;

    newFunc->args = func->args;
    newFunc->argTypes = func->argTypes;
    newFunc->ident = func->ident;
    newFunc->scope = func->scope;
    newFunc->scopeName = func->scopeName;
    newFunc->type = func->type;

    func->ident.ident = ".temp__graft_" + func->ident.ident;

    auto fpoint = new ast::Var;
    fpoint->Ident = func->ident.ident;

    auto args = convertsArgs(func->args);

    auto call = new ast::CallExpr;
    call->call = new ast::Call;
    call->call->Args = links::LinkedList<ast::Expr *>();
    call->call->Args.push(fpoint);
    call->call->Args.istitch(func->decoratorArgs);
    call->call->Args.istitch(args);

    if (func->decNSP == "") {
      bool fromClass = false;

      call->call->ident = func->decorator;

      ast::Function *dec =
          this->findFunction(this->root, func->decorator, fromClass);
      if (dec == nullptr) {
        if (!this->inclass)
          throw err::Exception("Can't use a class as a decorator outside a class");

        ast::Declare *declare = new ast::Declare;
        declare->ident = newFunc->ident.ident;
        declare->type.size = asmc::QWord;
        declare->type.typeName = func->decorator;
        declare->mut = false;
        declare->TypeName = func->decorator;
        declare->scope = func->scope;

        ast::DecAssign *decAssign = new ast::DecAssign;
        decAssign->declare = declare;
        decAssign->expr = call;
        delete newFunc;
        delete call;

        ast::NewExpr *newExpr = new ast::NewExpr;
        ast::Var *var = new ast::Var;
        var->Ident = func->ident.ident;
        ast::Var *my = new ast::Var;
        my->Ident = "my";
        newExpr->args.push(var);
        newExpr->args.push(my);
        func->decoratorArgs.reset();
        newExpr->args.istitch(func->decoratorArgs);
        newExpr->type = declare->type;
        decAssign->expr = newExpr;

        auto seq = new ast::Sequence;
        seq->Statement1 = func;
        seq->Statement2 = decAssign;

        res = seq;
        return res;
      };

      if (dec->scopeName != "global" || fromClass) {
        call->call->modList.push(call->call->ident);
        call->call->ident = "my";
      };
    } else {
      call->call->ident = func->decNSP;
      call->call->modList.push(func->decorator);
    }

    auto ret = new ast::Return;
    ret->expr = call;
    newFunc->statement = ret;

    auto seq = new ast::Sequence;
    seq->Statement1 = func;
    seq->Statement2 = newFunc;
    res = seq;
  }
  return res;
}

ast::Function *Lower::findFunction(ast::Statement *stmt, std::string ident,
                                   bool &fromClass) {
  
  if (stmt) if (dynamic_cast<ast::Sequence *>(stmt) != nullptr) {
   auto seq = dynamic_cast<ast::Sequence *>(stmt);
    this->curr = seq;
    if (this->findFunction(seq->Statement1, ident, fromClass) != nullptr)
      return this->findFunction(seq->Statement1, ident, fromClass);
    if (this->findFunction(seq->Statement2, ident, fromClass) != nullptr)
      return this->findFunction(seq->Statement2, ident, fromClass);
    // seq->Statment2 = this->lower(seq->Statment2);
  } else if (dynamic_cast<ast::Function *>(stmt) != nullptr) {
    auto func = dynamic_cast<ast::Function *>(stmt);
    if (func->ident.ident == ident)
      return func;
  } else if (dynamic_cast<ast::Class *>(stmt) != nullptr) {
    auto cl = dynamic_cast<ast::Class *>(stmt);
    if (this->findFunction(cl->statement, ident, fromClass) != nullptr) {
      fromClass = true;
      bool trash;
      return this->findFunction(cl->statement, ident, trash);
    }
    // cl->statment = this->lower(cl->statment);
  };
  return nullptr;
}

ast::Class *Lower::findClass(ast::Statement *stmt, std::string ident) {
  if (dynamic_cast<ast::Sequence *>(stmt) != nullptr) {
       auto seq = dynamic_cast<ast::Sequence *>(stmt);
    this->curr = seq;
    if (this->findClass(seq->Statement1, ident) != nullptr)
      return this->findClass(seq->Statement1, ident);
    if (this->findClass(seq->Statement2, ident) != nullptr)
      return this->findClass(seq->Statement2, ident);
    //seq->Statment2 = this->lower(seq->Statment2);
  } else if (dynamic_cast<ast::Class *>(stmt) != nullptr) {
    auto cl = dynamic_cast<ast::Class *>(stmt);
    if (cl->ident.ident == ident)
      return cl;
  }
  return nullptr;
}