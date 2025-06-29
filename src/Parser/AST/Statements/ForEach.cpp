#include "Parser/AST/Statements/ForEach.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"

namespace ast {
ForEach::ForEach(links::LinkedList<lex::Token *> &tokens,
                 parse::Parser &parser) {
  this->lambda = new Lambda();
  this->lambda->function = new Function();
  this->lambda->function->args = parser.parseArgs(
      tokens, ',', ':', this->lambda->function->argTypes,
      this->lambda->function->req, this->lambda->function->mutability,
      this->lambda->function->readOnly,
      this->lambda->function->optConvertionIndices, true);

  auto decscope = new Declare("*scope", ast::Public, "typeOf", false,
                              *parser.typeList["typeOf"], "state",
                              links::LinkedList<std::string>());
  auto sequence2 = new Sequence();
  sequence2->Statement1 = this->lambda->function->args;
  sequence2->Statement2 = decscope;
  this->lambda->function->args = sequence2;
  this->lambda->function->argTypes.push_back(decscope->type);
  this->lambda->function->req++;

  this->lambda->function->args->logicalLine = tokens.peek()->lineCount;

  this->iterator = parser.parseExpr(tokens);
  this->iterator->logicalLine = tokens.peek()->lineCount;

  auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym && sym->Sym == '{') {
    tokens.pop();
    this->lambda->function->statement = parser.parseStmt(tokens);
    this->lambda->function->statement->logicalLine = tokens.peek()->lineCount;
  } else {
    this->lambda->function->statement = parser.parseStmt(tokens, true);
    this->lambda->function->statement->logicalLine = tokens.peek()->lineCount;
  }
}

gen::GenerationResult const ForEach::generate(gen::CodeGenerator &generator) {
  if (generator.nameTable["_fEachOr"] == nullptr)
    generator.alert(
        "Please include the standard library to use the forEach function\n\n"
        ".needs <std> \n\n");

  auto dumbyGen = asmc::File();
  auto expr = generator.GenExpr(this->iterator, dumbyGen);
  ast::Type iteratorType = ast::Type("Iterator", asmc::QWord);
  generator.canAssign(iteratorType, expr.type,
                      "Can only iterate over {}, {} not allowed");
  auto call = new Call();
  // the __fEachOr should be defined in the standard library this is the
  // obviscated name for the forEach function
  auto type = ast::Type("typeOf", asmc::QWord);
  type.isGeneric = true;
  type.opType = asmc::Hard;

  auto state = new ast::Var();
  auto decAssign = new DecAssign();
  decAssign->declare = new Declare();
  state->Ident = "state";
  call->ident = "_fEachOr";
  call->Args.push(this->iterator);
  call->Args.push(this->lambda);
  call->Args.push(state);

  auto deconstruct = new Destructure();
  deconstruct->mute = true;
  deconstruct->logicalLine = this->logicalLine;
  auto var = new Var();
  var->Ident = "*scope";
  deconstruct->expr = var;
  auto inScope = gen::scope::ScopeManager::getInstance()->getScope(true);

  for (auto sym : inScope) {
    if (sym.underscores == 0) {
      deconstruct->identifiers.push_back(sym.symbol);
    }
  }

  auto sequence = new Sequence();
  sequence->Statement1 = deconstruct;
  sequence->Statement2 = this->lambda->function->statement;
  this->lambda->function->statement = sequence;

  return call->generate(generator);
}
}  // namespace ast
