#include "Parser/AST/Statements/Delete.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Scope/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Parser.hpp"

namespace ast {
Delete::Delete(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->logicalLine = lex::tokenLine(tokens.peek());
  auto ident = dynamic_cast<lex::LObj *>(tokens.pop());
  if (ident == nullptr)
    throw err::Exception(
        "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
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

gen::GenerationResult const Delete::generate(gen::CodeGenerator &generator) {
  asmc::File OutputFile;
  auto resolved = generator.resolveSymbol(
      this->ident, this->modList, OutputFile, links::LinkedList<ast::Expr *>());
  if (!std::get<2>(resolved))
    generator.alert("Identifier " + this->ident + " not found to delete");

  gen::Symbol *resolvedSym = &std::get<1>(resolved);
  gen::Symbol *sym = std::get<4>(resolved);
  if (sym == nullptr)
    sym = resolvedSym;
  if (sym->sold != -1)
    generator.alert("Variable " + this->ident + " was sold on line " +
                    std::to_string(sym->sold) + " and cannot be deleted");

  ast::Function *af_free = generator.nameTable()["af_free"];
  if (af_free == nullptr)
    generator.alert(
        "Please import std library in order to use delete operator.\n\n -> "
        ".needs <std> \n\n");

  gen::Type **type = generator.typeList()[sym->type.typeName];
  if (type != nullptr) {
    gen::Class *classType = dynamic_cast<gen::Class *>(*type);
    if (classType != nullptr) {
      // check if the class has a destructor
      ast::Function *destructor = classType->nameTable["del"];

      if (destructor != nullptr) {
        ast::Call *call = new ast::Call();
        call->ident = this->ident;
        call->modList = this->modList;
        call->modList.push("del");
        call->Args = LinkedList<ast::Expr *>();

        OutputFile << generator.GenSTMT(call);
      };
    }
  };
  // A loan may refer to inline storage (for example, an element inside a
  // vector buffer). Destroy its fields, but only release the allocation when
  // this symbol actually owns it.
  if (sym->owned && !sym->type.isLoan) {
    ast::Var *var = new ast::Var();
    var->logicalLine = this->logicalLine;
    var->Ident = this->ident;
    var->modList = LinkedList<std::string>();

    ast::Call *freeCall = new ast::Call();
    freeCall->logicalLine = this->logicalLine;
    freeCall->ident = "af_free";
    freeCall->modList = LinkedList<std::string>();
    freeCall->Args = LinkedList<ast::Expr *>();
    freeCall->Args.push(var);
    OutputFile << generator.GenSTMT(freeCall);
  }
  OutputFile << std::get<3>(resolved);
  // Explicit deletion consumes the owner. Prevent scope cleanup from freeing
  // it again and reject any later use through the normal sold-value checks.
  sym->sold = this->logicalLine;
  return {OutputFile, std::nullopt};
}

} // namespace ast
