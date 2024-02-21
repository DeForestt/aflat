#include "Parser/AST/Statements/Delete.hpp"

#include "Parser/Parser.hpp"

namespace ast {
Delete::Delete(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->logicalLine = tokens.peek()->lineCount;
  auto ident = dynamic_cast<lex::LObj *>(tokens.pop());
  if (ident == nullptr)
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Expected Ident");

  this->ident = ident->meta;
  links::LinkedList<std::string> modList;
  auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  while (sym != nullptr && sym->Sym == '.') {
    tokens.pop();
    if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
      auto mod = *dynamic_cast<lex::LObj *>(tokens.pop());
      modList << mod.meta;
    } else
      throw err::Exception("Expected, Ident after dot. on line " +
                           std::to_string(sym->lineCount));
    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
      sym = dynamic_cast<lex::OpSym *>(tokens.peek());
    } else
      throw err::Exception("expected assignment operator got on line " +
                           std::to_string(sym->lineCount) + " " + sym->Sym);
  }

  this->modList = modList;
}
}  // namespace ast
