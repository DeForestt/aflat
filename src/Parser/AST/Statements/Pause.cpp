#include "Parser/AST/Statements/Pause.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "Parser/AST/Statements/Assign.hpp"
#include "Parser/AST/Statements/Return.hpp"

namespace ast {
Pause::Pause(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  if (auto sym = dynamic_cast<lex::OpSym *>(tokens.peek())) {
    if (sym->Sym == '{') {
      hasBlock = true;
      tokens.pop();
      body = parser.parseStmt(tokens);
      logicalLine = sym->lineCount;
      return;
    }
  }

  body = parser.parseStmt(tokens, true);
  if (body != nullptr)
    logicalLine = body->logicalLine;
}

gen::GenerationResult const Pause::generate(gen::CodeGenerator &generator) {
  asmc::File file;

  if (!generator.coroutineActive() || generator.currentFunction() == nullptr ||
      !generator.currentFunction()->asyncFunction) {
    generator.alert("pause can only be used inside async functions", true,
                    __FILE__, __LINE__);
  }

  const auto pauseIndex = generator.coroutineStateIndex();
  generator.coroutineStateIndex() = pauseIndex + 1;

  if (body != nullptr) {
    file << generator.GenSTMT(body);
  }

  auto assignField = [&](const std::string &field, int value) {
    auto assign = new ast::Assign();
    assign->logicalLine = logicalLine;
    assign->Ident = generator.coroutineTaskIdent();
    assign->override = true;
    assign->modList.push(field);
    auto literal = new ast::IntLiteral();
    literal->logicalLine = logicalLine;
    literal->val = value;
    assign->expr = literal;
    file << assign->generate(generator).file;
  };

  assignField("state", pauseIndex + 1);
  assignField("status", 2);

  auto ret = new ast::Return();
  ret->logicalLine = logicalLine;
  auto result = new ast::Var();
  result->logicalLine = logicalLine;
  result->Ident = generator.coroutineTaskIdent();
  result->modList.push("result");
  ret->expr = result;
  file << ret->generate(generator).file;

  std::string resumeLabel;
  if (pauseIndex + 1 <
      static_cast<int>(generator.coroutineResumeLabels().size())) {
    resumeLabel = generator.coroutineResumeLabels()[pauseIndex + 1];
  } else {
    resumeLabel = ".L" + generator.moduleId() + "_" +
                  generator.currentFunction()->ident.ident + "_resume_" +
                  std::to_string(pauseIndex + 1);
  }
  auto label = new asmc::Label();
  label->logicalLine = logicalLine;
  label->label = resumeLabel;
  file.text << label;
  return {file, std::nullopt};
}
} // namespace ast
