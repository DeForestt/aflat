#include "Parser/AST/Statements/Inc.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"

namespace ast {
Inc::Inc(const std::string &ident, links::LinkedList<lex::Token *> &tokens)
    : ident(ident) {
  if (tokens.count < 1) {
    throw err::Exception("EOF while parsing increment statement");
  }
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr ||
      dynamic_cast<lex::OpSym *>(tokens.peek())->Sym != '+') {
    throw err::Exception(
        "Expected another + in the increment statment on line " +
        std::to_string(tokens.peek()->lineCount));
  }
  this->logicalLine = tokens.pop()->lineCount;
}

gen::GenerationResult const Inc::generate(gen::CodeGenerator &generator) {
  asmc::File OutputFile = asmc::File();
  gen::Symbol *sym = gen::scope::ScopeManager::getInstance()->get(this->ident);
  if (sym == nullptr) generator.alert("Identifier not found to increment");
  generator.canAssign(
      sym->type, "int",
      "type {} does not support incrementing please us int instead");

  asmc::Add *add = new asmc::Add();
  add->logicalLine = this->logicalLine;
  add->op1 = "$1";
  add->op2 = "-" + std::to_string(sym->byteMod) + "(%rbp)";
  OutputFile.text << add;
  gen::scope::ScopeManager::getInstance()->addAssign(this->ident);
  return {OutputFile, std::nullopt};
}

}  // namespace ast