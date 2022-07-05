#include "Parser/Lower.hpp"

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
        this->lowerFunction(func);
    }
    
    return stmt;
}

ast::Statment * Lower::lowerFunction(ast::Function * func){
    return func;
}