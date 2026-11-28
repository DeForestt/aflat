#pragma once

#include <cstdint>
#include <string>

#include "CodeGenerator/IntTypes.hpp"
#include "Parser/AST.hpp"

namespace gen::utils {

// How a value of one integer type may become a value of another. The
// classification is pure range arithmetic on IntTypeInfo: a conversion
// is lossless exactly when the destination's range covers the source's
// range, so a wider destination alone never makes a conversion safe
// (i32 -> usize is CheckedRequired because usize cannot represent
// negative values).
enum class ConversionKind {
  Identity,        // same canonical type (alias pairs included): no code
  Lossless,        // destination range covers the source range: implicit,
                   // emit an extension
  LiteralFits,     // the source is a literal whose value fits the
                   // destination: implicit, materialize at dest width
  CheckedRequired, // some source values are unrepresentable: implicit
                   // use is a diagnostic; an explicit `as` checks
  NotConvertible   // at least one side is not an integer type
};

ConversionKind classifyConversion(const IntTypeInfo &src,
                                  const IntTypeInfo &dst);
ConversionKind classifyConversion(const std::string &srcType,
                                  const std::string &dstType);

// The range of an integer type as diagnostic text, e.g.
// "-2147483648..2147483647" or "0..4294967295".
std::string intTypeRange(const IntTypeInfo &info);

// True when a literal value is representable in the destination type.
bool literalFits(std::int64_t value, const IntTypeInfo &dst);

// If the expression is an integer literal (plain, `#`, or character,
// possibly parenthesized), store its value and return true.
bool integerLiteralValue(ast::Expr *expr, std::int64_t &value);

// True when the literal source (see integerLiteralValue) is a `#`
// long literal. A long literal bound for a 64-bit destination is
// bit-preserving by construction, so it always fits there even when
// its signed reading is negative (the u64 maximum written in decimal
// reads as -1).
bool longLiteralSource(ast::Expr *expr);

// Diagnostic for an implicit conversion the classifier rejects. Names
// the source and destination types as written, both ranges, and the
// explicit forms that remain available.
std::string implicitConversionDiagnostic(const std::string &srcName,
                                         const std::string &dstName);
// Diagnostic for a literal that does not fit its destination type.
std::string literalRangeDiagnostic(std::int64_t value,
                                   const std::string &dstName);

} // namespace gen::utils
