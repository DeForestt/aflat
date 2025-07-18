#include <iostream>
#include <string>

#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"

namespace ast {

static links::LinkedList<std::string> copyStringList(
    const links::LinkedList<std::string> &list) {
  links::LinkedList<std::string> out;
  auto node = list.head;
  while (node) {
    out.append(node->data);
    node = node->next;
  }
  return out;
}

static links::LinkedList<Expr *> copyExprList(
    const links::LinkedList<Expr *> &list) {
  links::LinkedList<Expr *> out;
  auto node = list.head;
  while (node) {
    out.append(static_cast<Expr *>(deepCopy(node->data)));
    node = node->next;
  }
  return out;
}

Statement *deepCopy(const Statement *stmt) {
  if (!stmt) return nullptr;
  if (auto seq = dynamic_cast<const Sequence *>(stmt)) {
    auto *copy = new Sequence();
    copy->locked = seq->locked;
    copy->logicalLine = seq->logicalLine;
    copy->Statement1 = deepCopy(seq->Statement1);
    copy->Statement2 = deepCopy(seq->Statement2);
    return copy;
  }
  if (auto func = dynamic_cast<const Function *>(stmt)) {
    auto *copy = new Function(*func, func->locked);
    copy->args = deepCopy(func->args);
    copy->statement = deepCopy(func->statement);
    copy->decoratorArgs = copyExprList(func->decoratorArgs);
    return copy;
  }
  if (auto decl = dynamic_cast<const Declare *>(stmt)) {
    auto *copy = new Declare(*decl);
    copy->modList = copyStringList(decl->modList);
    return copy;
  }
  if (auto assign = dynamic_cast<const Assign *>(stmt)) {
    auto *copy = new Assign();
    copy->locked = assign->locked;
    copy->logicalLine = assign->logicalLine;
    copy->Ident = assign->Ident;
    copy->reference = assign->reference;
    copy->override = assign->override;
    copy->to = assign->to;
    copy->expr = static_cast<Expr *>(deepCopy(assign->expr));
    copy->modList = copyStringList(assign->modList);
    copy->indices = copyExprList(assign->indices);
    return copy;
  }
  if (auto ret = dynamic_cast<const Return *>(stmt)) {
    auto *copy = new Return();
    copy->locked = ret->locked;
    copy->logicalLine = ret->logicalLine;
    copy->empty = ret->empty;
    copy->expr = static_cast<Expr *>(deepCopy(ret->expr));
    return copy;
  }
  if (auto wh = dynamic_cast<const While *>(stmt)) {
    auto *copy = new While();
    copy->locked = wh->locked;
    copy->logicalLine = wh->logicalLine;
    copy->expr = static_cast<Expr *>(deepCopy(wh->expr));
    copy->stmt = deepCopy(wh->stmt);
    return copy;
  }
  if (auto fr = dynamic_cast<const For *>(stmt)) {
    auto *copy = new For();
    copy->locked = fr->locked;
    copy->logicalLine = fr->logicalLine;
    copy->declare = deepCopy(fr->declare);
    copy->expr = static_cast<Expr *>(deepCopy(fr->expr));
    copy->increment = deepCopy(fr->increment);
    copy->Run = deepCopy(fr->Run);
    return copy;
  }
  if (auto iff = dynamic_cast<const If *>(stmt)) {
    auto *copy = new If();
    copy->locked = iff->locked;
    copy->logicalLine = iff->logicalLine;
    copy->expr = static_cast<Expr *>(deepCopy(iff->expr));
    copy->statement = deepCopy(iff->statement);
    copy->elseStatement = deepCopy(iff->elseStatement);
    copy->elseIf = deepCopy(iff->elseIf);
    return copy;
  }
  if (auto deca = dynamic_cast<const DecAssign *>(stmt)) {
    auto *copy = new DecAssign(*deca);
    copy->declare = static_cast<Declare *>(deepCopy(deca->declare));
    copy->expr = static_cast<Expr *>(deepCopy(deca->expr));
    return copy;
  }
  if (auto decaa = dynamic_cast<const DecAssignArr *>(stmt)) {
    auto *copy = new DecAssignArr(*decaa);
    copy->declare = static_cast<DecArr *>(deepCopy(decaa->declare));
    copy->expr = static_cast<Expr *>(deepCopy(decaa->expr));
    return copy;
  }
  if (auto decarr = dynamic_cast<const DecArr *>(stmt)) {
    auto *copy = new DecArr(*decarr);
    copy->indices = copyExprList(decarr->indices);
    return copy;
  }
  if (auto un = dynamic_cast<const Union *>(stmt)) {
    auto *copy = new Union(*un);
    copy->statement = deepCopy(un->statement);
    copy->genericTypes = std::vector<std::string>(un->genericTypes);
    for (auto &alias : copy->aliases) {
      if (alias.isType()) {
        alias.value =
            new Type(alias.getType()->typeName, alias.getType()->size);
      } else if (alias.isConstExpr()) {
        alias.value = static_cast<Expr *>(deepCopy(alias.getConstExpr()));
      }
    }
    return copy;
  }
  if (auto cls = dynamic_cast<const Class *>(stmt)) {
    auto *copy = new Class(*cls);
    copy->contract = deepCopy(cls->contract);
    copy->statement = deepCopy(cls->statement);
    return copy;
  }
  if (auto strct = dynamic_cast<const Struct *>(stmt)) {
    auto *copy = new Struct(*strct);
    copy->statement = deepCopy(strct->statement);
    return copy;
  }
  if (auto imp = dynamic_cast<const Import *>(stmt)) {
    return new Import(*imp);
  }
  if (auto call = dynamic_cast<const Call *>(stmt)) {
    auto *copy = new Call();
    copy->locked = call->locked;
    copy->logicalLine = call->logicalLine;
    copy->ident = call->ident;
    copy->Args = copyExprList(call->Args);
    copy->modList = copyStringList(call->modList);
    copy->publify = call->publify;
    return copy;
  }
  if (auto var = dynamic_cast<const Var *>(stmt)) {
    auto *copy = new Var();
    copy->locked = var->locked;
    copy->logicalLine = var->logicalLine;
    copy->Ident = var->Ident;
    copy->indices = copyExprList(var->indices);
    copy->modList = copyStringList(var->modList);
    copy->clean = var->clean;
    copy->internal = var->internal;
    copy->selling = var->selling;
    copy->typeCast = var->typeCast;
    return copy;
  }
  if (auto strlit = dynamic_cast<const StringLiteral *>(stmt)) {
    return new StringLiteral(*strlit);
  }
  if (auto flit = dynamic_cast<const FloatLiteral *>(stmt)) {
    return new FloatLiteral(*flit);
  }
  if (auto ilit = dynamic_cast<const IntLiteral *>(stmt)) {
    return new IntLiteral(*ilit);
  }
  if (auto llit = dynamic_cast<const LongLiteral *>(stmt)) {
    return new LongLiteral(*llit);
  }
  if (auto charlit = dynamic_cast<const CharLiteral *>(stmt)) {
    return new CharLiteral(*charlit);
  }
  if (auto newe = dynamic_cast<const NewExpr *>(stmt)) {
    auto *copy = new NewExpr(*newe);
    copy->args = copyExprList(newe->args);
    return copy;
  }
  if (auto structList = dynamic_cast<const StructList *>(stmt)) {
    auto *copy = new StructList();
    copy->args = copyExprList(structList->args);
    return copy;
  }
  if (auto comp = dynamic_cast<const Compound *>(stmt)) {
    auto *copy = new Compound();
    copy->expr1 = static_cast<Expr *>(deepCopy(comp->expr1));
    copy->expr2 = static_cast<Expr *>(deepCopy(comp->expr2));
    copy->op = comp->op;
    return copy;
  }
  if (auto ref = dynamic_cast<const Reference *>(stmt)) {
    auto *copy = new Reference(*ref);
    copy->modList = copyStringList(ref->modList);
    return copy;
  }
  if (auto deref = dynamic_cast<const DeReference *>(stmt)) {
    auto *copy = new DeReference(*deref);
    copy->modList = copyStringList(deref->modList);
    return copy;
  }
  if (auto par = dynamic_cast<const ParenExpr *>(stmt)) {
    auto *copy = new ParenExpr();
    copy->expr = static_cast<Expr *>(deepCopy(par->expr));
    return copy;
  }
  if (auto par2 = dynamic_cast<const parenExpr *>(stmt)) {
    auto *copy = new parenExpr();
    copy->expr = static_cast<Expr *>(deepCopy(par2->expr));
    return copy;
  }
  if (auto callExpr = dynamic_cast<const CallExpr *>(stmt)) {
    auto *copy = new CallExpr();
    copy->templateTypes = std::vector<std::string>(callExpr->templateTypes);
    copy->call = static_cast<Call *>(deepCopy(callExpr->call));
    return copy;
  }
  if (auto lambda = dynamic_cast<const Lambda *>(stmt)) {
    auto *copy = new Lambda();
    copy->function = static_cast<Function *>(deepCopy(lambda->function));
    return copy;
  }
  if (auto buy = dynamic_cast<const Buy *>(stmt)) {
    auto *copy = new Buy();
    copy->expr = static_cast<Expr *>(deepCopy(buy->expr));
    return copy;
  }
  if (auto notExpr = dynamic_cast<const Not *>(stmt)) {
    auto *copy = new Not();
    copy->expr = static_cast<Expr *>(deepCopy(notExpr->expr));
    return copy;
  }
  if (auto ifExpr = dynamic_cast<const IfExpr *>(stmt)) {
    auto *copy = new IfExpr();
    copy->expr = static_cast<Expr *>(deepCopy(ifExpr->expr));
    copy->trueExpr = static_cast<Expr *>(deepCopy(ifExpr->trueExpr));
    copy->falseExpr = static_cast<Expr *>(deepCopy(ifExpr->falseExpr));
    return copy;
  }
  if (auto dec = dynamic_cast<const Dec *>(stmt)) {
    return new Dec(*dec);
  }
  if (auto inc = dynamic_cast<const Inc *>(stmt)) {
    return new Inc(*inc);
  }
  if (auto des = dynamic_cast<const Destructure *>(stmt)) {
    auto *copy = new Destructure(*des);
    copy->expr = static_cast<Expr *>(deepCopy(des->expr));
    return copy;
  }
  if (auto del = dynamic_cast<const Delete *>(stmt)) {
    auto *copy = new Delete(*del);
    copy->modList = copyStringList(del->modList);
    return copy;
  }
  if (auto en = dynamic_cast<const Enum *>(stmt)) {
    return new Enum(*en);
  }
  if (auto trans = dynamic_cast<const Transform *>(stmt)) {
    return new Transform(trans->ident, trans->_template);
  }
  if (auto push = dynamic_cast<const Push *>(stmt)) {
    auto *copy = new Push();
    copy->expr = static_cast<Expr *>(deepCopy(push->expr));
    return copy;
  }
  if (auto pull = dynamic_cast<const Pull *>(stmt)) {
    auto *copy = new Pull();
    copy->expr = static_cast<Expr *>(deepCopy(pull->expr));
    return copy;
  }
  if (auto cw = dynamic_cast<const CWrite *>(stmt)) {
    auto *copy = new CWrite();
    copy->expr = static_cast<Expr *>(deepCopy(cw->expr));
    return copy;
  }
  if (auto note = dynamic_cast<const Note *>(stmt)) {
    auto *copy = new Note(note->message);
    return copy;
  }
  if (auto brk = dynamic_cast<const Break *>(stmt)) {
    return new Break(*brk);
  }
  if (auto cont = dynamic_cast<const Continue *>(stmt)) {
    return new Continue(*cont);
  }
  if (auto format = dynamic_cast<const FStringLiteral *>(stmt)) {
    auto *copy = new FStringLiteral();
    copy->original = format->original;
    copy->val = format->val;
    for (auto &arg : format->args) {
      copy->args.push_back(static_cast<Expr *>(deepCopy(arg)));
    }
    return copy;
  }
  if (auto unionConstructor = dynamic_cast<const UnionConstructor *>(stmt)) {
    auto *copy = new UnionConstructor();
    copy->unionType = unionConstructor->unionType;
    copy->variantName = unionConstructor->variantName;
    copy->expr = static_cast<Expr *>(deepCopy(unionConstructor->expr));
    copy->dynamic = unionConstructor->dynamic;
    copy->templateTypes = unionConstructor->templateTypes;
    return copy;
  }
  if (auto match = dynamic_cast<const Match *>(stmt)) {
    auto *copy = new Match();
    copy->expr = static_cast<Expr *>(deepCopy(match->expr));
    copy->cases = std::vector<Match::Case>();
    copy->returns = match->returns;
    for (const auto &caseItem : match->cases) {
      auto *caseCopy = new Match::Case();
      caseCopy->pattern = Match::Pattern(caseItem.pattern);
      caseCopy->statement = deepCopy(caseItem.statement);
      copy->cases.push_back(*caseCopy);
    }
    return copy;
  }
  return nullptr;
}

}  // namespace ast
