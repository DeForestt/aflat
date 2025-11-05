#pragma once
#include <cstdint>
#include <functional>
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

  std::string toString() const {
    return "line " + std::to_string(line) + ", column " +
           std::to_string(column);
  }
};

struct Range {
  Position start;
  Position end;
  std::string toString() const {
    return "line " + std::to_string(start.line) + ", column " +
           std::to_string(start.column);
  }
};

// --- token payloads ---
struct Identifier {
  std::string name;
};

#define KEYWORD_ITEMS(X)                                                       \
  X(If, "if")                                                                  \
  X(Else, "else")                                                              \
  X(While, "while")                                                            \
  X(For, "for")                                                                \
  X(Return, "return")                                                          \
  X(Function, "function")                                                      \
  X(Const, "const")                                                            \
  X(Var, "var")                                                                \
  X(True, "true")                                                              \
  X(False, "false")                                                            \
  X(Null, "null")                                                              \
  X(Class, "class")                                                            \
  X(Extends, "extends")                                                        \
  X(Super, "super")                                                            \
  X(This, "this")                                                              \
  X(New, "new")                                                                \
  X(Delete, "delete")                                                          \
  X(In, "in")                                                                  \
  X(Of, "of")                                                                  \
  X(Break, "break")                                                            \
  X(Continue, "continue")                                                      \
  X(Switch, "switch")                                                          \
  X(Case, "case")                                                              \
  X(Default, "default")                                                        \
  X(Import, "import")                                                          \
  X(Export, "export")                                                          \
  X(As, "as")                                                                  \
  X(Fn, "fn")                                                                  \
  X(Let, "let")                                                                \
  X(Match, "match")                                                            \
  X(Safe, "safe")                                                              \
  X(Dynamic, "dynamic")                                                        \
  X(Pedantic, "pedantic")                                                      \
  X(Types, "types")                                                            \
  X(Unique, "unique")                                                          \
  X(Struct, "struct")                                                          \
  X(Union, "union")                                                            \
  X(Enum, "enum")                                                              \
  X(Transform, "transform")                                                    \
  X(When, "when")

DEFINE_ENUM_WITH_CONVERSIONS(Keyword, Type, KEYWORD_ITEMS, keyword)
#undef KEYWORD_ITEMS

#pragma region Symbol
#define SYMBOL_ITEMS(X)                                                        \
  X(Plus, "+")                                                                 \
  X(Minus, "-")                                                                \
  X(Asterisk, "*")                                                             \
  X(Slash, "/")                                                                \
  X(Percent, "%")                                                              \
  X(Equal, "=")                                                                \
  X(DoubleEqual, "==")                                                         \
  X(NotEqual, "!=")                                                            \
  X(Less, "<")                                                                 \
  X(LessEqual, "<=")                                                           \
  X(Greater, ">")                                                              \
  X(GreaterEqual, ">=")                                                        \
  X(And, "&&")                                                                 \
  X(Or, "||")                                                                  \
  X(Not, "!")                                                                  \
  X(LeftParen, "(")                                                            \
  X(RightParen, ")")                                                           \
  X(LeftBrace, "{")                                                            \
  X(RightBrace, "}")                                                           \
  X(LeftBracket, "[")                                                          \
  X(RightBracket, "]")                                                         \
  X(Semicolon, ";")                                                            \
  X(Comma, ",")                                                                \
  X(Dot, ".")                                                                  \
  X(Arrow, "->")                                                               \
  X(Increment, "++")                                                           \
  X(Decrement, "--")                                                           \
  X(FatArrow, "=>")                                                            \
  X(Colon, ":")                                                                \
  X(DoubleColon, "::")                                                         \
  X(Question, "?")                                                             \
  X(Tilde, "~")                                                                \
  X(Caret, "^")                                                                \
  X(At, "@")                                                                   \
  X(Pipe, "|")                                                                 \
  X(Backslash, "\\")

