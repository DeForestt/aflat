#include "Parser/Lower.hpp"
#include "memory.h"
#include <iostream>

typedef parse::lower::Lowerer Lower;

Lower::Lowerer(ast::Statment *root) {
  this->root = root;
  this->curr = root;
  this->root = this->lower(root);
}

ast::Statment *Lower::lower(ast::Statment *stmt) {
  if (dynamic_cast<ast::Sequence *>(stmt) != nullptr) {
    ast::Sequence *seq = dynamic_cast<ast::Sequence *>(stmt);
    this->curr = seq;
    seq->Statment1 = this->lower(seq->Statment1);
    seq->Statment2 = this->lower(seq->Statment2);
  } else if (dynamic_cast<ast::Function *>(stmt) != nullptr) {
    ast::Function *func = dynamic_cast<ast::Function *>(stmt);
    stmt = this->lowerFunction(func);
  } else if (dynamic_cast<ast::Class *>(stmt) != nullptr) {
    ast::Class *cl = dynamic_cast<ast::Class *>(stmt);
    this->inclass = true;
    this->className = cl->ident.ident;
    cl->statment = this->lower(cl->statment);
    this->inclass = false;
    stmt = cl;
  };

  return stmt;
}

ast::Statment *Lower::lowerFunction(ast::Function *func) {
  ast::Statment *res = func;
  if (func->decorator != "") {
    ast::Function *newFunc = new ast::Function;

    newFunc->args = func->args;
    newFunc->argTypes = func->argTypes;
    newFunc->ident = func->ident;
    newFunc->scope = func->scope;
    newFunc->scopeName = func->scopeName;
    newFunc->type = func->type;

    func->ident.ident = "_" + func->ident.ident;

    bool fromClass = false;

    ast::Var *fpoint = new ast::Var;
    fpoint->Ident = func->ident.ident;
    ast::Var *Arg = new ast::Var;
    Arg->Ident = "_arg";

    ast::CallExpr *call = new ast::CallExpr;
    call->call = new ast::Call;
    call->call->Args = links::LinkedList<ast::Expr *>();
    call->call->Args.push(fpoint);
    call->call->Args.push(Arg);

    if (func->decNSP == "") {

      call->call->ident = func->decorator;

      ast::Function *dec =
          this->findFunction(this->root, func->decorator, fromClass);
      if (dec == nullptr)
        throw err::Exception("No Function " + func->decorator + " found");

      if (dec->scopeName != "global" || fromClass) {
        call->call->modList.push(call->call->ident);
        call->call->ident = "my";
      };
    } else {
      call->call->ident = func->decNSP;
      call->call->modList.push(func->decorator);
    }

    ast::Return *ret = new ast::Return;
    ret->expr = call;
    newFunc->statment = ret;

    ast::Sequence *seq = new ast::Sequence;
    seq->Statment1 = func;
    seq->Statment2 = newFunc;
    res = seq;
  }
  return res;
}

ast::Function *Lower::findFunction(ast::Statment *stmt, std::string ident,
                                   bool &fromClass) {
  if (dynamic_cast<ast::Sequence *>(stmt) != nullptr) {
    ast::Sequence *seq = dynamic_cast<ast::Sequence *>(stmt);
    this->curr = seq;
    if (this->findFunction(seq->Statment1, ident, fromClass) != nullptr)
      return this->findFunction(seq->Statment1, ident, fromClass);
    if (this->findFunction(seq->Statment2, ident, fromClass) != nullptr)
      return this->findFunction(seq->Statment2, ident, fromClass);
    seq->Statment2 = this->lower(seq->Statment2);
  } else if (dynamic_cast<ast::Function *>(stmt) != nullptr) {
    ast::Function *func = dynamic_cast<ast::Function *>(stmt);
    if (func->ident.ident == ident)
      return func;
  } else if (dynamic_cast<ast::Class *>(stmt) != nullptr) {
    ast::Class *cl = dynamic_cast<ast::Class *>(stmt);
    if (this->findFunction(cl->statment, ident, fromClass) != nullptr) {
      fromClass = true;
      bool trash;
      return this->findFunction(cl->statment, ident, trash);
    }
    cl->statment = this->lower(cl->statment);
  };
  return nullptr;
}