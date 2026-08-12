#include "Parser/AST/Statements/ForEach.hpp"

#include <unistd.h>

#include <optional>
#include <string>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Exceptions.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements/Assign.hpp"
#include "Parser/AST/Statements/DecAssign.hpp"
#include "Parser/AST/Statements/For.hpp"
#include "Parser/AST/Statements/Sequence.hpp"
#include "Scanner.hpp"

namespace ast {
namespace {

Function *findIteratorNext(gen::Class *iteratorClass) {
  for (auto *current = iteratorClass; current != nullptr;
       current = current->parent) {
    if (auto *next = current->nameTable["next"])
      return next;
  }
  return nullptr;
}

} // namespace

ForEach::ForEach(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser,
                 bool cooperative)
    : cooperative(cooperative) {
  auto identifier = dynamic_cast<lex::LObj *>(tokens.pop());
  if (identifier == nullptr) {
    throw err::Exception("Expected an identifier for the forEach loop");
  }

  this->binding_identifier = identifier->meta;
  this->logicalLine = identifier->lineCount;

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
  generator.logicalLine() = this->logicalLine;

  auto decl = new ast::DecAssign();
  decl->logicalLine = this->logicalLine;
  decl->declare = new ast::Declare();
  decl->declare->logicalLine = this->logicalLine;
  decl->declare->ident = "_foreach_" + std::to_string(generator.tempCount()++);
  decl->declare->type.typeName = "let";
  decl->mute = false;
  decl->declare->mut = false;
  decl->expr = this->iterator;

  file << decl->generate(generator).file;

  Function *nextMethod = nullptr;
  std::string iteratorTypeName;
  auto *iteratorSymbol =
      gen::scope::ScopeManager::getInstance()->get(decl->declare->ident);
  if (iteratorSymbol != nullptr) {
    iteratorTypeName = iteratorSymbol->type.typeName;
    auto **iteratorType = generator.getType(iteratorTypeName, file);
    if (iteratorType != nullptr) {
      if (auto *iteratorClass = dynamic_cast<gen::Class *>(*iteratorType)) {
        nextMethod = findIteratorNext(iteratorClass);
      }
    }
  }

  const auto *currentFunction = generator.currentFunction();
  const bool inAsyncContext =
      currentFunction != nullptr &&
      (currentFunction->isAsync || currentFunction->isAsyncBody);
  const bool asyncIterator = nextMethod != nullptr && nextMethod->isAsync;
  if (asyncIterator && !this->cooperative && !inAsyncContext) {
    generator.alert("`foreach` cannot consume async iterator type `" +
                        iteratorTypeName +
                        "` from a synchronous function because its `next()` "
                        "method starts a "
                        "task. Write `async foreach " +
                        this->binding_identifier +
                        " in ...` to explicitly run each item task to "
                        "completion, or move the "
                        "loop into an async function where plain `foreach` "
                        "awaits each item.",
                    true, __FILE__, __LINE__);
  }
  if (this->cooperative && !asyncIterator) {
    if (nextMethod == nullptr) {
      generator.alert(
          "`async foreach` requires an iterator with `async fn next()`, but "
          "type `" +
              iteratorTypeName +
              "` does not define `next()`. Define `async fn next() -> "
              "option<T>` or use an iterator type that does.",
          true, __FILE__, __LINE__);
    }
    generator.alert("`async foreach` requires an async iterator, but type `" +
                        iteratorTypeName +
                        "` has a synchronous `next()` method. Remove `async` "
                        "and write `foreach " +
                        this->binding_identifier + " in ...` instead.",
                    true, __FILE__, __LINE__);
  }

  const std::string itemIdent = decl->declare->ident + "_item";
  auto makeNextCall = [&]() -> ast::Expr * {
    auto nextCall = new ast::Call();
    nextCall->logicalLine = this->logicalLine;
    nextCall->ident = decl->declare->ident;
    nextCall->modList << "next";
    auto nextCallExpr = new ast::CallExpr();
    nextCallExpr->logicalLine = this->logicalLine;
    nextCallExpr->call = nextCall;
    if (!asyncIterator)
      return nextCallExpr;

    auto cooperativeNext = new ast::AsyncExpression();
    cooperativeNext->logicalLine = this->logicalLine;
    cooperativeNext->kind = inAsyncContext ? ast::AsyncExpressionKind::Await
                                           : ast::AsyncExpressionKind::Run;
    cooperativeNext->expr = nextCallExpr;
    return cooperativeNext;
  };

  auto itemDecl = new ast::DecAssign();
  itemDecl->logicalLine = this->logicalLine;
  itemDecl->declare = new ast::Declare();
  itemDecl->declare->logicalLine = this->logicalLine;
  itemDecl->declare->ident = itemIdent;
  itemDecl->declare->type.typeName = "let";
  itemDecl->declare->mut = true;
  itemDecl->mute = true;
  itemDecl->expr = makeNextCall();

  auto forLoop = new ast::For();
  forLoop->logicalLine = this->logicalLine;
  forLoop->declare = itemDecl;

  auto isSomeCall = new ast::CallExpr();
  isSomeCall->logicalLine = this->logicalLine;
  isSomeCall->call = new ast::Call();
  isSomeCall->call->logicalLine = this->logicalLine;
  isSomeCall->call->ident = itemIdent;
  isSomeCall->call->modList << "isSome";
  forLoop->expr = isSomeCall;

  auto updateNext = new ast::Assign();
  updateNext->logicalLine = this->logicalLine;
  updateNext->Ident = itemIdent;
  updateNext->expr = makeNextCall();
  forLoop->increment = updateNext;

  auto someBinding = new ast::DecAssign();
  someBinding->logicalLine = this->logicalLine;
  someBinding->declare = new ast::Declare();
  someBinding->declare->logicalLine = this->logicalLine;
  someBinding->declare->ident = this->binding_identifier;
  someBinding->declare->type.typeName = "let";
  someBinding->declare->mut = false;
  someBinding->mute = false;
  auto expectCall = new ast::CallExpr();
  expectCall->logicalLine = this->logicalLine;
  expectCall->call = new ast::Call();
  expectCall->call->logicalLine = this->logicalLine;
  expectCall->call->ident = itemIdent;
  expectCall->call->modList << "expect";
  auto expectMsg = new ast::StringLiteral();
  expectMsg->val = "Failed to get next value in foreach";
  expectCall->call->Args.push(expectMsg);
  someBinding->expr = expectCall;

  auto someBody = new ast::Sequence();
  someBody->logicalLine = this->logicalLine;
  someBody->Statement1 = someBinding;
  someBody->Statement2 = this->implementation;

  forLoop->Run = someBody;
  file << forLoop->generate(generator).file;

  gen::scope::ScopeManager::getInstance()->popScope(&generator, file);

  return {
      .file = file,
      .expr = std::nullopt,
  };
}

} // namespace ast
