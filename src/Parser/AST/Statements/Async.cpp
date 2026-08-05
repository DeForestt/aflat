#include "Parser/AST.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "Exceptions.hpp"
#include "Parser/AST/Statements/Function.hpp"

namespace ast {
namespace {

void appendMove(asmc::File &file, const std::string &from,
                const std::string &to, asmc::Size size, int line) {
  auto *move = new asmc::Mov();
  move->from = from;
  move->to = to;
  move->size = size;
  move->logicalLine = line;
  file.text << move;
}

void appendSuspension(asmc::File &file, Function *function,
                      const std::string &runtimeFunction, int line) {
  const int state = function->nextAsyncState();
  appendMove(file, "$" + std::to_string(state), "%edi", asmc::DWord, line);
  appendMove(file, "%rbp", "%rsi", asmc::QWord, line);
  appendMove(file, "$__AF_ASYNC_FRAME_SIZE__", "%rdx", asmc::QWord, line);
  auto *call = new asmc::Call();
  call->function = runtimeFunction;
  call->logicalLine = line;
  file.text << call;
  auto *suspendReturn = new asmc::Return();
  suspendReturn->logicalLine = line;
  file.text << suspendReturn;
  auto *resume = new asmc::Label();
  resume->label = function->asyncStateLabel(state);
  resume->logicalLine = line;
  file.text << resume;
}

} // namespace

std::string AsyncExpression::toString() {
  const char *keyword = kind == AsyncExpressionKind::Await   ? "await"
                        : kind == AsyncExpressionKind::Spawn ? "spawn"
                                                             : "run";
  return std::string(keyword) + " " + (expr ? expr->toString() : "");
}

std::string Pause::toString() {
  return "pause " + (expr ? expr->toString() : "") + ";";
}

gen::GenerationResult const Pause::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  if (generator.currentFunction() == nullptr ||
      !(generator.currentFunction()->isAsync ||
        generator.currentFunction()->isAsyncBody)) {
    generator.alert("pause may only be used inside an async fn", true, __FILE__,
                    __LINE__);
  }
  generator.GenExpr(expr, file);
  appendSuspension(file, generator.currentFunction(), "af_task_pause_suspend",
                   logicalLine);
  return {file, std::nullopt};
}

gen::GenerationResult const Yield::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  if (generator.currentFunction() == nullptr ||
      !(generator.currentFunction()->isAsync ||
        generator.currentFunction()->isAsyncBody)) {
    generator.alert("yield may only be used inside an async fn", true, __FILE__,
                    __LINE__);
  }
  appendSuspension(file, generator.currentFunction(), "af_task_yield_suspend",
                   logicalLine);
  return {file, std::nullopt};
}

std::string TaskControl::toString() {
  const char *keyword = kind == TaskControlKind::Cancel ? "cancel" : "detach";
  return std::string(keyword) + " " + (expr ? expr->toString() : "") + ";";
}

gen::GenerationResult const
TaskControl::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  auto task = generator.GenExpr(expr, file, asmc::QWord);
  if (task.type.rfind("task<", 0) != 0 || task.type.back() != '>') {
    generator.alert(
        std::string(kind == TaskControlKind::Cancel ? "cancel" : "detach") +
            " requires task<T>, got " + task.type,
        true, __FILE__, __LINE__);
  }
  auto *move = new asmc::Mov();
  move->from = task.access;
  move->to = generator.intArgs()[0].get(asmc::QWord);
  move->size = asmc::QWord;
  move->logicalLine = logicalLine;
  file.text << move;
  auto *call = new asmc::Call();
  call->function =
      kind == TaskControlKind::Cancel ? "af_task_cancel" : "af_task_detach";
  call->logicalLine = logicalLine;
  file.text << call;
  return {file, std::nullopt};
}

} // namespace ast
