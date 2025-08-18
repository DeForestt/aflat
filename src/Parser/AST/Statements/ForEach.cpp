#include "Parser/AST/Statements/ForEach.hpp"

#include <unistd.h>

#include <optional>
#include <string>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Exceptions.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements/DecAssign.hpp"
#include "Scanner.hpp"

namespace ast {
ForEach::ForEach(links::LinkedList<lex::Token *> &tokens,
                 parse::Parser &parser) {
  auto identifier = dynamic_cast<lex::LObj *>(tokens.pop());
  if (identifier == nullptr) {
    throw err::Exception("Expected an identifier for the forEach loop");
  }

  this->binding_identifier = identifier->meta;

  auto in = dynamic_cast<lex::LObj *>(tokens.pop());
  if (in == nullptr || in->meta != "in") {
    throw err::Exception("Expected 'in' keyword in forEach loop");
  }
  this->iterator = parser.parseExpr(tokens);
  if (this->iterator == nullptr) {
    throw err::Exception("Expected an expression for the iterator in forEach");
  }

  auto openCurly = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (openCurly && openCurly->Sym == '{') {
    tokens.pop(); // Remove the '{'
    this->implementation = parser.parseStmt(tokens);
  } else {
    this->implementation = parser.parseStmt(tokens, true);
  }
}

gen::GenerationResult const ForEach::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  gen::scope::ScopeManager::getInstance()->pushScope(true);
  generator.logicalLine = this->logicalLine;

  auto decl = new ast::DecAssign();
  decl->logicalLine = this->logicalLine;
  decl->declare = new ast::Declare();
  decl->declare->logicalLine = this->logicalLine;
  decl->declare->ident = "_foreach_" + std::to_string(generator.tempCount++);
  decl->declare->type.typeName = "let";
  decl->mute = false;
  decl->declare->mut = false;
  decl->expr = this->iterator;

  file << decl->generate(generator).file;

  // create the while loop

  auto whileLoop = new ast::While();
  whileLoop->logicalLine = this->logicalLine;
  auto trueVar = new ast::Var();
  trueVar->logicalLine = this->logicalLine;
  trueVar->Ident = "true";
  whileLoop->expr = trueVar;

  // We need a match statement to make up the body of the while loop

  // Create the call for the nextDecl
  auto nextCall = new ast::Call();
  nextCall->logicalLine = this->logicalLine;
  nextCall->ident = decl->declare->ident;
  nextCall->modList << "next";

  auto nextCallExpr = new ast::CallExpr();
  nextCallExpr->logicalLine = this->logicalLine;
  nextCallExpr->call = nextCall;

  // Create the match Statement
  auto match = new ast::Match();
  match->logicalLine = this->logicalLine;
  match->expr = nextCallExpr;

  auto somePattern = ast::Match::Pattern("Some", this->binding_identifier);
  auto someCase = ast::Match::Case(somePattern, this->implementation);

  match->cases.push_back(someCase);
  auto nonePattern = ast::Match::Pattern("None", std::nullopt);
  auto noneCase = ast::Match::Case(nonePattern, new ast::Break());
  match->cases.push_back(noneCase);

  whileLoop->stmt = match;
  file << whileLoop->generate(generator).file;

  gen::scope::ScopeManager::getInstance()->popScope(&generator, file);

  return {
      .file = file,
      .expr = std::nullopt,
  };
}

} // namespace ast
