#include "Scanner.hpp"

#include <sstream>
#include <string>
#include <utility>

#include "Exceptions.hpp"
#include "Scan/Scan.hpp"
#include "Scan/Token.hpp"

namespace {

using ScanSymbol = aflat::scan::token::Symbol;

int adjustLine(int startLine, std::size_t line) {
  const int offset = startLine - 1;
  return static_cast<int>(line) + offset;
}

std::string sliceLexeme(const std::string &source,
                        const aflat::scan::token::Token &token) {
  const auto start = token.range.start.byte_offset == 0
                         ? 0
                         : token.range.start.byte_offset - 1;
  const auto end = token.range.end.byte_offset;
  if (end < start)
    return {};
  const auto length = end - start;
  return source.substr(start, length);
}

void pushOpSym(links::LinkedList<lex::Token *> &tokens, char sym, int line) {
  auto *node = new lex::OpSym();
  node->Sym = sym;
  node->lineCount = line;
  tokens.push(node);
}

void pushSymbol(links::LinkedList<lex::Token *> &tokens, std::string meta,
                int line) {
  auto *node = new lex::Symbol();
  node->meta = std::move(meta);
  node->lineCount = line;
  tokens.push(node);
}

void pushRef(links::LinkedList<lex::Token *> &tokens, int line) {
  auto *node = new lex::Ref();
  node->lineCount = line;
  tokens.push(node);
}

} // namespace

