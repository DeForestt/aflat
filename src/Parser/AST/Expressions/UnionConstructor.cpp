#include <algorithm>
#include <iostream>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Scope/ScopeManager.hpp"
#include "Parser/AST.hpp"

namespace ast {

namespace {

void emitCallWithReceiver(gen::CodeGenerator &generator, asmc::File &file,
                          const std::string &function,
                          const std::string &receiver,
                          const std::optional<std::string> &argument,
                          int logicalLine) {
  for (const auto *reg : {"%rdi", "%rsi", "%rdx"}) {
    auto *push = new asmc::Push();
    push->logicalLine = logicalLine;
    push->op = generator.registers()[reg]->get(asmc::QWord);
    file.text << push;
  }

  auto *loadReceiver = new asmc::Mov();
  loadReceiver->logicalLine = logicalLine;
  loadReceiver->size = asmc::QWord;
  loadReceiver->from = receiver;
  loadReceiver->to = generator.registers()["%rdi"]->get(asmc::QWord);
  file.text << loadReceiver;

  if (argument.has_value()) {
    auto *loadArgument = new asmc::Mov();
    loadArgument->logicalLine = logicalLine;
    loadArgument->size = asmc::QWord;
    loadArgument->from = argument.value();
    loadArgument->to = generator.registers()["%rsi"]->get(asmc::QWord);
    file.text << loadArgument;
  }

  auto *call = new asmc::Call();
  call->logicalLine = logicalLine;
  call->function = function;
  file.text << call;

  for (const auto *reg : {"%rdx", "%rsi", "%rdi"}) {
    auto *pop = new asmc::Pop();
    pop->logicalLine = logicalLine;
    pop->op = generator.registers()[reg]->get(asmc::QWord);
    file.text << pop;
  }
}

} // namespace

gen::GenerationResult const
UnionConstructor::getStaticExpr(gen::CodeGenerator &generator, asmc::Size size,
                                std::string typeHint) {
  asmc::File file;

  auto callExpr = new ast::CallExpr();
  callExpr->call = new ast::Call();
  callExpr->templateTypes = templateTypes;
  callExpr->call->ident = unionType.typeName;
  callExpr->call->logicalLine = logicalLine;

  auto resolvedCall = generator.GenExpr(callExpr, file, size, typeHint);
  return {file, resolvedCall};
}

gen::GenerationResult const
UnionConstructor::getDynamicExpr(gen::CodeGenerator &generator, asmc::Size size,
                                 std::string typeHint) {
  asmc::File file;
  auto newExpr = new ast::NewExpr();
  newExpr->type = unionType;
  newExpr->logicalLine = logicalLine;
  newExpr->templateTypes = templateTypes;

  auto resolvedExpr = generator.GenExpr(newExpr, file, size, typeHint);
  return {file, resolvedExpr};
}

gen::GenerationResult const
UnionConstructor::generateExpression(gen::CodeGenerator &generator,
                                     asmc::Size size, std::string typeHint) {
  asmc::File file;
  if (!templateTypes.empty() &&
      unionType.typeName.find('<') == std::string::npos) {
    unionType.typeName += "<";
    for (size_t i = 0; i < templateTypes.size(); ++i) {
      unionType.typeName += templateTypes[i];
      if (i < templateTypes.size() - 1) {
        unionType.typeName += ",";
      }
    }
    unionType.typeName += ">";
  }
  if (unionType.typeName.find('<') != std::string::npos)
    templateTypes.clear();

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
    generator.alert("Variant " + variantName + " not found in union " +
                        unionType.typeName,
                    true, __FILE__, __LINE__);
    return {file, std::nullopt};
  }

  int variantIndex = std::distance(unionGen->aliases.begin(), it);
  auto alias = *it;

  auto internalAccess = dynamic ? getDynamicExpr(generator, size, typeHint)
                                : getStaticExpr(generator, size, typeHint);
  file << internalAccess.file;

  // create a temporary variable to hold the union
  auto tempName = "$" + std::to_string(generator.tempCount()++) + "_temp";
  auto mod = gen::scope::ScopeManager::getInstance()->assign(tempName,
                                                             unionType, false);

  auto mov = new asmc::Mov();
  mov->logicalLine = logicalLine;
  mov->from = internalAccess.expr->access;
  mov->to = generator.registers()["%rax"]->get(asmc::QWord);
  mov->size = asmc::QWord;

  file.text << mov;

  auto store = new asmc::Mov();
  store->logicalLine = logicalLine;
  store->from = generator.registers()["%rax"]->get(asmc::QWord);
  store->to = "-" + std::to_string(mod) + "(%rbp)";
  store->size = asmc::QWord;

  file.text << store;

  auto useExpr = std::holds_alternative<ast::Type *>(alias.value)
                     ? expr
                     : std::get<ast::Expr *>(alias.value);
  const bool explicitlyTransferred =
      dynamic_cast<ast::Buy *>(useExpr) != nullptr;
  const bool addressableSource =
      dynamic_cast<ast::Var *>(useExpr) != nullptr ||
      dynamic_cast<ast::Reference *>(useExpr) != nullptr;
  bool ownsPayloadWrapper = explicitlyTransferred || !addressableSource;