DEFINE_ENUM_WITH_CONVERSIONS(Symbol, Type, SYMBOL_ITEMS, symbol)
#undef SYMBOL_ITEMS

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
inline Token makeIdentifier(const Range &r, std::string name) {
  return {r, Identifier{std::move(name)}};
}
inline Token makeKeyword(const Range &r, Keyword::Type type) {
  return {r, Keyword{type}};
}
inline Token makeInteger(const Range &r, std::int64_t v) {
  return {r, IntegerLiteral{v}};
}
inline Token makeFloat(const Range &r, double v) {
  return {r, FloatLiteral{v}};
}
inline Token makeString(const Range &r, std::string v) {
  return {r, StringLiteral{std::move(v)}};
}
inline Token makeChar(const Range &r, char v) { return {r, CharLiteral{v}}; }
inline Token makeLong(const Range &r, std::int64_t v) {
  return {r, LongLiteral{v}};
}
inline Token makeSymbol(const Range &r, Symbol::Type type) {
  return {r, Symbol{type}};
}
inline Token makeTemplate(const Range &r, std::string v) {
  return {r, TemplateString{std::move(v)}};
}
inline Token makeEof(Range r) { return {std::move(r), Eof{}}; }
inline Token makeError(const Range &r, std::string msg) {
  return {r, Error{std::move(msg)}};
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
    return std::get<IntegerLiteral>(t.kind).value;
  return std::nullopt;
}
inline std::optional<double> asFloat(const Token &t) noexcept {
  if (is<FloatLiteral>(t))
    return std::get<FloatLiteral>(t.kind).value;
  return std::nullopt;
}
inline std::optional<std::string_view> asString(const Token &t) noexcept {
  if (is<StringLiteral>(t))
    return std::string_view(std::get<StringLiteral>(t.kind).value);
  return std::nullopt;
}
inline std::optional<char> asChar(const Token &t) noexcept {
  if (is<CharLiteral>(t))
    return std::get<CharLiteral>(t.kind).value;
  return std::nullopt;
}
inline std::optional<std::int64_t> asLong(const Token &t) noexcept {
  if (is<LongLiteral>(t))
    return std::get<LongLiteral>(t.kind).value;
  return std::nullopt;
}
inline std::optional<std::string_view> asIdentifier(const Token &t) noexcept {
  if (is<Identifier>(t))
    return std::string_view(std::get<Identifier>(t.kind).name);
  return std::nullopt;
}
inline std::optional<Keyword::Type> asKeyword(const Token &t) noexcept {
  if (is<Keyword>(t))
    return std::get<Keyword>(t.kind).type;
  return std::nullopt;
}
inline std::optional<Symbol::Type> asSymbol(const Token &t) noexcept {
  if (is<Symbol>(t))
    return std::get<Symbol>(t.kind).type;
  return std::nullopt;
}
inline std::optional<std::string_view> asTemplate(const Token &t) noexcept {
  if (is<TemplateString>(t))
    return std::string_view(std::get<TemplateString>(t.kind).value);
  return std::nullopt;
}
inline std::optional<std::string_view> asError(const Token &t) noexcept {
  if (is<Error>(t))
    return std::string_view(std::get<Error>(t.kind).message);
  return std::nullopt;
}

inline std::int64_t
asIntegerOrElse(const Token &t, std::function<std::int64_t()> elseFn) noexcept {
  if (is<IntegerLiteral>(t))
    return std::get<IntegerLiteral>(t.kind).value;
  return elseFn();
}

inline double asFloatOrElse(const Token &t,
                            std::function<double()> elseFn) noexcept {
  if (is<FloatLiteral>(t))
    return std::get<FloatLiteral>(t.kind).value;
  return elseFn();
}

inline std::string_view
asStringOrElse(const Token &t,
               std::function<std::string_view()> elseFn) noexcept {
  if (is<StringLiteral>(t))
    return std::string_view(std::get<StringLiteral>(t.kind).value);
  return elseFn();
}

inline char asCharOrElse(const Token &t,
                         std::function<char()> elseFn) noexcept {
  if (is<CharLiteral>(t))
    return std::get<CharLiteral>(t.kind).value;
  return elseFn();
}

inline std::int64_t
asLongOrElse(const Token &t, std::function<std::int64_t()> elseFn) noexcept {
  if (is<LongLiteral>(t))
    return std::get<LongLiteral>(t.kind).value;
  return elseFn();
}

inline std::string_view
asIdentifierOrElse(const Token &t,
                   std::function<std::string_view()> elseFn) noexcept {
  if (is<Identifier>(t))
    return std::string_view(std::get<Identifier>(t.kind).name);
  return elseFn();
}

inline Keyword::Type
asKeywordOrElse(const Token &t,
                std::function<Keyword::Type()> elseFn) noexcept {
  if (is<Keyword>(t))
    return std::get<Keyword>(t.kind).type;
  return elseFn();
}

inline Symbol::Type
asSymbolOrElse(const Token &t, std::function<Symbol::Type()> elseFn) noexcept {
  if (is<Symbol>(t))
    return std::get<Symbol>(t.kind).type;
  return elseFn();
}

inline std::string_view
asTemplateOrElse(const Token &t,
                 std::function<std::string_view()> elseFn) noexcept {
  if (is<TemplateString>(t))
    return std::string_view(std::get<TemplateString>(t.kind).value);
  return elseFn();
}

inline std::string_view
asErrorOrElse(const Token &t,
              std::function<std::string_view()> elseFn) noexcept {
  if (is<Error>(t))
    return std::string_view(std::get<Error>(t.kind).message);
  return elseFn();
}

} // namespace aflat::scan::token
