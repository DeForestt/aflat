#include "Parser/AST/Statements/For.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Scope/ScopeManager.hpp"

namespace ast {
For::For(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->declare = parser.parseStmt(tokens, true);
  this->logicalLine = this->declare->logicalLine;
  auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());

  if (sym == nullptr)
    throw err::Exception(
        "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
        " Unterminated for loop initializer");
  if (sym->Sym != ';')
    throw err::Exception(
        "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
        "unterminated for loop initializer");

  tokens.pop();

  this->expr = parser.parseExpr(tokens);

  sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym == nullptr)
    throw err::Exception(
        "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
        " Unterminated for loop condition");
  if (sym->Sym != ';')
    throw err::Exception(
        "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
        "unterminated for loop condition");
  tokens.pop();

  this->increment = parser.parseStmt(tokens, true);

  sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym != nullptr) {
    if (sym->Sym == ';') {
      tokens.pop();
    };
    sym = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (sym != nullptr) {
      if (sym->Sym == '{') {
        tokens.pop();
        this->Run = parser.parseStmt(tokens);
      } else
        throw err::Exception(
            "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
            " Unopened for loop body");
    }
  } else
    this->Run = parser.parseStmt(tokens, true);
}

gen::GenerationResult const For::generate(gen::CodeGenerator &generator) {
  asmc::File OutputFile = asmc::File();
  gen::scope::ScopeManager::getInstance()->pushScope(true);
  generator.beginStackCleanupFrame();
  OutputFile << generator.emitStackCleanupHeadReset();

  asmc::Label *label1 = new asmc::Label();
  label1->logicalLine = this->logicalLine;
  label1->label = ".L" + generator.nameTable().head->data.ident.ident +
                  std::to_string(generator.labelCount());
  generator.labelCount()++;

  asmc::Label *label2 = new asmc::Label();
  label2->logicalLine = this->logicalLine;
  label2->label = ".L" + generator.nameTable().head->data.ident.ident +
                  std::to_string(generator.labelCount());
  generator.labelCount()++;

  asmc::Label *breakLabel = new asmc::Label();
  breakLabel->logicalLine = this->logicalLine;
  breakLabel->label = ".L" + generator.nameTable().head->data.ident.ident +
                      std::to_string(generator.labelCount());
  generator.labelCount()++;

  asmc::Label *continueLabel = new asmc::Label();
  continueLabel->logicalLine = this->logicalLine;
  continueLabel->label = ".L" + generator.nameTable().head->data.ident.ident +
                         std::to_string(generator.labelCount());
  generator.labelCount()++;
  asmc::Label *breakCleanupLabel = new asmc::Label();
  breakCleanupLabel->logicalLine = this->logicalLine;
  breakCleanupLabel->label = ".L" +
                             generator.nameTable().head->data.ident.ident +
                             std::to_string(generator.labelCount());
  generator.labelCount()++;
  asmc::Label *continueCleanupLabel = new asmc::Label();
  continueCleanupLabel->logicalLine = this->logicalLine;
  continueCleanupLabel->label = ".L" +
                                generator.nameTable().head->data.ident.ident +
                                std::to_string(generator.labelCount());
  generator.labelCount()++;
  OutputFile << generator.GenSTMT(this->declare);
  asmc::Jmp *jmp = new asmc::Jmp();
  jmp->to = label2->label;
  OutputFile.text << jmp;

  OutputFile.text << label1;

  generator.breakContext().push(breakCleanupLabel->label);
  generator.continueContext().push(continueCleanupLabel->label);

  gen::scope::ScopeManager::getInstance()->pushScope(true);
  generator.beginStackCleanupFrame();
  OutputFile << generator.emitStackCleanupHeadReset();
  OutputFile << generator.GenSTMT(this->Run);
  OutputFile << generator.emitStackCleanups();
  OutputFile.text << continueCleanupLabel;
  OutputFile << generator.emitStackCleanups();
  auto *continueJump = new asmc::Jmp();
  continueJump->logicalLine = this->logicalLine;
  continueJump->to = continueLabel->label;
  OutputFile.text << continueJump;
  OutputFile.text << breakCleanupLabel;
  OutputFile << generator.emitStackCleanups();
  auto *breakJump = new asmc::Jmp();
  breakJump->logicalLine = this->logicalLine;
  breakJump->to = breakLabel->label;
  OutputFile.text << breakJump;
  generator.endStackCleanupFrame();
  gen::scope::ScopeManager::getInstance()->popScope(&generator, OutputFile);
  OutputFile.text << continueLabel;
  OutputFile << generator.GenSTMT(this->increment);

  OutputFile.text << label2;
  generator.breakContext().pop();
  generator.continueContext().pop();

  gen::Expr expr = generator.GenExpr(this->expr, OutputFile);

  ast::Type t = ast::Type();
  t.typeName = "bool";
  t.size = asmc::Byte;
  generator.canAssign(t, expr.type,
                      "for conditions must be of type {}, {} was given.");

  asmc::Mov *mov = new asmc::Mov();
  mov->logicalLine = this->logicalLine;

  mov->size = expr.size;

  mov->from = expr.access;

  mov->to = generator.registers()["%eax"]->get(mov->size);

  asmc::Cmp *cmp = new asmc::Cmp();
  cmp->logicalLine = this->logicalLine;
  asmc::Je *je = new asmc::Je();
  je->logicalLine = this->logicalLine;
  je->to = label1->label;

  cmp->from = "$1";
  cmp->to = mov->to;
  cmp->size = expr.size;

  OutputFile.text << mov;
  OutputFile.text << cmp;
  OutputFile.text << je;
  OutputFile.text << breakLabel;
  OutputFile << generator.emitStackCleanups();
  generator.endStackCleanupFrame();
  gen::scope::ScopeManager::getInstance()->popScope(&generator, OutputFile);
  generator.currentFunction()->has_return = false;
  return {OutputFile, std::nullopt};
}

} // namespace ast
