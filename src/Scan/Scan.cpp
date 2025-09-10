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
    pos = token::Position{source_id, 1, 1, 0};
  }
  bool ok() const { return input.good(); }

  void skip_whitespace() {
    char c;
    while (input.get(c)) {
      if (c == ' ' || c == '\t' || c == '\r') {
        pos.column++;
        pos.byte_offset++;
      } else if (c == '\n') {
        pos.line++;
        pos.column = 1;
        pos.byte_offset++;
      } else {
        // put back the non-whitespace character and return
        input.unget();
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
    while (input.get(c) && (std::isdigit(c) || c == '.' || c == '_')) {
      if (c == '.') {
        if (floating_point) {
          // second dot in number, stop parsing here
          input.unget();
          break;
        }
        floating_point = true;
      }

      if (c != '_') {
        num_str += c;
      }

      pos.column++;
      pos.byte_offset++;
    }

    // put back the non-digit character
    if (input)
      input.unget();

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
    while (input.get(c) && (std::isalnum(c) || c == '_')) {
      ident_str += c;
      pos.column++;
      pos.byte_offset++;
    }

    // put back the non-identifier character
    if (input)
      input.unget();

    token::Position end_pos = pos;
    token::Range range{start_pos, end_pos};

    // For simplicity, treat all identifiers as the same token type.
    // In a full implementation, you would check against a list of keywords.
    auto tok = token::makeIdentifier(range, ident_str);
    return tok;
  }

  outcome::result<token::Token, std::error_code> next() {

    if (peeked_token) {
      token::Token t = *peeked_token;
      peeked_token.reset();
      return t;
    }

    skip_whitespace();

    char c;
    if (!input.get(c)) {
      if (input.eof()) {
        auto range = token::Range{pos, pos};
        return token::makeEof(range);
      }
    }

    if (auto number = try_scan_number(c)) {
      return *number;
    }
    if (auto keyword_or_ident = try_scan_keyword_or_ident(c)) {
      return *keyword_or_ident;
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