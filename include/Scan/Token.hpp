#pragma once
#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace aflat::scan::token {

// --- Source identity (swap filename for a small id) ---
using SourceId = std::uint32_t;

struct Position {
  SourceId source = 0;
  std::size_t line = 1;   // 1-based
  std::size_t column = 1; // 1-based
  std::size_t byte_offset = 0;
};

struct Range {
  Position start;
  Position end;
};

// --- token payloads ---
struct Identifier {
  std::string name;
};

struct Keyword {
  enum class Type {
    If,
    Else,
    While,
    For,
    Return,
    Function,
    Let,
    Const,
    Var,
    True,
    False,
    Null,
    Class,
    Extends,
    Super,
    This,
    New,
    Delete,
    In,
    Of,
    Break,
    Continue,
    Switch,
    Case,
    Default,
    Import,
    Export
  } type;
};

struct IntegerLiteral {
  std::int64_t value;
};
struct FloatLiteral {
  double value;
};
struct StringLiteral {
  std::string value;
};
struct CharLiteral {
  char value;
};
struct LongLiteral {
  std::int64_t value;
};

struct Symbol {
  enum class Type {
    Plus,
    Minus,
    Asterisk,
    Slash,
    Percent,
    Equal,
    DoubleEqual,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    And,
    Or,
    Not,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,
    Semicolon,
    Comma,
    Dot,
    Arrow,
    Colon,
    DoubleColon,
    Question,
    Tilde,
    Caret,
    Pipe,
    Backslash
  } type;
};

struct TemplateString {
  std::string value;
};

// Useful sentinels
struct Eof {};
struct Error {
  std::string message;
};

using TokenValue =
    std::variant<Identifier, Keyword, IntegerLiteral, FloatLiteral,
                 StringLiteral, CharLiteral, LongLiteral, Symbol,
                 TemplateString, Eof, Error>;

struct Token {
  Range range;
  TokenValue kind;
};

// --- visiting & helpers ---
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <class... Fs> decltype(auto) visit(const Token &tok, Fs &&...fs) {
  return std::visit(overloaded{std::forward<Fs>(fs)...}, tok.kind);
}

template <class Alt> bool is(const Token &t) {
  return std::holds_alternative<Alt>(t.kind);
}

template <class Alt> const Alt *as(const Token &t) {
  return std::get_if<Alt>(&t.kind);
}

template <class Alt> const Alt &get(const Token &t) {
  return std::get<Alt>(t.kind);
}

// --- factories (keep call sites terse) ---
inline Token makeIdentifier(Range r, std::string name) {
  return {std::move(r), Identifier{std::move(name)}};
}
inline Token makeKeyword(Range r, Keyword::Type type) {
  return {std::move(r), Keyword{type}};
}
inline Token makeInteger(Range r, std::int64_t v) {
  return {std::move(r), IntegerLiteral{v}};
}
inline Token makeFloat(Range r, double v) {
  return {std::move(r), FloatLiteral{v}};
}
inline Token makeString(Range r, std::string v) {
  return {std::move(r), StringLiteral{std::move(v)}};
}
inline Token makeChar(Range r, char v) {
  return {std::move(r), CharLiteral{v}};
}
inline Token makeLong(Range r, std::int64_t v) {
  return {std::move(r), LongLiteral{v}};
}
inline Token makeSymbol(Range r, Symbol::Type type) {
  return {std::move(r), Symbol{type}};
}
inline Token makeTemplate(Range r, std::string v) {
  return {std::move(r), TemplateString{std::move(v)}};
}
inline Token makeEof(Range r) { return {std::move(r), Eof{}}; }
inline Token makeError(Range r, std::string msg) {
  return {std::move(r), Error{std::move(msg)}};
}

inline bool isEof(const Token &t) { return is<Eof>(t); }
inline bool isError(const Token &t) { return is<Error>(t); }
inline bool isIdentifier(const Token &t) { return is<Identifier>(t); }
inline bool isKeyword(const Token &t) { return is<Keyword>(t); }
inline bool isInteger(const Token &t) { return is<IntegerLiteral>(t); }
inline bool isFloat(const Token &t) { return is<FloatLiteral>(t); }
inline bool isString(const Token &t) { return is<StringLiteral>(t); }
inline bool isChar(const Token &t) { return is<CharLiteral>(t); }
inline bool isLong(const Token &t) { return is<LongLiteral>(t); }
inline bool isSymbol(const Token &t) { return is<Symbol>(t); }
inline bool isTemplate(const Token &t) { return is<TemplateString>(t); }

// TODO: Make these return optionals and handle bad casts better
inline std::int64_t asInteger(const Token &t) {
  return get<IntegerLiteral>(t).value;
}
inline double asFloat(const Token &t) { return get<FloatLiteral>(t).value; }
inline const std::string &asString(const Token &t) {
  return get<StringLiteral>(t).value;
}
inline char asChar(const Token &t) { return get<CharLiteral>(t).value; }
inline std::int64_t asLong(const Token &t) { return get<LongLiteral>(t).value; }
inline const std::string &asIdentifier(const Token &t) {
  return get<Identifier>(t).name;
}
inline Keyword::Type asKeyword(const Token &t) { return get<Keyword>(t).type; }
inline Symbol::Type asSymbol(const Token &t) { return get<Symbol>(t).type; }
inline const std::string &asTemplate(const Token &t) {
  return get<TemplateString>(t).value;
}
inline const std::string &asError(const Token &t) {
  return get<Error>(t).message;
}

} // namespace aflat::scan::token
