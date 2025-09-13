#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include "Scan/TextEnum.hpp"

namespace aflat::scan::token {

// --- Source identity (swap filename for a small id) ---
using SourceId = std::uint32_t;

struct Position {
  SourceId source = 0;
  std::size_t line = 1;   // the scanner will increment with each '\n'
  std::size_t column = 0; // the scanner will start increment with each get
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

// KeywordUtils.hpp
#pragma once
#include <optional>
#include <string_view>

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
    Export,
    As,
    Fn
  } type;
};

// enum → text
constexpr std::string_view to_string(Keyword::Type k) noexcept {
  switch (k) {
  case Keyword::Type::If:
    return "if";
  case Keyword::Type::Else:
    return "else";
  case Keyword::Type::While:
    return "while";
  case Keyword::Type::For:
    return "for";
  case Keyword::Type::Return:
    return "return";
  case Keyword::Type::Function:
    return "function";
  case Keyword::Type::Let:
    return "let";
  case Keyword::Type::Const:
    return "const";
  case Keyword::Type::Var:
    return "var";
  case Keyword::Type::True:
    return "true";
  case Keyword::Type::False:
    return "false";
  case Keyword::Type::Null:
    return "null";
  case Keyword::Type::Class:
    return "class";
  case Keyword::Type::Extends:
    return "extends";
  case Keyword::Type::Super:
    return "super";
  case Keyword::Type::This:
    return "this";
  case Keyword::Type::New:
    return "new";
  case Keyword::Type::Delete:
    return "delete";
  case Keyword::Type::In:
    return "in";
  case Keyword::Type::Of:
    return "of";
  case Keyword::Type::Break:
    return "break";
  case Keyword::Type::Continue:
    return "continue";
  case Keyword::Type::Switch:
    return "switch";
  case Keyword::Type::Case:
    return "case";
  case Keyword::Type::Default:
    return "default";
  case Keyword::Type::Import:
    return "import";
  case Keyword::Type::Export:
    return "export";
  case Keyword::Type::As:
    return "as";
  case Keyword::Type::Fn:
    return "fn";
  }
  return {}; // unreachable if all cases handled
}

// text → enum (linear scan; fine for a lexer)
inline std::optional<Keyword::Type> keyword_from(std::string_view s) noexcept {
#define TRY(name, lit)                                                         \
  if (s == lit)                                                                \
  return Keyword::Type::name
  TRY(If, "if");
  TRY(Else, "else");
  TRY(While, "while");
  TRY(For, "for");
  TRY(Return, "return");
  TRY(Function, "function");
  TRY(Let, "let");
  TRY(Const, "const");
  TRY(Var, "var");
  TRY(True, "true");
  TRY(False, "false");
  TRY(Null, "null");
  TRY(Class, "class");
  TRY(Extends, "extends");
  TRY(Super, "super");
  TRY(This, "this");
  TRY(New, "new");
  TRY(Delete, "delete");
  TRY(In, "in");
  TRY(Of, "of");
  TRY(Break, "break");
  TRY(Continue, "continue");
  TRY(Switch, "switch");
  TRY(Case, "case");
  TRY(Default, "default");
  TRY(Import, "import");
  TRY(Export, "export");
  TRY(As, "as");
  TRY(Fn, "fn");
#undef TRY
  return std::nullopt;
}

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

inline std::optional<std::int64_t> asInteger(const Token &t) noexcept {
  if (is<IntegerLiteral>(t))
    return get<IntegerLiteral>(t).value;
  return std::nullopt;
}
inline std::optional<double> asFloat(const Token &t) noexcept {
  if (is<FloatLiteral>(t))
    return get<FloatLiteral>(t).value;
  return std::nullopt;
}
inline std::optional<std::string_view> asString(const Token &t) noexcept {
  if (is<StringLiteral>(t))
    return std::string_view(get<StringLiteral>(t).value);
  return std::nullopt;
}
inline std::optional<char> asChar(const Token &t) noexcept {
  if (is<CharLiteral>(t))
    return get<CharLiteral>(t).value;
  return std::nullopt;
}
inline std::optional<std::int64_t> asLong(const Token &t) noexcept {
  if (is<LongLiteral>(t))
    return get<LongLiteral>(t).value;
  return std::nullopt;
}
inline std::optional<std::string_view> asIdentifier(const Token &t) noexcept {
  if (is<Identifier>(t))
    return std::string_view(get<Identifier>(t).name);
  return std::nullopt;
}
inline std::optional<Keyword::Type> asKeyword(const Token &t) noexcept {
  if (is<Keyword>(t))
    return get<Keyword>(t).type;
  return std::nullopt;
}
inline std::optional<Symbol::Type> asSymbol(const Token &t) noexcept {
  if (is<Symbol>(t))
    return get<Symbol>(t).type;
  return std::nullopt;
}
inline std::optional<std::string_view> asTemplate(const Token &t) noexcept {
  if (is<TemplateString>(t))
    return std::string_view(get<TemplateString>(t).value);
  return std::nullopt;
}
inline std::optional<std::string_view> asError(const Token &t) noexcept {
  if (is<Error>(t))
    return std::string_view(get<Error>(t).message);
  return std::nullopt;
}

} // namespace aflat::scan::token
