#include "Parser/AST/Expressions/Bubble.hpp"

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

gen::GenerationResult const Bubble::generateExpression(
    gen::CodeGenerator &generator, asmc::Size size, std::string typeHint) {
  asmc::File file;
  auto matchExpr = new ast::Match();
  matchExpr->expr = expr;

  auto caseOne = new ast::Match::Case();
  caseOne->pattern.aliasName = "Ok";
  caseOne->pattern.veriableName = "value";
  auto var = new ast::Var();
  var->Ident = "value";
  auto returnStmt = new ast::Return();
  returnStmt->expr = var;
  returnStmt->implicit = true;
  returnStmt->resolver = true;
  caseOne->statement = returnStmt;
  auto caseTwo = new ast::Match::Case();
  caseTwo->pattern.aliasName = "Err";
  caseTwo->pattern.veriableName = "err";
  auto errVar = new ast::Var();
  errVar->Ident = "err";
  auto returnErr = new ast::Return();
  returnErr->expr = errVar;
  returnErr->implicit = false;
  returnErr->resolver = false;
  caseTwo->statement = returnErr;
  matchExpr->cases.push_back(*caseOne);
  matchExpr->cases.push_back(*caseTwo);

  return matchExpr->generateExpression(generator, size, typeHint);
}

}  // namespace ast
