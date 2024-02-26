#include "Parser/AST/Statements/For.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"

namespace ast {
For::For(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->declare = parser.parseStmt(tokens, true);
  this->logicalLine = this->declare->logicalLine;
  auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());

  if (sym == nullptr)
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Unterminated for loop initializer");
  if (sym->Sym != ';')
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         "unterminated for loop initializer");

  tokens.pop();

  this->expr = parser.parseExpr(tokens);

  sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym == nullptr)
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Unterminated for loop condition");
  if (sym->Sym != ';')
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
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
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unopened for loop body");
    }
  } else
    this->Run = parser.parseStmt(tokens, true);
}

gen::GenerationResult const For::generate(gen::CodeGenerator &generator) {
  asmc::File OutputFile = asmc::File();
  gen::scope::ScopeManager::getInstance()->pushScope(true);

  asmc::Label *label1 = new asmc::Label();
  label1->logicalLine = this->logicalLine;
  label1->label = ".L" + generator.nameTable.head->data.ident.ident +
                  std::to_string(generator.labelCount);
  generator.labelCount++;

  asmc::Label *label2 = new asmc::Label();
  label2->logicalLine = this->logicalLine;
  label2->label = ".L" + generator.nameTable.head->data.ident.ident +
                  std::to_string(generator.labelCount);
  generator.labelCount++;

  asmc::Label *breakLabel = new asmc::Label();
  breakLabel->logicalLine = this->logicalLine;
  breakLabel->label = ".L" + generator.nameTable.head->data.ident.ident +
                      std::to_string(generator.labelCount);
  generator.labelCount++;

  asmc::Label *continueLabel = new asmc::Label();
  continueLabel->logicalLine = this->logicalLine;
  continueLabel->label = ".L" + generator.nameTable.head->data.ident.ident +
                         std::to_string(generator.labelCount);
  generator.labelCount++;
  OutputFile << generator.GenSTMT(this->declare);
  asmc::Jmp *jmp = new asmc::Jmp();
  jmp->to = label2->label;
  OutputFile.text << jmp;

  OutputFile.text << label1;

  generator.breakContext.push(breakLabel->label);
  generator.continueContext.push(continueLabel->label);

  gen::scope::ScopeManager::getInstance()->pushScope(true);
  OutputFile << generator.GenSTMT(this->Run);
  OutputFile.text << continueLabel;
  OutputFile << generator.GenSTMT(this->increment);
  gen::scope::ScopeManager::getInstance()->popScope(&generator, OutputFile);
  OutputFile.text << label2;
  generator.breakContext.pop();
  generator.continueContext.pop();

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

  mov->to = generator.registers["%eax"]->get(mov->size);

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
  gen::scope::ScopeManager::getInstance()->popScope(&generator, OutputFile);
  generator.currentFunction->has_return = false;
  return {OutputFile, std::nullopt};
}

}  // namespace ast
