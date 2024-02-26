#include "Parser/AST/Statements/While.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/Parser.hpp"

namespace ast {
/*
 * @brief This will parse a while statement
 * @param tokens The tokens to parse
 * @param parser The parser
 */
While::While(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->expr = parser.parseExpr(tokens);
  this->logicalLine = this->expr->logicalLine;
  auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym != nullptr) {
    if (sym->Sym == '{') {
      tokens.pop();
      this->stmt = parser.parseStmt(tokens);
    } else
      throw err::Exception("Line: " + std::to_string(sym->lineCount) +
                           " un - opened while loop, expected '{' got " +
                           sym->Sym + " instead");
  } else
    this->stmt = parser.parseStmt(tokens, true);
}

gen::GenerationResult const While::generate(gen::CodeGenerator &generator) {
  asmc::File file;
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

  asmc::Jmp *jmp = new asmc::Jmp();
  jmp->logicalLine = this->logicalLine;
  jmp->to = label2->label;
  file.text << jmp;

  file.text << label1;
  generator.breakContext.push(breakLabel->label);
  generator.continueContext.push(label2->label);
  file << generator.GenSTMT(this->stmt);
  generator.breakContext.pop();
  generator.continueContext.pop();
  gen::scope::ScopeManager::getInstance()->popScope(&generator, file);

  file.text << label2;

  gen::Expr expr = generator.GenExpr(this->expr, file);

  ast::Type t = ast::Type();
  t.typeName = "bool";
  t.size = asmc::Byte;
  generator.canAssign(t, expr.type,
                      "while conditions must be of type {}, {} was given.");

  asmc::Mov *mov = new asmc::Mov();
  mov->logicalLine = this->logicalLine;

  mov->size = expr.size;

  mov->from = expr.access;

  mov->to = generator.registers["%eax"]->get(mov->size);

  asmc::Cmp *cmp = new asmc::Cmp();
  cmp->logicalLine = this->logicalLine;
  asmc::Je *je = new asmc::Je();
  je->logicalLine = this->logicalLine;

  cmp->from = "$1";
  cmp->to = mov->to;
  cmp->size = expr.size;
  je->to = label1->label;

  file.text << mov;
  file.text << cmp;
  file.text << je;
  file.text << breakLabel;
  generator.currentFunction->has_return = false;
  return {file, std::nullopt};
}
}  // namespace ast