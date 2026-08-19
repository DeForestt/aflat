#include "Parser/AST/Expressions/Bubble.hpp"

#include <variant>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Scope/ScopeManager.hpp"
#include "CodeGenerator/Types.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements/Match.hpp"

namespace ast {

/*
 * The Bubble operator looks like `expr?` it is used to "bubble" up errors
 * from an expression. All that it does is expand to
 * ```
 * match expr {
 *    Ok(value) => value,
 *    Err(err) => return err
 * }
 */

gen::GenerationResult const
Bubble::generateExpression(gen::CodeGenerator &generator, asmc::Size size,
                           std::string typeHint) {
  asmc::File file;
  auto exprResult = generator.GenExpr(expr, file);
  auto t = generator.getType(exprResult.type, file);
  if (!t) {
    generator.alert("Bubble expression type not found: " + exprResult.type,
                    true, __FILE__, __LINE__);
    return {.file = file, .expr = std::nullopt};
  }

  auto unionType = dynamic_cast<gen::Union *>(*t);
  if (!unionType) {
    generator.alert("Bubble expression type is not a union: " + exprResult.type,
                    true, __FILE__, __LINE__);
    return {.file = file, .expr = std::nullopt};
  }

  ast::Type bubbleReturnType;
  bool foundOk = false;
  for (const auto &alias : unionType->aliases) {
    if (alias.name != "Ok" || !std::holds_alternative<ast::Type *>(alias.value))
      continue;
    bubbleReturnType = *std::get<ast::Type *>(alias.value);
    foundOk = true;
    break;
  }
  if (!foundOk) {
    generator.alert("Bubble expression type has no Ok payload: " +
                        exprResult.type,
                    true, __FILE__, __LINE__);
    return {.file = file, .expr = std::nullopt};
  }

  const auto tempName =
      "$" + std::to_string(generator.tempCount()++) + "_bubble_temp";
  auto type = ast::Type(exprResult.type, exprResult.size);
  auto mod =
      gen::scope::ScopeManager::getInstance()->assign(tempName, type, false);
  auto symbol = gen::scope::ScopeManager::getInstance()->get(tempName);
  symbol->owned = exprResult.owned;

  auto saveResult = new asmc::Mov();
  saveResult->logicalLine = logicalLine;
  saveResult->from = exprResult.access;
  saveResult->to = "-" + std::to_string(mod) + "(%rbp)";
  saveResult->size = exprResult.size;
  saveResult->op = exprResult.op;
  file.text << saveResult;

  auto matchExpr = new ast::Match();
  auto matchValue = new ast::Var();
  matchValue->Ident = tempName;
  matchValue->logicalLine = logicalLine;
  matchExpr->expr = matchValue;
  matchExpr->returns = bubbleReturnType;

  ast::Match::Case caseOne;
  caseOne.pattern.aliasName = "Ok";
  caseOne.pattern.veriableName = "value";
  auto var = new ast::Var();
  var->Ident = "value";
  auto returnStmt = new ast::Return();
  returnStmt->expr = var;
  returnStmt->implicit = true;
  returnStmt->resolver = true;
  caseOne.statement = returnStmt;
  ast::Match::Case caseTwo;
  caseTwo.pattern.aliasName = "Err";
  caseTwo.pattern.veriableName = "err";
  auto errVar = new ast::Var();
  errVar->Ident = "err";
  auto returnErr = new ast::Return();
  returnErr->expr = errVar;
  returnErr->implicit = false;
  returnErr->resolver = false;
  caseTwo.statement = returnErr;
  matchExpr->cases.push_back(caseOne);
  matchExpr->cases.push_back(caseTwo);

  auto result = matchExpr->generateExpression(generator, size, typeHint);
  file << result.file;
  return {.file = file, .expr = result.expr};
}

} // namespace ast
