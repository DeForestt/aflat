#include <algorithm>
#include <iostream>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"

namespace ast {

gen::GenerationResult const UnionConstructor::getStaticExpr(
    gen::CodeGenerator &generator, asmc::Size size, std::string typeHint) {
  asmc::File file;

  auto callExpr = new ast::CallExpr();
  callExpr->call = new ast::Call();
  callExpr->templateTypes = templateTypes;
  callExpr->call->ident = unionType.typeName;
  callExpr->call->logicalLine = logicalLine;

  auto resolvedCall = generator.GenExpr(callExpr, file, size, typeHint);
  return {file, resolvedCall};
}

gen::GenerationResult const UnionConstructor::getDynamicExpr(
    gen::CodeGenerator &generator, asmc::Size size, std::string typeHint) {
  asmc::File file;
  auto newExpr = new ast::NewExpr();
  newExpr->type = unionType;
  newExpr->logicalLine = logicalLine;
  newExpr->templateTypes = templateTypes;

  auto resolvedExpr = generator.GenExpr(newExpr, file, size, typeHint);
  return {file, resolvedExpr};
}

gen::GenerationResult const UnionConstructor::generateExpression(
    gen::CodeGenerator &generator, asmc::Size size, std::string typeHint) {
  asmc::File file;
  if (templateTypes.size() > 0) {
    for (const auto &type : templateTypes) {
      unionType.typeName += "." + type;
    }
  }

  auto type = generator.getType(unionType.typeName, file);
  if (type == nullptr) {
    generator.alert("Type " + unionType.typeName + " not found", true, __FILE__,
                    __LINE__);
    return {file, std::nullopt};
  }

  auto t = *type;
  auto unionGen = dynamic_cast<gen::Union *>(t);
  if (unionGen == nullptr) {
    generator.alert("Type " + unionType.typeName +
                        " is not a union type, cannot construct it as such",
                    true, __FILE__, __LINE__);
    return {file, std::nullopt};
  }

  auto it = std::find_if(unionGen->aliases.begin(), unionGen->aliases.end(),
                         [&](const gen::Union::Alias &alias) {
                           return alias.name == variantName;
                         });

  if (it == unionGen->aliases.end()) {
    generator.alert(
        "Variant " + variantName + " not found in union " + unionType.typeName,
        true, __FILE__, __LINE__);
    return {file, std::nullopt};
  }

  int variantIndex = std::distance(unionGen->aliases.begin(), it);
  auto alias = *it;

  auto internalAccess = dynamic ? getDynamicExpr(generator, size, typeHint)
                                : getStaticExpr(generator, size, typeHint);
  file << internalAccess.file;

  // create a temporary variable to hold the union
  auto tempName = "$" + std::to_string(generator.tempCount++) + "_temp";
  auto mod = gen::scope::ScopeManager::getInstance()->assign(tempName,
                                                             unionType, false);

  auto mov = new asmc::Mov();
  mov->logicalLine = logicalLine;
  mov->from = internalAccess.expr->access;
  mov->to = "-" + std::to_string(mod) + "(%rbp)";
  mov->size = asmc::QWord;

  file.text << mov;

  auto fromExpr = std::holds_alternative<ast::Type *>(alias.value)
                      ? generator.GenExpr(expr, file, asmc::QWord)
                      : generator.GenExpr(std::get<ast::Expr *>(alias.value),
                                          file, asmc::QWord);

  if (std::holds_alternative<ast::Type *>(alias.value)) {
    if (!generator.canAssign(*std::get<ast::Type *>(alias.value), fromExpr.type,
                             "Cannot assign type {} to union variant {}")) {
      fromExpr = generator.GenExpr(
          generator.imply(expr, std::get<ast::Type *>(alias.value)->typeName),
          file);
    }
  }

  // check if the expression is a primitive type

  if (parse::PRIMITIVE_TYPES.find(fromExpr.type) !=
      parse::PRIMITIVE_TYPES.end()) {
    file << generator.setOffset(mov->to, 0, fromExpr.access, fromExpr.size);
  } else {
    file << generator.memMove(fromExpr.access, mov->to, unionGen->largestSize);
  };

  // set the variant index in the union
  file << generator.setOffset(mov->to, unionGen->largestSize,
                              "$" + std::to_string(variantIndex), asmc::DWord);

  auto out = gen::Expr();
  out.access = mov->to;
  out.size = asmc::QWord;
  out.type = unionType.typeName;
  out.op = asmc::Hard;

  return {file, out};
}
};  // namespace ast