  auto fromExpr = generator.GenExpr(useExpr, file, asmc::QWord);

  if (std::holds_alternative<ast::Type *>(alias.value)) {
    if (!generator.canAssign(*std::get<ast::Type *>(alias.value), fromExpr.type,
                             "Cannot assign type {} to union variant {}")) {
      auto prev = fromExpr;
      fromExpr = generator.GenExpr(
          generator.imply(expr, std::get<ast::Type *>(alias.value)->typeName),
          file);
      fromExpr.adoptImmutableRequirement(prev);
    }
  }
  if (parse::PRIMITIVE_TYPES.find(fromExpr.type) ==
      parse::PRIMITIVE_TYPES.end()) {
    auto tnt = generator.getType(fromExpr.type, file);
    auto cls = tnt ? dynamic_cast<gen::Class *>(*tnt) : nullptr;
    if (!explicitlyTransferred && addressableSource && cls != nullptr &&
        cls->publicNameTable["__copy__"] != nullptr) {
      auto call = new ast::CallExpr();
      call->call = new ast::Call();
      call->call->ident = "__copy__";
      call->call->publify = fromExpr.type;
      call->call->Args.push(useExpr);
      call->logicalLine = logicalLine;
      auto prev = fromExpr;
      fromExpr = generator.GenExpr(call, file, asmc::QWord);
      fromExpr.adoptImmutableRequirement(prev);
      ownsPayloadWrapper = true;
    }
  }

  // check if the expression is a primitive type

  if (parse::PRIMITIVE_TYPES.find(fromExpr.type) !=
      parse::PRIMITIVE_TYPES.end()) {
    file << generator.setOffset(store->to, 0, fromExpr.access, fromExpr.size,
                                fromExpr.op);
  } else {
    if (ownsPayloadWrapper && !fromExpr.owned) {
      generator.alert("cannot store unowned value of type `" + fromExpr.type +
                          "` in ownership-bearing union variant `" +
                          variantName + "`",
                      true, __FILE__, __LINE__);
    }

    const auto payloadTempName =
        "$" + std::to_string(generator.tempCount()++) + "_union_payload";
    const auto payloadMod = gen::scope::ScopeManager::getInstance()->assign(
        payloadTempName, ast::Type("adr", asmc::QWord), false);
    const std::string payloadSlot = "-" + std::to_string(payloadMod) + "(%rbp)";

    auto *savePayload = new asmc::Mov();
    savePayload->logicalLine = logicalLine;
    savePayload->size = asmc::QWord;
    savePayload->from = fromExpr.access;
    savePayload->to = payloadSlot;
    file.text << savePayload;

    auto *payloadEntry = generator.getType(fromExpr.type, file);
    auto *payloadClass = payloadEntry == nullptr
                             ? nullptr
                             : dynamic_cast<gen::Class *>(*payloadEntry);
    auto *transfer = payloadClass == nullptr
                         ? nullptr
                         : payloadClass->publicNameTable["__transfer_to__"];

    if (ownsPayloadWrapper && payloadClass != nullptr &&
        payloadClass->uniqueType && transfer != nullptr) {
      const std::string scopeName = transfer->scopeName != "global"
                                        ? transfer->scopeName
                                        : payloadClass->Ident;
      emitCallWithReceiver(generator, file,
                           "pub_" + scopeName + "_" + transfer->ident.ident,
                           payloadSlot, store->to, logicalLine);
    } else {
      // Non-unique composite values do not expose a transfer hook. Moving
      // their inline state is still safe once the redundant source wrapper is
      // released without running its lifecycle method.
      file << generator.memMove(payloadSlot, store->to, alias.byteSize);
    }

    if (ownsPayloadWrapper) {
      // The payload now lives inline in the union. Its former heap wrapper
      // must be released without destroying the state that was transferred.
      emitCallWithReceiver(generator, file, "af_free", payloadSlot,
                           std::nullopt, logicalLine);
    }
  }

  // Payload generation may clobber %rax, so write the tag through the saved
  // union storage pointer rather than the original allocation register.
  file << generator.setOffset(store->to, unionGen->largestSize,
                              "$" + std::to_string(variantIndex), asmc::DWord);

  auto restoreUnionPointer = new asmc::Mov();
  restoreUnionPointer->logicalLine = logicalLine;
  restoreUnionPointer->from = store->to;
  restoreUnionPointer->to = generator.registers()["%rax"]->get(asmc::QWord);
  restoreUnionPointer->size = asmc::QWord;
  file.text << restoreUnionPointer;

  auto out = gen::Expr();
  out.access = restoreUnionPointer->to;
  out.size = asmc::QWord;
  out.type = unionType.typeName;
  out.op = asmc::Hard;
  out.owned = internalAccess.expr->owned;

  return {file, out};
}
}; // namespace ast
