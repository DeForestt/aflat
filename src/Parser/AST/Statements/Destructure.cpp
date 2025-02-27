#include "Parser/AST/Statements/Destructure.hpp"

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/Parser.hpp"

namespace ast {
Destructure::Destructure(const bool mute,
                         links::LinkedList<lex::Token *> &tokens,
                         parse::Parser &parser)
    : mute(mute) {
  auto identifier = dynamic_cast<lex::LObj *>(tokens.pop());
  if (!identifier)
    throw err::Exception("On line " + std::to_string(tokens.peek()->lineCount) +
                         " need at least one identifier for destructuring");

  while (identifier) {
    identifiers.push_back(identifier->meta);
    auto symbol = dynamic_cast<lex::OpSym *>(tokens.pop());
    if (!symbol)
      throw err::Exception(
          "On line " + std::to_string(tokens.peek()->lineCount) +
          " expected ',' or '}' after identifier in destructuring");
    if (symbol->Sym == '}') break;
    if (symbol->Sym != ',')
      throw err::Exception(
          "On line " + std::to_string(tokens.peek()->lineCount) +
          " expected ',' or '}' after identifier in destructuring");
    identifier = dynamic_cast<lex::LObj *>(tokens.pop());
    if (!identifier)
      throw err::Exception("On line " +
                           std::to_string(tokens.peek()->lineCount) +
                           " expected identifier after ',' in destructuring");
  }

  auto eq = dynamic_cast<lex::OpSym *>(tokens.pop());
  if (!eq || eq->Sym != '=')
    throw err::Exception("On line " + std::to_string(tokens.peek()->lineCount) +
                         " expected '=' after identifiers in destructuring");

  this->expr = parser.parseExpr(tokens);
}

gen::GenerationResult const Destructure::generate(
    gen::CodeGenerator &generator) {
  asmc::File file;
  const auto temp_uuid = boost::uuids::random_generator()();
  const auto temp_ident = "**temp_" + boost::uuids::to_string(temp_uuid);

  auto letType = ast::Type("let", asmc::QWord);
  auto decl = new ast::Declare(temp_ident, ast::ScopeMod::Public, "let", false,
                               letType, "", links::LinkedList<std::string>());
  auto decAssign = new ast::DecAssign();
  decAssign->declare = decl;
  decAssign->expr = expr;
  decAssign->mute = false;
  decAssign->logicalLine = this->logicalLine;
  file << decAssign->generate(generator).file;
  for (auto &ident : identifiers) {
    const auto var = new ast::Var();
    var->Ident = temp_ident;
    var->clean = true;
    var->modList.push(ident);

    asmc::File dump = asmc::File();
    auto resolved = generator.resolveSymbol(temp_ident, var->modList, dump,
                                            links::LinkedList<ast::Expr *>());
    if (!std::get<2>(resolved)) {
      generator.alert(ident + " does not exist on the type");
    }
    auto symbol = &std::get<1>(resolved);
    const auto mutable_ =
        this->mute && (symbol->mutable_ || !symbol->type.isReference);
    auto decl = new ast::Declare(
        ident, ast::ScopeMod::Public, symbol->type.typeName, symbol->mutable_,
        symbol->type, "", links::LinkedList<std::string>());
    auto decAssign = new ast::DecAssign();
    if (mutable_ && symbol->mutable_) {
      gen::scope::ScopeManager::getInstance()->addAssign(symbol->symbol, false);
    }
    decAssign->declare = decl;
    decAssign->mute = mutable_;
    decAssign->expr = var;
    decAssign->logicalLine = this->logicalLine;
    decAssign->declare->mask = true;
    file << decAssign->generate(generator).file;
  }
  return {file, std::nullopt};
}
}  // namespace ast
