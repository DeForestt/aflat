#include "Parser/AST/Statements/Match.hpp"

#include <optional>
#include <vector>

#include "CodeGenerator/CodeGenerator.hpp"

namespace ast {

Match::Pattern::Pattern(links::LinkedList<lex::Token *> &tokens) {
  auto name = dynamic_cast<lex::LObj *>(tokens.pop());
  if (name == nullptr) {
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Match pattern must have an alias name.");
  }
  aliasName = name->meta;
  std::optional<std::string> veriableName = std::nullopt;
  if (auto openParen = dynamic_cast<lex::OpSym *>(tokens.peek());
      openParen != nullptr && openParen->Sym == '(') {
    tokens.pop();
    auto veriableToken = dynamic_cast<lex::LObj *>(tokens.pop());
    if (veriableToken == nullptr) {
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Match pattern must have a variable name.");
    }
    veriableName = veriableToken->meta;
    auto closeParen = dynamic_cast<lex::OpSym *>(tokens.pop());
    if (closeParen == nullptr || closeParen->Sym != ')') {
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Match pattern must have a closing parenthesis.");
    }
  }
}

Match::Case::Case(links::LinkedList<lex::Token *> &tokens,
                  parse::Parser &parser) {
  pattern = Match::Pattern(tokens);
  auto token = tokens.pop();
  if (token == nullptr) {
    throw err::Exception("Unexpected end of tokens while parsing match case.");
  }
  const auto equalsSym = dynamic_cast<lex::OpSym *>(token);
  if (equalsSym == nullptr || equalsSym->Sym != '=') {
    throw err::Exception("Match case must have a `=>` symbol.");
  }
  const auto carrot = dynamic_cast<lex::Symbol *>(tokens.pop());
  if (carrot == nullptr || carrot->meta != ">") {
    throw err::Exception("Match case must have a `=>` symbol.");
  }

  auto openBrace = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (openBrace && openBrace->Sym == '{') {
    statement = parser.parseStmt(tokens);
  } else {
    statement = parser.parseStmt(tokens, true);
  }
}

std::vector<Match::Case> const Match::parseCases(
    links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  std::vector<Match::Case> cases;
  bool firstCase = true;

  do {
    if (!firstCase) {
      tokens.pop();
    } else {
      firstCase = false;
    }
    // if we have a close brace, there is a trailing comma (Which we want to
    // allow so we can break)
    auto closeBrace = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (closeBrace && closeBrace->Sym == '}') {
      break;
    }
    // Now we can parse the case
    cases.push_back(Match::Case(tokens, parser));
  } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
           dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
  return cases;
}

Match::Match(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  expr = parser.parseExpr(tokens);
  auto openBrace = dynamic_cast<lex::OpSym *>(tokens.pop());
  if (openBrace == nullptr || openBrace->Sym != '{') {
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Match statement must start with an opening brace.");
  }
  cases = parseCases(tokens, parser);
  auto closeBrace = dynamic_cast<lex::OpSym *>(tokens.pop());
  if (closeBrace == nullptr || closeBrace->Sym != '}') {
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Match statement must end with a closing brace.");
  }
}

gen::GenerationResult const Match::generate(gen::CodeGenerator &generator) {
  asmc::File file;

  auto exprResult = generator.GenExpr(expr, file);
  auto t = generator.getType(exprResult.type, file);

  if (!t) {
    generator.alert("Match expression type not found: " + exprResult.type, true,
                    __FILE__, __LINE__);
    return {.file = file, .expr = std::nullopt};
  }
}

}  // namespace ast
