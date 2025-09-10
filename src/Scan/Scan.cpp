#include "Scan/Scan.hpp"
#include <cerrno>
#include <fstream>
#include <optional>

namespace aflat::scan {

struct Scanner::Impl {
  std::ifstream file;
  token::Position pos; // current position in the file
  // will be a unique id for each source file eventually
  token::SourceId source_id = 1;

  // peek buffer to hold one token after a peek
  std::optional<token::Token> peeked_token;

  explicit Impl(const char *path) : file(path, std::ios::binary) {}
  bool ok() const { return file.good(); }

  void skip_whitespace() {
    char c;
    while (file.get(c)) {
      if (c == ' ' || c == '\t' || c == '\r') {
        pos.column++;
        pos.byte_offset++;
      } else if (c == '\n') {
        pos.line++;
        pos.column = 1;
        pos.byte_offset++;
      } else {
        // put back the non-whitespace character and return
        file.unget();
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
    while (file.get(c) && (std::isdigit(c) || c == '.' || c == '_')) {
      if (c == '.') {
        if (floating_point) {
          // second dot in number, stop parsing here
          file.unget();
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
    if (file)
      file.unget();

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

  outcome::result<token::Token, std::error_code> next() {
    // If there is a peeked token, there is no need to read from the file
    if (peeked_token) {
      token::Token t = *peeked_token;
      peeked_token.reset();
      return t;
    }

    // skip whitespace
    skip_whitespace();

    char c;
    if (!file.get(c)) {
      if (file.eof()) {
        auto range = token::Range{pos, pos};
        return token::makeEof(range);
      }
    }

    if (auto number = try_scan_number(c)) {
      return *number;
    }
  }
};

outcome::result<Scanner, std::error_code> Scanner::open(const char *path) {
  auto p = std::make_unique<Impl>(path);
  if (!p->ok()) {
    std::error_code ec(errno, std::generic_category());
    if (!ec)
      ec = std::make_error_code(std::errc::no_such_file_or_directory);
    return outcome::failure(ec);
  }
  return Scanner(std::move(p));
}

outcome::result<token::Token, std::error_code> Scanner::next() {
  return impl_->next();
}

} // namespace aflat::scan