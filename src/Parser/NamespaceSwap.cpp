#include "LinkedList.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"

namespace ast {

template <typename T>
static void applyList(links::LinkedList<T *> &list,
                      const std::unordered_map<std::string, std::string> &map) {
  list.reset();
  for (int i = 0; i < list.count; ++i) {
    T *val = list.shift();
    if (val) val->namespaceSwap(map);
  }
  list.reset();
}

void Statement::namespaceSwap(
    std::unordered_map<std::string, std::string> map) {
  if (auto expr = dynamic_cast<Expr *>(this)) {
    if (expr->extention) expr->extention->namespaceSwap(map);
  }
  if (auto arg = dynamic_cast<Argument *>(this)) {
    return;  // no call ident here
  }
  if (auto par = dynamic_cast<ParenExpr *>(this)) {
    if (par->expr) par->expr->namespaceSwap(map);
    return;
  }
  if (auto par2 = dynamic_cast<parenExpr *>(this)) {
    if (par2->expr) par2->expr->namespaceSwap(map);
    return;
  }
  if (auto cwrite = dynamic_cast<CWrite *>(this)) {
    if (cwrite->expr) cwrite->expr->namespaceSwap(map);
    return;
  }
  if (auto push = dynamic_cast<Push *>(this)) {
    if (push->expr) push->expr->namespaceSwap(map);
    return;
  }
  if (auto pull = dynamic_cast<Pull *>(this)) {
    if (pull->expr) pull->expr->namespaceSwap(map);
    return;
  }
  if (auto note = dynamic_cast<Note *>(this)) {
    return;
  }
  if (auto buy = dynamic_cast<Buy *>(this)) {
    if (buy->expr) buy->expr->namespaceSwap(map);
    return;
  }
  if (auto var = dynamic_cast<Var *>(this)) {
    applyList(var->indices, map);
    return;
  }
  if (auto strList = dynamic_cast<StructList *>(this)) {
    applyList(strList->args, map);
    return;
  }
  if (auto notexpr = dynamic_cast<Not *>(this)) {
    if (notexpr->expr) notexpr->expr->namespaceSwap(map);
    return;
  }
  if (auto ifexpr = dynamic_cast<IfExpr *>(this)) {
    if (ifexpr->expr) ifexpr->expr->namespaceSwap(map);
    if (ifexpr->trueExpr) ifexpr->trueExpr->namespaceSwap(map);
    if (ifexpr->falseExpr) ifexpr->falseExpr->namespaceSwap(map);
    return;
  }
  if (auto comp = dynamic_cast<Compound *>(this)) {
    if (comp->expr1) comp->expr1->namespaceSwap(map);
    if (comp->expr2) comp->expr2->namespaceSwap(map);
    return;
  }
  if (auto ref = dynamic_cast<Reference *>(this)) {
    return;
  }
  if (auto deref = dynamic_cast<DeReference *>(this)) {
    return;
  }
  if (auto lambda = dynamic_cast<Lambda *>(this)) {
    if (lambda->function) lambda->function->namespaceSwap(map);
    return;
  }
  if (auto newExpr = dynamic_cast<NewExpr *>(this)) {
    applyList(newExpr->args, map);
    return;
  }
  if (auto callExpr = dynamic_cast<CallExpr *>(this)) {
    if (callExpr->call) callExpr->call->namespaceSwap(map);
    return;
  }
  if (auto assign = dynamic_cast<Assign *>(this)) {
    if (assign->expr) assign->expr->namespaceSwap(map);
    applyList(assign->indices, map);
    return;
  }
  if (auto declare = dynamic_cast<Declare *>(this)) {
    return;
  }
  if (auto decArr = dynamic_cast<DecArr *>(this)) {
    applyList(decArr->indices, map);
    return;
  }
  if (auto decAssign = dynamic_cast<DecAssign *>(this)) {
    if (decAssign->declare) decAssign->declare->namespaceSwap(map);
    if (decAssign->expr) decAssign->expr->namespaceSwap(map);
    return;
  }
  if (auto decAssignArr = dynamic_cast<DecAssignArr *>(this)) {
    if (decAssignArr->declare) decAssignArr->declare->namespaceSwap(map);
    if (decAssignArr->expr) decAssignArr->expr->namespaceSwap(map);
    return;
  }
  if (auto call = dynamic_cast<Call *>(this)) {
    auto it = map.find(call->ident);
    if (it != map.end()) call->imbeddedNamespace = it->second;
    applyList(call->Args, map);
    return;
  }
  if (auto destruct = dynamic_cast<Destructure *>(this)) {
    if (destruct->expr) destruct->expr->namespaceSwap(map);
    return;
  }
  if (auto forst = dynamic_cast<For *>(this)) {
    if (forst->declare) forst->declare->namespaceSwap(map);
    if (forst->expr) forst->expr->namespaceSwap(map);
    if (forst->increment) forst->increment->namespaceSwap(map);
    if (forst->Run) forst->Run->namespaceSwap(map);
    return;
  }
  if (auto foreach = dynamic_cast<ForEach *>(this)) {
    if (foreach->implementation)
      foreach
        ->implementation->namespaceSwap(map);
    if (foreach->iterator)
      foreach
        ->iterator->namespaceSwap(map);
    return;
  }
  if (auto func = dynamic_cast<Function *>(this)) {
    if (func->args) func->args->namespaceSwap(map);
    if (func->statement) func->statement->namespaceSwap(map);
    applyList(func->decoratorArgs, map);
    return;
  }
  if (auto ifst = dynamic_cast<If *>(this)) {
    if (ifst->expr) ifst->expr->namespaceSwap(map);
    if (ifst->statement) ifst->statement->namespaceSwap(map);
    if (ifst->elseStatement) ifst->elseStatement->namespaceSwap(map);
    if (ifst->elseIf) ifst->elseIf->namespaceSwap(map);
    return;
  }
  if (auto ret = dynamic_cast<Return *>(this)) {
    if (ret->expr) ret->expr->namespaceSwap(map);
    return;
  }
  if (auto seq = dynamic_cast<Sequence *>(this)) {
    if (seq->Statement1) seq->Statement1->namespaceSwap(map);
    if (seq->Statement2) seq->Statement2->namespaceSwap(map);
    return;
  }
  if (auto strct = dynamic_cast<Struct *>(this)) {
    if (strct->statement) strct->statement->namespaceSwap(map);
    return;
  }
  if (auto whileSt = dynamic_cast<While *>(this)) {
    if (whileSt->expr) whileSt->expr->namespaceSwap(map);
    if (whileSt->stmt) whileSt->stmt->namespaceSwap(map);
    return;
  }
  if (auto uni = dynamic_cast<Union *>(this)) {
    if (uni->statement) uni->statement->namespaceSwap(map);
    for (auto alias : uni->aliases) {
      if (alias->value && std::holds_alternative<ast::Expr *>(*alias->value)) {
        auto expr = std::get<ast::Expr *>(*alias->value);
        if (expr) expr->namespaceSwap(map);
      }
    }
    return;
  }
  if (auto match = dynamic_cast<Match *>(this)) {
    if (match->expr) match->expr->namespaceSwap(map);
    for (auto &caseExpr : match->cases) {
      if (caseExpr.statement) caseExpr.statement->namespaceSwap(map);
    }
    return;
  }
  if (auto unionConstructor = dynamic_cast<UnionConstructor *>(this)) {
    if (unionConstructor->expr) unionConstructor->expr->namespaceSwap(map);
    return;
  }
  if (auto cls = dynamic_cast<Class *>(this)) {
    if (cls->contract) cls->contract->namespaceSwap(map);
    if (cls->statement) cls->statement->namespaceSwap(map);
    return;
  }
  if (auto lambdaExpr = dynamic_cast<Lambda *>(this)) {
    if (lambdaExpr->function) lambdaExpr->function->namespaceSwap(map);
    return;
  }
}

}  // namespace ast
