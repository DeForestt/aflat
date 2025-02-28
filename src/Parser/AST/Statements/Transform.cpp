#include "Parser/AST/Statements/Transform.hpp"

#include <regex>

#include "CodeGenerator/CodeGenerator.hpp"

namespace ast {
Transform::Transform(links::LinkedList<lex::Token *> &tokens) {
  auto identToken = dynamic_cast<lex::LObj *>(tokens.pop());
  if (identToken == nullptr) {
    throw new err::Exception("Expected  identifier in transform statment");
  }
  ident = identToken->meta;

  lex::StringObj *templateToken = dynamic_cast<lex::StringObj *>(tokens.pop());
  if (templateToken == nullptr) {
    throw new err::Exception("Expected string literal in transform statment");
  }
  _template = templateToken->value;
}

gen::GenerationResult const Transform::generate(gen::CodeGenerator &generator) {
  generator.transforms[ident] = *this;
  return gen::GenerationResult();
}

ast::Statement *Transform::parse(const std::string &ident, std::string &type,
                                 std::string &expr, parse::Parser &parser) {
  std::string result = _template;

  // replace all instances of ${ident} and ${type} and ${expr} with the
  // corresponding dvalues

  result = std::regex_replace(result, std::regex("\\$\\{ident\\}"), ident);
  result = std::regex_replace(result, std::regex("\\$\\{type\\}"), type);
  result = std::regex_replace(result, std::regex("\\$\\{expr\\}"), expr);

  lex::Lexer lexer = lex::Lexer();
  auto tokens = lexer.Scan(result);
  auto snippet = parser.parseStmt(tokens);
  return snippet;
}
};  // namespace ast