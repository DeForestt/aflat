#include "Parser/AST/Statements/Transform.hpp"

#include <regex>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Lower.hpp"
#include "PreProcessor.hpp"
#include "Utils.hpp"

namespace ast {
Transform::Transform(links::LinkedList<lex::Token *> &tokens) {
  auto identToken = dynamic_cast<lex::LObj *>(tokens.pop());
  if (identToken == nullptr) {
    throw err::Exception("Expected  identifier in transform statment");
  }
  ident = identToken->meta;

  lex::StringObj *templateToken = dynamic_cast<lex::StringObj *>(tokens.pop());
  if (templateToken == nullptr) {
    throw err::Exception("Expected string literal in transform statment");
  }
  _template = templateToken->value;
  auto semi = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (semi == nullptr || semi->Sym != ';') {
    throw err::Exception("Expected ; in transform statment");
  }
}

gen::GenerationResult const Transform::generate(gen::CodeGenerator &generator) {
  generator.transforms[ident] = *this;
  return gen::GenerationResult();
}

ast::Statement *Transform::parse(const std::string &ident, std::string &type,
                                 std::string &expr,
                                 gen::CodeGenerator &generator) {
  std::string result = _template;

  // replace all instances of ${ident} and ${type} and ${expr} with the
  // corresponding dvalues

  result = std::regex_replace(result, std::regex("\\$\\{ident\\}"), ident);
  result = std::regex_replace(result, std::regex("\\$\\{type\\}"), type);
  result = std::regex_replace(result, std::regex("\\$\\{expr\\}"), expr);

  // replace Self with scope
  if (generator.scope != nullptr)
    result = std::regex_replace(result, std::regex("\\$\\{Self\\}"),
                                generator.scope->Ident);

  // replace \" with "
  result = std::regex_replace(result, std::regex("\\\\\""), "\"");

  lex::Lexer lexer = lex::Lexer();
  lex::Lexer l = lex::Lexer();
  PreProcessor pp = PreProcessor();

  auto tokens = l.Scan(pp.PreProcess(result, gen::utils::getLibPath("head")));
  tokens.invert();
  // parse the file
  ast::Statement *statement = generator.parser.parseStmt(tokens);
  auto Lowerer = parse::lower::Lowerer(statement, true);

  return statement;
}
std::string Compound::toString() {
  return expr1->toString() + " " + utils::op_to_string(op) + " " +
         expr2->toString();
}

};  // namespace ast