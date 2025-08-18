#include "Parser/AST/Statements/Dec.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"

namespace ast {
Dec::Dec(const std::string &ident, links::LinkedList<lex::Token *> &tokens)
    : ident(ident) {
  if (tokens.count < 1) {
    throw err::Exception("EOF while parsing increment statement");
  }
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr ||
      dynamic_cast<lex::OpSym *>(tokens.peek())->Sym != '-') {
    throw err::Exception(
        "Expected another '-' in the increment statment on line " +
        std::to_string(tokens.peek()->lineCount));
  }
  this->logicalLine = tokens.pop()->lineCount;
}

gen::GenerationResult const Dec::generate(gen::CodeGenerator &generator) {
  asmc::File OutputFile = asmc::File();
  gen::Symbol *sym = gen::scope::ScopeManager::getInstance()->get(this->ident);
  if (sym == nullptr)
    generator.alert("Identifier not found to decrement");
  generator.canAssign(
      sym->type, "int",
      "type {} does not support decrementing please us int instead");

  asmc::Sub *sub = new asmc::Sub();
  sub->logicalLine = sub->logicalLine;
  sub->op1 = "$-1";
  sub->op2 = "-" + std::to_string(sym->byteMod) + "(%rbp)";
  OutputFile.text << sub;
  gen::scope::ScopeManager::getInstance()->addAssign(this->ident);
  return {OutputFile, std::nullopt};
}
} // namespace ast