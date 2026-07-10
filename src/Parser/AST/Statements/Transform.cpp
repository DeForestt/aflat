#include "Parser/AST/Statements/Transform.hpp"

#include <cctype>
#include <regex>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"
#include "ErrorReporter.hpp"
#include "Parser/AST/Statements/Sequence.hpp"
#include "Parser/Lower.hpp"
#include "PreProcessor.hpp"
#include "Utils.hpp"

namespace ast {
namespace {

std::string transformTypeIdent(const std::string &type) {
  auto genericStart = type.find('<');
  std::string ident =
      genericStart == std::string::npos ? type : type.substr(0, genericStart);

  std::string result;
  for (char c : ident) {
    if (std::isalnum(static_cast<unsigned char>(c)) || c == '_') {
      result += c;
    }
  }
  return result.empty() ? ident : result;
}

void attachSourceLocation(ast::Statement *statement,
                          const ast::SourceLocation &location) {
  if (statement == nullptr)
    return;
  statement->sourceLocation = location;
  if (auto *sequence = dynamic_cast<ast::Sequence *>(statement)) {
    attachSourceLocation(sequence->Statement1, location);
    attachSourceLocation(sequence->Statement2, location);
  }
}

} // namespace

Transform::Transform(links::LinkedList<lex::Token *> &tokens) {
  auto identToken = dynamic_cast<lex::LObj *>(tokens.pop());
  if (identToken == nullptr) {
    throw err::Exception("Expected  identifier in transform statment");
  }
  ident = identToken->meta;
  logicalLine = identToken->lineCount;

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
  generator.transforms()[ident] = *this;
  return gen::GenerationResult();
}

ast::Statement *
Transform::parse(const std::string &ident, std::string &type, std::string &expr,
                 std::string &scope, std::string &mut,
                 std::unordered_map<std::string, std::string> &args,
                 gen::CodeGenerator &generator) {
  std::string result = _template;

  // replace all instances of ${ident} and ${type} and ${expr} with the
  // corresponding dvalues

  result = std::regex_replace(result, std::regex("\\$\\{ident\\}"), ident);
  result = std::regex_replace(result, std::regex("\\$\\{type_ident\\}"),
                              transformTypeIdent(type));
  result = std::regex_replace(result, std::regex("\\$\\{type\\}"), type);
  result = std::regex_replace(result, std::regex("\\$\\{expr\\}"), expr);
  result = std::regex_replace(result, std::regex("\\$\\{scope\\}"), scope);
  result = std::regex_replace(result, std::regex("\\$\\{mut\\}"), mut);

  // replace all instances of ${arg} with the corresponding value
  for (auto &arg : args) {
    result = std::regex_replace(
        result, std::regex("\\$\\{" + arg.first + "\\}"), arg.second);
  }

  // replace Self with scope
  if (generator.scope() != nullptr)
    result = std::regex_replace(result, std::regex("\\$\\{Self\\}"),
                                generator.scope()->Ident);

  // replace \" with "
  result = std::regex_replace(result, std::regex("\\\\\""), "\"");

  // check for any remaining ${} and alert the name of the missing arg
  std::smatch match;
  if (std::regex_search(result, match, std::regex("\\$\\{[a-zA-Z0-9_]+\\}"))) {
    generator.alert("Transform " + this->ident + " missing argument " +
                    match.str());
  }

  ast::SourceLocation location;
  const std::string originFile = this->definitionFile.empty()
                                     ? generator.diagnosticFile()
                                     : this->definitionFile;
  const std::string originSource = this->definitionSource.empty()
                                       ? generator.source()
                                       : this->definitionSource;
  location.file = originFile + " (generated transform " + this->ident + ")";
  location.source = result;
  location.generatedFromFile = originFile;
  location.generatedFromSource = originSource;
  location.generatedFromLine = this->logicalLine;
  location.description = "transform `" + this->ident + "`";
  if (!type.empty())
    location.description += " for type `" + type + "`";
  if (!expr.empty())
    location.description += " from `" + expr + "`";

  try {
    lex::Lexer l = lex::Lexer();
    PreProcessor pp = PreProcessor();

    auto tokens = l.Scan(pp.PreProcess(result, gen::utils::getLibPath("head")));
    tokens.invert();
    // parse the file
    ast::Statement *statement = generator.parser().parseStmt(tokens);
    auto Lowerer = parse::lower::Lowerer(statement, true);

    auto *lowered = Lowerer.result();
    attachSourceLocation(lowered, location);
    return lowered;
  } catch (err::Exception &e) {
    int line = error::extractLine(e.errorMsg);
    error::report(location.file, line, e.errorMsg, location.source);
    if (!location.generatedFromFile.empty() && location.generatedFromLine > 0) {
      std::cout << "generated from " << location.description << ":\n";
      error::report(location.generatedFromFile, location.generatedFromLine,
                    "generation site", location.generatedFromSource);
    }
    throw err::Exception("Line: " + std::to_string(this->logicalLine) + " " +
                         location.description + " failed: " + e.errorMsg);
  }
}
std::string Compound::toString() {
  return expr1->toString() + " " + utils::op_to_string(op) + " " +
         expr2->toString();
}

}; // namespace ast
