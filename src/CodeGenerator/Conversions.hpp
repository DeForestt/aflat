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

// The IntTypeInfo a conversion boundary uses for a type as written,
// or nullptr when the type does not participate in the conversion
// rules. `adr` is excluded even though it aliases usize: an adr-typed
// value is a pointer, not an integer source (design section 2.2), and
// keeps its existing pointer rules at every boundary. Only the
// `usize` spelling is the integer face of the alias.
const IntTypeInfo *classifiableIntType(const std::string &name);

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
// Diagnostic for a checked conversion (explicit `as`) of a constant
// that is out of range: the conversion is rejected at compile time.
std::string checkedConstantDiagnostic(std::int64_t value,
                                      const std::string &srcName,
                                      const std::string &dstName);

// One operand of a binary integer operation, resolved for the
// common-type rule (design section 6). A literal operand participates
// by value; a typed operand by its type's range; a computed value of
// literal provenance (its expression type is "number") carries i32
// provenance. Operands that do not participate (classes, floats,
// adr-typed pointers, NULL) leave `present` false: the operation
// keeps its legacy typing for them.
struct OperandInfo {
  bool present = false;
  bool isLiteral = false;
  std::int64_t literalValue = 0;
  const IntTypeInfo *typeInfo = nullptr;
};

// Resolve an operand from its AST node and its generated type.
// Returns false (and leaves `out` untouched) when the operand does
// not participate in the common-type lattice.
bool integerOperandInfo(ast::Expr *expr, const std::string &probedType,
                        OperandInfo &out);

// The common type of a binary operation: the integer type whose
// range covers both operands' ranges (a literal contributes its
// single value), preferring an operand's own type when it covers,
// and the fixed-width name on range-identical pairs (i64 over isize,
// u64 over usize). The result depends only on the pair of ranges,
// never on operand order. Returns nullptr when no integer type
// covers both: the operation is a diagnostic.
const IntTypeInfo *commonIntType(const OperandInfo &a, const OperandInfo &b);

// Diagnostic for a binary operation with no common type.
std::string noCommonTypeDiagnostic(const std::string &aName,
                                   const std::string &bName);

// The display name of an operand for common-type diagnostics: the
// literal's value for a literal operand, the written type name
// otherwise.
std::string operandDisplayName(const OperandInfo &info,
                               const std::string &typeName);

// The result type name of a common-type operation: the written name
// of an operand whose canonical type is the common type (preserving
// alias flavor), else the common type's canonical name.
std::string commonResultName(const OperandInfo &a, const std::string &aName,
                             const OperandInfo &b, const std::string &bName,
                             const IntTypeInfo &common);

} // namespace gen::utils