LinkedList<lex::Token *> lex::Lexer::Scan(string input, int startLine) {
  LinkedList<lex::Token *> tokens;
  std::istringstream stream(input);
  aflat::scan::Scanner scanner(stream, 1);

  int lastLine = startLine;
  while (true) {
    auto result = scanner.next();
    if (!result) {
      auto cursor = scanner.cursor();
      const int line = adjustLine(startLine, cursor.line);
      throw err::Exception("Invalid token on line " + std::to_string(line) +
                           ": " + result.error().message());
    }

    const auto token = result.value();

    if (aflat::scan::token::isError(token)) {
      const auto message = aflat::scan::token::asError(token).value();
      throw err::Exception("Invalid token: " + std::string(message));
    }

    if (aflat::scan::token::isEof(token)) {
      lastLine = adjustLine(startLine, token.range.start.line);
      break;
    }

    const int line = adjustLine(startLine, token.range.start.line);
    lastLine = line;

    aflat::scan::token::visit(
        token, aflat::scan::token::overloaded{
                   [&](const aflat::scan::token::Identifier &ident) {
                     auto *node = new lex::LObj();
                     node->meta = ident.name;
                     node->lineCount = line;
                     tokens.push(node);
                   },
                   [&](const aflat::scan::token::Keyword &kw) {
                     auto *node = new lex::LObj();
                     node->meta = std::string(
                         aflat::scan::token::keyword_to_string(kw.type));
                     node->lineCount = line;
                     tokens.push(node);
                   },
                   [&](const aflat::scan::token::IntegerLiteral &) {
                     auto *node = new lex::INT();
                     node->value = sliceLexeme(input, token);
                     node->lineCount = line;
                     tokens.push(node);
                   },
                   [&](const aflat::scan::token::FloatLiteral &) {
                     auto *node = new lex::FloatLit();
                     node->value = sliceLexeme(input, token);
                     node->lineCount = line;
                     tokens.push(node);
                   },
                   [&](const aflat::scan::token::LongLiteral &) {
                     auto *node = new lex::Long();
                     node->value = sliceLexeme(input, token);
                     node->lineCount = line;
                     tokens.push(node);
                   },
                   [&](const aflat::scan::token::StringLiteral &stringVal) {
                     auto *node = new lex::StringObj();
                     node->value = stringVal.value;
                     node->lineCount = line;
                     tokens.push(node);
                   },
                   [&](const aflat::scan::token::TemplateString &templ) {
                     auto *node = new lex::FStringObj();
                     node->value = templ.value;
                     node->lineCount = line;
                     tokens.push(node);
                   },
                   [&](const aflat::scan::token::CharLiteral &charVal) {
                     auto *node = new lex::CharObj();
                     node->value = charVal.value;
                     node->lineCount = line;
                     tokens.push(node);
                   },
                   [&](const aflat::scan::token::Symbol &sym) {
                     switch (sym.type) {
                     case ScanSymbol::Type::Plus:
                       pushOpSym(tokens, '+', line);
                       break;
                     case ScanSymbol::Type::Minus:
                       pushOpSym(tokens, '-', line);
                       break;
                     case ScanSymbol::Type::Asterisk:
                       pushOpSym(tokens, '*', line);
                       break;
                     case ScanSymbol::Type::Slash:
                       pushOpSym(tokens, '/', line);
                       break;
                     case ScanSymbol::Type::Percent:
                       pushOpSym(tokens, '%', line);
                       break;
                     case ScanSymbol::Type::Equal:
                       pushOpSym(tokens, '=', line);
                       break;
                     case ScanSymbol::Type::DoubleEqual:
                       pushSymbol(tokens, "==", line);
                       break;
                     case ScanSymbol::Type::NotEqual:
                       pushSymbol(tokens, "!=", line);
                       break;
                     case ScanSymbol::Type::Less:
                       pushSymbol(tokens, "<", line);
                       break;
                     case ScanSymbol::Type::LessEqual:
                       pushSymbol(tokens, "<=", line);
                       break;
                     case ScanSymbol::Type::Greater:
                       pushSymbol(tokens, ">", line);
                       break;
                     case ScanSymbol::Type::GreaterEqual:
                       pushSymbol(tokens, ">=", line);
                       break;
                     case ScanSymbol::Type::And:
                       pushOpSym(tokens, '&', line);
                       pushOpSym(tokens, '&', line);
                       break;
                     case ScanSymbol::Type::Or:
                       pushSymbol(tokens, "||", line);
                       break;
                     case ScanSymbol::Type::Not:
                       pushOpSym(tokens, '!', line);
                       break;
                     case ScanSymbol::Type::LeftParen:
                       pushOpSym(tokens, '(', line);
                       break;
                     case ScanSymbol::Type::RightParen:
                       pushOpSym(tokens, ')', line);
                       break;
                     case ScanSymbol::Type::LeftBrace:
                       pushOpSym(tokens, '{', line);
                       break;
                     case ScanSymbol::Type::RightBrace:
                       pushOpSym(tokens, '}', line);
                       break;
                     case ScanSymbol::Type::LeftBracket:
                       pushOpSym(tokens, '[', line);
                       break;
                     case ScanSymbol::Type::RightBracket:
                       pushOpSym(tokens, ']', line);
                       break;
                     case ScanSymbol::Type::Semicolon:
                       pushOpSym(tokens, ';', line);
                       break;
                     case ScanSymbol::Type::Comma:
                       pushOpSym(tokens, ',', line);
                       break;
                     case ScanSymbol::Type::Dot:
                       pushOpSym(tokens, '.', line);
                       break;
                     case ScanSymbol::Type::Arrow:
                       pushSymbol(tokens, "->", line);
                       break;
                     case ScanSymbol::Type::Increment:
                       pushOpSym(tokens, '+', line);
                       pushOpSym(tokens, '+', line);
                       break;
                     case ScanSymbol::Type::Decrement:
                       pushOpSym(tokens, '-', line);
                       pushOpSym(tokens, '-', line);
                       break;
                     case ScanSymbol::Type::FatArrow:
                       pushSymbol(tokens, "=>", line);
                       break;
                     case ScanSymbol::Type::Colon:
                       pushOpSym(tokens, ':', line);
                       break;
                     case ScanSymbol::Type::DoubleColon:
                       pushSymbol(tokens, "::", line);
                       break;
                     case ScanSymbol::Type::Question:
                       pushRef(tokens, line);
                       break;
                     case ScanSymbol::Type::Tilde:
                       pushOpSym(tokens, '~', line);
                       break;
                     case ScanSymbol::Type::Caret:
                       pushOpSym(tokens, '^', line);
                       break;
                     case ScanSymbol::Type::At:
                       pushOpSym(tokens, '@', line);
                       break;
                     case ScanSymbol::Type::Pipe:
                       pushOpSym(tokens, '|', line);
                       break;
                     case ScanSymbol::Type::Backslash:
                       pushOpSym(tokens, '\\', line);
                       break;
                     default:
                       throw err::Exception("Unknown symbol on line " +
                                            std::to_string(line));
                     }
                   },
                   [&](const aflat::scan::token::Error &errToken) {
                     throw err::Exception("Invalid token: " +
                                          std::string(errToken.message));
                   },
                   [&](const aflat::scan::token::Eof &) {
                     // Already handled before visit
                   }});
  }

  auto *lastSemi = new lex::OpSym();
  lastSemi->Sym = ';';
  lastSemi->lineCount = lastLine;
  tokens.push(lastSemi);

  return tokens;
}
