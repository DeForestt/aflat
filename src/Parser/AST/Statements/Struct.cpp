#include "Parser/AST/Statements/Struct.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "Parser/Parser.hpp"

namespace ast {
Struct::Struct(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->logicalLine = tokens.peek()->lineCount;
  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto ident = *dynamic_cast<lex::LObj *>(tokens.pop());
    this->ident.ident = ident.meta;
  } else
    throw err::Exception("struct needs Ident", tokens.peek());
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto op = *dynamic_cast<lex::OpSym *>(tokens.pop());
    if (op.Sym != '{')
      throw err::Exception("Unopened UDeffType", tokens.peek());
  } else
    throw err::Exception("Unopened UDeffType", tokens.peek());
  this->statement = parser.parseStmt(tokens);
  parser.addType(this->ident.ident, asmc::Hard, asmc::QWord);
}

gen::GenerationResult const Struct::generate(gen::CodeGenerator &generator) {
  gen::Type *type = new gen::Type();
  bool saveScope = generator.globalScope;
  generator.globalScope = false;
  type->Ident = this->ident.ident;
  type->SymbolTable = generator.GenTable(this->statement, type->SymbolTable);
  generator.typeList.push(type);
  generator.globalScope = saveScope;
  return {asmc::File(), std::nullopt};
}
}  // namespace ast