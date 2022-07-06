#include "Parser/Lower.hpp"
#include "memory.h"
#include <iostream>

typedef parse::lower::Lowerer Lower;

Lower::Lowerer(ast::Statment * root){
    this->root = root;
    this->curr = root;
    this->root = this->lower(root);
}

ast::Statment * Lower::lower(ast::Statment *stmt){
    if (dynamic_cast<ast::Sequence *>(stmt) != nullptr){
        ast::Sequence * seq = dynamic_cast<ast::Sequence *>(stmt);
        this->curr = seq;
        seq->Statment1 = this->lower(seq->Statment1);
        seq->Statment2 = this->lower(seq->Statment2);
    } else if (dynamic_cast<ast::Function *>(stmt) != nullptr){
        ast::Function * func = dynamic_cast<ast::Function *>(stmt);
        stmt = this->lowerFunction(func);
    };
    
    return stmt;
}

ast::Statment * Lower::lowerFunction(ast::Function * func){
    ast::Statment * res = func;
    if (func->decorator != ""){
        ast::Function * newFunc = new ast::Function;

        newFunc->args = func->args;
        newFunc->argTypes = func->argTypes;
        newFunc->ident = func->ident;
        newFunc->scope = func->scope;
        newFunc->scopeName = func->scopeName;
        newFunc->type = func->type;
        ast::Var * fpoint = new ast::Var;
        fpoint->Ident = func->ident.ident;
        ast::CallExpr * call = new ast::CallExpr;
        call->call = new ast::Call;
        call->call->Args = links::LinkedList<ast::Expr *>();
        call->call->Args.push(fpoint);
        call->call->ident = func->decorator;
        
        ast::Return * ret = new ast::Return;
        ret->expr = call;
        newFunc->statment = ret;

        ast::Sequence * seq = new ast::Sequence;
        seq->Statment1 = func;
        seq->Statment2 = newFunc;
        res = seq;
    }
    return res;
}