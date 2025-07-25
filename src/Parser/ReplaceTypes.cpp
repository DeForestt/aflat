#include <iostream>
#include <sstream>

#include "ASM.hpp"
#include "CodeGenerator/Utils.hpp"
#include "LinkedList.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"

namespace ast {

static void applyType(Type &t,
                      const std::unordered_map<std::string, std::string> &map) {
  auto it = map.find(t.typeName);
  if (it != map.end()) t.typeName = it->second;
  if (t.fPointerArgs.returnType) applyType(*t.fPointerArgs.returnType, map);
  for (auto &a : t.fPointerArgs.argTypes) applyType(a, map);
}

template <typename T>
static void applyList(links::LinkedList<T *> &list,
                      const std::unordered_map<std::string, std::string> &map) {
  list.reset();
  for (int i = 0; i < list.count; ++i) {
    T *val = list.shift();
    if (val) val->replaceTypes(map);
  }
  list.reset();
}

static void applyTemplateTypes(
    std::vector<std::string> &templateTypes,
    const std::unordered_map<std::string, std::string> &map) {
  for (auto &t : templateTypes) {
    auto it = map.find(t);
    if (it != map.end()) t = it->second;
  }
}

static std::string replaceAllParts(
    const std::string &input,
    const std::unordered_map<std::string, std::string> &typeMap) {
  std::stringstream ss(input);
  std::string segment;
  std::vector<std::string> parts;

  // Split the string on '.'
  while (std::getline(ss, segment, '.')) {
    // Replace if mapping exists
    auto it = typeMap.find(segment);
    if (it != typeMap.end()) {
      parts.push_back(it->second);
    } else {
      parts.push_back(segment);
    }
  }

  // Join back together
  std::string result;
  for (size_t i = 0; i < parts.size(); ++i) {
    if (i > 0) result += ".";
    result += parts[i];
  }

  return result;
}

void Statement::replaceTypes(std::unordered_map<std::string, std::string> map) {
  if (auto expr = dynamic_cast<Expr *>(this)) {
    if (expr->extention) expr->extention->replaceTypes(map);
  }
  if (auto arg = dynamic_cast<Argument *>(this)) {
    applyType(arg->type, map);
  }
  if (auto par = dynamic_cast<ParenExpr *>(this)) {
    if (par->expr) par->expr->replaceTypes(map);
  }
  if (auto par2 = dynamic_cast<parenExpr *>(this)) {
    if (par2->expr) par2->expr->replaceTypes(map);
  }
  if (auto cwrite = dynamic_cast<CWrite *>(this)) {
    if (cwrite->expr) cwrite->expr->replaceTypes(map);
    return;
  }
  if (auto push = dynamic_cast<Push *>(this)) {
    if (push->expr) push->expr->replaceTypes(map);
    return;
  }
  if (auto pull = dynamic_cast<Pull *>(this)) {
    if (pull->expr) pull->expr->replaceTypes(map);
    return;
  }
  if (auto note = dynamic_cast<Note *>(this)) {
    return;
  }
  if (auto buy = dynamic_cast<Buy *>(this)) {
    if (buy->expr) buy->expr->replaceTypes(map);
    return;
  }
  if (auto var = dynamic_cast<Var *>(this)) {
    if (map.find(var->Ident) != map.end()) var->Ident = map[var->Ident];
    applyList(var->indices, map);
    return;
  }
  if (auto strList = dynamic_cast<StructList *>(this)) {
    applyList(strList->args, map);
    return;
  }
  if (auto notexpr = dynamic_cast<Not *>(this)) {
    if (notexpr->expr) notexpr->expr->replaceTypes(map);
    return;
  }
  if (auto ifexpr = dynamic_cast<IfExpr *>(this)) {
    if (ifexpr->expr) ifexpr->expr->replaceTypes(map);
    if (ifexpr->trueExpr) ifexpr->trueExpr->replaceTypes(map);
    if (ifexpr->falseExpr) ifexpr->falseExpr->replaceTypes(map);
    return;
  }
  if (auto comp = dynamic_cast<Compound *>(this)) {
    if (comp->expr1) comp->expr1->replaceTypes(map);
    if (comp->expr2) comp->expr2->replaceTypes(map);
    return;
  }
  if (auto ref = dynamic_cast<Reference *>(this)) {
    return;
  }
  if (auto deref = dynamic_cast<DeReference *>(this)) {
    applyType(deref->type, map);
    return;
  }
  if (auto lambda = dynamic_cast<Lambda *>(this)) {
    if (lambda->function) lambda->function->replaceTypes(map);
    return;
  }
  if (auto newExpr = dynamic_cast<NewExpr *>(this)) {
    applyType(newExpr->type, map);
    applyList(newExpr->args, map);
    applyTemplateTypes(newExpr->templateTypes, map);

    return;
  }
  if (auto callExpr = dynamic_cast<CallExpr *>(this)) {
    applyTemplateTypes(callExpr->templateTypes, map);
    if (callExpr->call) callExpr->call->replaceTypes(map);
    return;
  }
  if (auto assign = dynamic_cast<Assign *>(this)) {
    if (assign->expr) assign->expr->replaceTypes(map);
    applyList(assign->indices, map);
    return;
  }
  if (auto declare = dynamic_cast<Declare *>(this)) {
    applyType(declare->type, map);
    auto it = map.find(declare->TypeName);
    if (it != map.end()) declare->TypeName = it->second;
    if (!declare->requestType.empty()) {
      auto it2 = map.find(declare->requestType);
      if (it2 != map.end()) declare->requestType = it2->second;
    }
    return;
  }
  if (auto decArr = dynamic_cast<DecArr *>(this)) {
    applyType(decArr->type, map);
    applyList(decArr->indices, map);
    return;
  }
  if (auto decAssign = dynamic_cast<DecAssign *>(this)) {
    if (decAssign->declare) decAssign->declare->replaceTypes(map);
    if (decAssign->expr) decAssign->expr->replaceTypes(map);
    return;
  }
  if (auto decAssignArr = dynamic_cast<DecAssignArr *>(this)) {
    if (decAssignArr->declare) decAssignArr->declare->replaceTypes(map);
    if (decAssignArr->expr) decAssignArr->expr->replaceTypes(map);
    return;
  }
  if (auto call = dynamic_cast<Call *>(this)) {
    applyList(call->Args, map);
    applyTemplateTypes(call->genericTypes, map);
    return;
  }
  if (auto destruct = dynamic_cast<Destructure *>(this)) {
    if (destruct->expr) destruct->expr->replaceTypes(map);
    return;
  }
  if (auto forst = dynamic_cast<For *>(this)) {
    if (forst->declare) forst->declare->replaceTypes(map);
    if (forst->expr) forst->expr->replaceTypes(map);
    if (forst->increment) forst->increment->replaceTypes(map);
    if (forst->Run) forst->Run->replaceTypes(map);
    return;
  }
  if (auto foreach = dynamic_cast<ForEach *>(this)) {
    if (foreach->implementation)
      foreach
        ->implementation->replaceTypes(map);
    if (foreach->iterator)
      foreach
        ->iterator->replaceTypes(map);
    return;
  }
  if (auto func = dynamic_cast<Function *>(this)) {
    if (func->type.typeName.find('.') != std::string::npos) {
      func->type.typeName = replaceAllParts(func->type.typeName, map);
    } else {
      applyType(func->type, map);
    }

    if (func->useType.typeName.find('.') != std::string::npos) {
      func->useType.typeName = replaceAllParts(func->useType.typeName, map);
    } else {
      applyType(func->useType, map);
    }

    for (auto &t : func->argTypes) applyType(t, map);
    if (func->args) func->args->replaceTypes(map);
    if (func->statement) func->statement->replaceTypes(map);
    applyList(func->decoratorArgs, map);
    return;
  }
  if (auto ifst = dynamic_cast<If *>(this)) {
    if (ifst->expr) ifst->expr->replaceTypes(map);
    if (ifst->statement) ifst->statement->replaceTypes(map);
    if (ifst->elseStatement) ifst->elseStatement->replaceTypes(map);
    if (ifst->elseIf) ifst->elseIf->replaceTypes(map);
    return;
  }
  if (auto ret = dynamic_cast<Return *>(this)) {
    if (ret->expr) ret->expr->replaceTypes(map);
    return;
  }
  if (auto seq = dynamic_cast<Sequence *>(this)) {
    if (seq->Statement1) seq->Statement1->replaceTypes(map);
    if (seq->Statement2) seq->Statement2->replaceTypes(map);
    return;
  }
  if (auto strct = dynamic_cast<Struct *>(this)) {
    if (strct->statement) strct->statement->replaceTypes(map);
    return;
  }
  if (auto un = dynamic_cast<Union *>(this)) {
    for (auto &alias : un->aliases) {
      if (alias->isType()) {
        auto it = map.find(alias->getType().typeName);
        if (it != map.end()) {
          auto &typeName = it->second;
          auto it = parse::PRIMITIVE_TYPES.find(typeName);
          if (it != parse::PRIMITIVE_TYPES.end()) {
            alias->value = Type(typeName, gen::utils::toSize(it->second));
          } else {
            alias->value =
                Type(typeName, asmc::QWord);  // if it is a complex type, it
                                              // passes as a pointer.
          }
        }
      } else if (alias->isConstExpr()) {
        alias->getConstExpr()->replaceTypes(map);
      }
    }
    if (un->statement) un->statement->replaceTypes(map);
    return;
  }
  if (auto unionConstructor = dynamic_cast<UnionConstructor *>(this)) {
    applyType(unionConstructor->unionType, map);
    if (unionConstructor->expr) {
      unionConstructor->expr->replaceTypes(map);
    }
    applyTemplateTypes(unionConstructor->templateTypes, map);
    return;
  }
  if (auto whileSt = dynamic_cast<While *>(this)) {
    if (whileSt->expr) whileSt->expr->replaceTypes(map);
    if (whileSt->stmt) whileSt->stmt->replaceTypes(map);
    return;
  }
  if (auto cls = dynamic_cast<Class *>(this)) {
    if (cls->contract) cls->contract->replaceTypes(map);
    if (cls->statement) cls->statement->replaceTypes(map);
    return;
  }
  if (auto lambdaExpr = dynamic_cast<Lambda *>(this)) {
    if (lambdaExpr->function) lambdaExpr->function->replaceTypes(map);
    return;
  }
  // other statements: Break, Continue, Enum, Import, Inc, Dec, Delete,
  // Transform, Iflush have no type information
}

}  // namespace ast
