#include "Scan/Scan.hpp"
#include <cerrno>
#include <fstream>
#include <optional>

namespace aflat::scan {

struct Scanner::Impl {
  std::istream &input;
  token::Position pos; // current position in the file
  // will be a unique id for each source file eventually
  token::SourceId source_id = 1;

  // peek buffer to hold one token after a peek
  std::optional<token::Token> peeked_token;

  explicit Impl(std::istream &input, token::SourceId source_id)
      : input(input), source_id(source_id) {
    pos = token::Position{source_id, 1, 0, 0};
  }
  bool ok() const { return input.good(); }

  bool get(char &c) {
    if (input.get(c)) {
      pos.column++;
      pos.byte_offset++;
      if (c == '\n') {
        pos.line++;
        pos.column = 1;
      }
      return true;
    } else {
      c = '\0'; // indicate end of file or error
      return false;
    }
  }

  void unget() {
    if (input.unget()) {
      pos.column--;
      pos.byte_offset--;
    }
  }

  void skip_whitespace() {
    char c;
    while (get(c)) {
      if (!std::isspace(c)) {
        unget();
        break;
      }
    }
  }

  std::optional<token::Token> try_scan_number(char first_char) {
    if (!std::isdigit(first_char))
      return std::nullopt;

    std::string num_str;
    num_str += first_char;
    token::Position start_pos = pos;
    bool floating_point = false;
    char c;
    while (get(c) && (std::isdigit(c) || c == '.' || c == '_')) {
      if (c == '.') {
        if (floating_point) {
          // second dot in number, stop parsing here
          unget();
          break;
        }
        floating_point = true;
      }

      if (c != '_') {
        num_str += c;
      }
    }

    // put back the non-digit character
    if (input)
      unget();

    token::Position end_pos = pos;
    token::Range range{start_pos, end_pos};

    if (floating_point) {
      // parse as floating point
      double v = std::stod(num_str);
      auto tok = token::makeFloat(range, v);
      return tok;
    }

    // parse as integer
    auto tok = token::makeInteger(range, stoi(num_str));
    return tok;
  }

  std::optional<token::Token> try_scan_keyword_or_ident(char first_char) {
    if (!std::isalpha(first_char) && first_char != '_')
      return std::nullopt;

    std::string ident_str;
    ident_str += first_char;
    token::Position start_pos = pos;
    char c;
    while (get(c) && (std::isalnum(c) || c == '_')) {
      ident_str += c;
    }

    // put back the non-identifier character
    if (input)
      unget();

    token::Position end_pos = pos;
    token::Range range{start_pos, end_pos};

    if (auto kw = token::keyword_from(ident_str)) {
      return token::makeKeyword(range, *kw);
    }

    return token::makeIdentifier(range, ident_str);
  }

  std::optional<token::Token> try_scan_symbol(char first_char) {
    // A symbol can be one or two characters.. the parsing will be greedy so we
    // start by trying to get two characters
    char second_char;
    if (!get(second_char)) {
      // if we can't get a second character, just try to parse the first
      // character as a
      if (auto sym = token::symbol_from(std::string(1, first_char))) {
        token::Position start_pos = pos;
        token::Position end_pos = pos;
        token::Range range{start_pos, end_pos};
        return token::makeSymbol(range, *sym);
      }
      return std::nullopt;
    }
    std::string sym_str =
        std::string(1, first_char) + std::string(1, second_char);
    if (auto sym = token::symbol_from(sym_str)) {
      token::Position start_pos = pos;
      token::Position end_pos = pos;
      token::Range range{start_pos, end_pos};
      return token::makeSymbol(range, *sym);
    }
    // if we can't parse the two character symbol, try the first character alone
    unget();
    if (auto sym = token::symbol_from(std::string(1, first_char))) {
      token::Position start_pos = pos;
      token::Position end_pos = pos;
      token::Range range{start_pos, end_pos};
      return token::makeSymbol(range, *sym);
    }
    return std::nullopt;
  }

  std::optional<token::Token> try_scan_string(char quote_char) {
    if (quote_char != '"' && quote_char != '`')
      return std::nullopt;
    std::string str_val;
    token::Position start_pos = pos;
    char c;
    while (get(c)) {
      if (c == quote_char) {
        // end of string
        token::Position end_pos = pos;
        token::Range range{start_pos, end_pos};
        if (quote_char == '`')
          return token::makeTemplate(range, str_val);
        return token::makeString(range, str_val);
      }
      if (c == '\\') {
        // handle escape sequences
        if (get(c)) {
          switch (c) {
          case 'n':
            str_val += '\n';
            break;
          case 't':
            str_val += '\t';
            break;
          case 'r':
            str_val += '\r';
            break;
          case '\\':
            str_val += '\\';
            break;
          case '\'':
            str_val += '\'';
            break;
          case '\"':
            str_val += '\"';
            break;
          case '0':
            str_val += '\0';
            break;
          case 'b':
            str_val += '\b';
            break;
          case '`':
            str_val += '`';
            break;
          default:
            str_val += c; // unknown escape, just add the char
            break;
          }
        } else {
          // unterminated escape sequence
          return std::nullopt;
        }
      } else {
        str_val += c;
      }
    }
    // if we reach here, we hit EOF before closing quote
    return std::nullopt;
  }

  outcome::result<token::Token, std::error_code> next() {

    if (peeked_token) {
      token::Token t = *peeked_token;
      peeked_token.reset();
      return t;
    }

    skip_whitespace();

    char c;
    if (get(c)) {
      if (input.eof()) {
        auto range = token::Range{pos, pos};
        return token::makeEof(range);
      }
    } else {
      auto range = token::Range{pos, pos};
      return token::makeEof(range);
    }
    if (auto number = try_scan_number(c)) {
      return *number;
    }
    if (auto keyword_or_ident = try_scan_keyword_or_ident(c)) {
      return *keyword_or_ident;
    }
    if (auto symbol = try_scan_symbol(c)) {
      return *symbol;
    }
    if (auto str = try_scan_string(c)) {
      return *str;
    }

    // return an error for unrecognized characters
    return outcome::failure(std::make_error_code(std::errc::invalid_argument));
  }
};

outcome::result<token::Token, std::error_code> Scanner::next() {
  return impl_->next();
}

Scanner::Scanner(std::istream &input, token::SourceId source_id)
    : impl_(new Impl(input, source_id)) {}
Scanner::~Scanner() { delete impl_; }

} // namespace aflat::scan