#include "Parser/AST/Statements/Await.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "Parser/AST/Statements/Assign.hpp"
#include "Parser/AST/Statements/DecAssign.hpp"
#include "Parser/AST/Statements/Declare.hpp"
#include "Parser/AST/Statements/Return.hpp"

namespace ast {
Await::Await(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  expr = parser.parseExpr(tokens);
  logicalLine = expr != nullptr ? expr->logicalLine : 0;
}

static void assignCoroutineField(gen::CodeGenerator &generator,
                                 asmc::File &file, const std::string &taskIdent,
                                 const std::string &field, int value,
                                 int logicalLine) {
  auto assign = new ast::Assign();
  assign->logicalLine = logicalLine;
  assign->Ident = taskIdent;
  assign->override = true;
  assign->modList.push(field);
  auto literal = new ast::IntLiteral();
  literal->logicalLine = logicalLine;
  literal->val = value;
  assign->expr = literal;
  file << assign->generate(generator).file;
}

gen::GenerationResult const
Await::generateExpression(gen::CodeGenerator &generator, asmc::Size size,
                          std::string typeHint) {
  asmc::File file;

  if (!generator.coroutineActive() || generator.currentFunction() == nullptr ||
      !generator.currentFunction()->asyncFunction) {
    generator.alert("await can only be used inside async functions", true,
                    __FILE__, __LINE__);
  }

  if (expr == nullptr) {
    generator.alert("await requires a task expression", true, __FILE__,
                    __LINE__);
  }

  const auto awaitIndex = generator.coroutineStateIndex();
  generator.coroutineStateIndex() = awaitIndex + 1;

  auto *tempDeclare = new ast::Declare();
  tempDeclare->ident =
      "__await_task_" + std::to_string(generator.tempCount()++);
  tempDeclare->type = ast::Type("let", asmc::QWord);
  tempDeclare->mut = false;
  tempDeclare->readOnly = true;

  auto *tempAssign = new ast::DecAssign();
  tempAssign->declare = tempDeclare;
  tempAssign->mute = false;
  tempAssign->logicalLine = logicalLine;
  tempAssign->expr = expr;
  file << tempAssign->generate(generator).file;

  if (tempDeclare->type.typeName.rfind("task<", 0) != 0) {
    generator.alert(
        "await expects an expression that resolves to task<T>, got " +
            tempDeclare->type.typeName,
        true, __FILE__, __LINE__);
  }

  auto statusExpr = new ast::Var();
  statusExpr->Ident = tempDeclare->ident;
  statusExpr->logicalLine = logicalLine;
  statusExpr->modList.push("status");

  gen::Expr statusValue = generator.GenExpr(statusExpr, file);

  std::string resumeLabel;
  if (awaitIndex + 1 <
      static_cast<int>(generator.coroutineResumeLabels().size())) {
    resumeLabel = generator.coroutineResumeLabels()[awaitIndex + 1];
  } else {
    resumeLabel = ".L" + generator.moduleId() + "_" +
                  generator.currentFunction()->ident.ident + "_resume_" +
                  std::to_string(awaitIndex + 1);
  }

  auto cmp = new asmc::Cmp();
  cmp->logicalLine = logicalLine;
  cmp->from = "$3";
  cmp->to = statusValue.access;
  cmp->size = statusValue.size;
  file.text << cmp;

  auto je = new asmc::Je();
  je->logicalLine = logicalLine;
  je->to = resumeLabel;
  file.text << je;

  auto waiterAssign = new ast::Assign();
  waiterAssign->logicalLine = logicalLine;
  waiterAssign->Ident = tempDeclare->ident;
  waiterAssign->override = true;
  waiterAssign->modList.push("waiter");
  auto waiterVar = new ast::Var();
  waiterVar->logicalLine = logicalLine;
  waiterVar->Ident = generator.coroutineTaskIdent();
  waiterAssign->expr = waiterVar;
  file << waiterAssign->generate(generator).file;

  auto frameAssign = new ast::Assign();
  frameAssign->logicalLine = logicalLine;
  frameAssign->Ident = generator.coroutineTaskIdent();
  frameAssign->override = true;
  frameAssign->modList.push("frame");
  auto frameVar = new ast::Var();
  frameVar->logicalLine = logicalLine;
  frameVar->Ident = tempDeclare->ident;
  frameAssign->expr = frameVar;
  file << frameAssign->generate(generator).file;

  assignCoroutineField(generator, file, generator.coroutineTaskIdent(), "state",
                       awaitIndex + 1, logicalLine);
  assignCoroutineField(generator, file, generator.coroutineTaskIdent(),
                       "status", 2, logicalLine);

  auto ret = new ast::Return();
  ret->logicalLine = logicalLine;
  auto result = new ast::Var();
  result->logicalLine = logicalLine;
  result->Ident = generator.coroutineTaskIdent();
  result->modList.push("result");
  ret->expr = result;
  file << ret->generate(generator).file;

  auto label = new asmc::Label();
  label->logicalLine = logicalLine;
  label->label = resumeLabel;
  file.text << label;

  auto restoreAssign = new ast::Assign();
  restoreAssign->logicalLine = logicalLine;
  restoreAssign->Ident = tempDeclare->ident;
  restoreAssign->override = true;
  auto restoreVar = new ast::Var();
  restoreVar->logicalLine = logicalLine;
  restoreVar->Ident = generator.coroutineTaskIdent();
  restoreVar->modList.push("frame");
  restoreAssign->expr = restoreVar;
  file << restoreAssign->generate(generator).file;

  auto resultExpr = new ast::Var();
  resultExpr->Ident = tempDeclare->ident;
  resultExpr->logicalLine = logicalLine;
  resultExpr->modList.push("result");
  auto resolved = generator.GenExpr(resultExpr, file, size, typeHint);
  return {file, resolved};
}

gen::GenerationResult const Await::generate(gen::CodeGenerator &generator) {
  return generateExpression(generator, asmc::AUTO);
}
} // namespace ast
