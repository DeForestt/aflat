#include "CodeGenerator/Conversions.hpp"

#include "ASM.hpp"
#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"

namespace gen::utils {

ConversionKind classifyConversion(const IntTypeInfo &src,
                                  const IntTypeInfo &dst) {
  if (src.canonical == dst.canonical)
    return ConversionKind::Identity;
  const std::int64_t srcMin = intTypeMin(src);
  const std::int64_t dstMin = intTypeMin(dst);
  const std::uint64_t srcMax = intTypeMax(src);
  const std::uint64_t dstMax = intTypeMax(dst);
  if (dstMin <= srcMin && srcMax <= dstMax)
    return ConversionKind::Lossless;
  return ConversionKind::CheckedRequired;
}

ConversionKind classifyConversion(const std::string &srcType,
                                  const std::string &dstType) {
  const IntTypeInfo *src = intTypeInfo(srcType);
  const IntTypeInfo *dst = intTypeInfo(dstType);
  if (src == nullptr || dst == nullptr)
    return ConversionKind::NotConvertible;
  return classifyConversion(*src, *dst);
}

const IntTypeInfo *classifiableIntType(const std::string &name) {
  if (name == "adr")
    return nullptr;
  return intTypeInfo(name);
}

std::string intTypeRange(const IntTypeInfo &info) {
  return std::to_string(intTypeMin(info)) + ".." +
         std::to_string(intTypeMax(info));
}

bool literalFits(std::int64_t value, const IntTypeInfo &dst) {
  if (value < 0) {
    if (!dst.isSigned)
      return false;
    return value >= intTypeMin(dst);
  }
  return static_cast<std::uint64_t>(value) <= intTypeMax(dst);
}

bool integerLiteralValue(ast::Expr *expr, std::int64_t &value) {
  while (auto *paren = dynamic_cast<ast::ParenExpr *>(expr))
    expr = paren->expr;
  if (auto *intLit = dynamic_cast<ast::IntLiteral *>(expr)) {
    value = intLit->val;
    return true;
  }
  if (auto *longLit = dynamic_cast<ast::LongLiteral *>(expr)) {
    value = longLit->val;
    return true;
  }
  if (auto *charLit = dynamic_cast<ast::CharLiteral *>(expr)) {
    value = static_cast<std::int64_t>(charLit->value);
    return true;
  }
  return false;
}

namespace {

// The name a diagnostic uses for a type: as written in the source, with
// the canonical name added when the written name is an alias.
std::string displayName(const std::string &name) {
  const std::string canonical = integerTypeAlias(name);
  if (canonical.empty() || canonical == name)
    return name;
  return name + " (" + canonical + ")";
}

} // namespace

bool longLiteralSource(ast::Expr *expr) {
  while (auto *paren = dynamic_cast<ast::ParenExpr *>(expr))
    expr = paren->expr;
  return dynamic_cast<ast::LongLiteral *>(expr) != nullptr;
}

std::string implicitConversionDiagnostic(const std::string &srcName,
                                         const std::string &dstName) {
  const IntTypeInfo *src = intTypeInfo(srcName);
  const IntTypeInfo *dst = intTypeInfo(dstName);
  std::string reason;
  if (src != nullptr && dst != nullptr && src->isSigned && !dst->isSigned)
    reason = displayName(dstName) + " cannot represent negative " +
             displayName(srcName) + " values";
  else
    reason = displayName(dstName) + " cannot represent every " +
             displayName(srcName) + " value";
  std::string message = "cannot implicitly convert " + displayName(srcName) +
                        " to " + displayName(dstName) + ": " + reason;
  if (src != nullptr && dst != nullptr)
    message += " (" + displayName(srcName) + " range " + intTypeRange(*src) +
               ", " + displayName(dstName) + " range " + intTypeRange(*dst) +
               ")";
  message += ". Use a checked conversion (`value as " + dstName +
             "`) if the value is known to fit, or `as wrap` if wrapping "
             "is intended.";
  return message;
}

std::string literalRangeDiagnostic(std::int64_t value,
                                   const std::string &dstName) {
  const IntTypeInfo *dst = intTypeInfo(dstName);
  std::string message = "integer literal " + std::to_string(value) +
                        " does not fit in " + displayName(dstName);
  if (dst != nullptr) {
    message += " (range " + intTypeRange(*dst) + ")";
    if (dst->canonical == "i32" && literalFits(value, *intTypeInfo("i64")))
      message += "; it fits in i64 \u2014 use a # literal or an i64 "
                 "destination";
  }
  return message;
}

bool integerOperandInfo(ast::Expr *expr, const std::string &probedType,
                        OperandInfo &out) {
  std::int64_t literalValue = 0;
  if (integerLiteralValue(expr, literalValue)) {
    out.present = true;
    out.isLiteral = true;
    out.literalValue = literalValue;
    out.typeInfo = nullptr;
    return true;
  }
  if (const IntTypeInfo *info = classifiableIntType(probedType)) {
    out.present = true;
    out.isLiteral = false;
    out.typeInfo = info;
    return true;
  }
  if (probedType == "number") {
    // A computed value of literal provenance (a compound of literals)
    // was computed at int width; i32 is its honest provenance.
    out.present = true;
    out.isLiteral = false;
    out.typeInfo = intTypeInfo("i32");
    return true;
  }
  return false;
}

namespace {

bool rangeCovers(const IntTypeInfo &info, std::int64_t coverMin,
                 std::uint64_t coverMax) {
  return intTypeMin(info) <= coverMin && coverMax <= intTypeMax(info);
}

void operandRange(const OperandInfo &op, std::int64_t &mn, std::uint64_t &mx) {
  if (op.isLiteral) {
    // A literal contributes its single value. A negative value adds
    // nothing to the pair's positive reach (every integer type's max
    // is positive, and literal-literal pairs never reach this code),
    // so it contributes 0 there rather than a wrapped uint64.
    mn = op.literalValue;
    mx = op.literalValue < 0 ? 0 : static_cast<std::uint64_t>(op.literalValue);
    return;
  }
  mn = intTypeMin(*op.typeInfo);
  mx = intTypeMax(*op.typeInfo);
}

} // namespace

const IntTypeInfo *commonIntType(const OperandInfo &a, const OperandInfo &b) {
  std::int64_t aMin = 0, bMin = 0;
  std::uint64_t aMax = 0, bMax = 0;
  operandRange(a, aMin, aMax);
  operandRange(b, bMin, bMax);
  const std::int64_t coverMin = aMin < bMin ? aMin : bMin;
  const std::uint64_t coverMax = aMax > bMax ? aMax : bMax;

  static const char *const kCanonical[] = {
      "i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64", "isize", "usize"};
  const IntTypeInfo *best = nullptr;
  for (const char *name : kCanonical) {
    const IntTypeInfo *cand = intTypeInfo(name);
    if (cand == nullptr || !rangeCovers(*cand, coverMin, coverMax))
      continue;
    if (best == nullptr || cand->widthBytes < best->widthBytes)
      best = cand;
  }
  if (best == nullptr)
    return nullptr;

  // Operand preference: an operand whose own type covers the pair
  // wins over the minimal cover (u32 + usize computes at usize, not
  // u64). When both operands cover and differ, they are a
  // range-identical pair and the fixed-width name wins.
  const IntTypeInfo *prefA = a.isLiteral ? nullptr : a.typeInfo;
  const IntTypeInfo *prefB = b.isLiteral ? nullptr : b.typeInfo;
  const bool aCovers =
      prefA != nullptr && rangeCovers(*prefA, coverMin, coverMax);
  const bool bCovers =
      prefB != nullptr && rangeCovers(*prefB, coverMin, coverMax);
  if (aCovers && bCovers && prefA->canonical != prefB->canonical) {
    if (prefA->canonical == "i64" || prefA->canonical == "u64")
      return prefA;
    if (prefB->canonical == "i64" || prefB->canonical == "u64")
      return prefB;
    return prefA;
  }
  if (aCovers)
    return prefA;
  if (bCovers)
    return prefB;
  return best;
}

std::string noCommonTypeDiagnostic(const std::string &aName,
                                   const std::string &bName) {
  return "no integer type can represent every value of both " +
         displayName(aName) + " and " + displayName(bName) +
         "; convert one operand explicitly (a checked `as` conversion "
         "or `as wrap`) before the operation";
}

std::string operandDisplayName(const OperandInfo &info,
                               const std::string &typeName) {
  if (info.isLiteral)
    return "the integer literal " + std::to_string(info.literalValue);
  return typeName;
}

std::string commonResultName(const OperandInfo &a, const std::string &aName,
                             const OperandInfo &b, const std::string &bName,
                             const IntTypeInfo &common) {
  if (!a.isLiteral && a.typeInfo != nullptr &&
      a.typeInfo->canonical == common.canonical)
    return aName;
  if (!b.isLiteral && b.typeInfo != nullptr &&
      b.typeInfo->canonical == common.canonical)
    return bName;
  return common.canonical;
}

} // namespace gen::utils

void gen::CodeGenerator::emitIntegerExtension(asmc::File &OutputFile,
                                              const std::string &reg,
                                              const utils::IntTypeInfo &src,
                                              const utils::IntTypeInfo &dst) {
  if (dst.widthBytes <= src.widthBytes)
    return;
  const std::string full = registers()[reg]->get(asmc::QWord);
  if (src.isSigned) {
    // Shift the value to the top of the register and arithmetic-shift
    // it back: the sign bit replicates into every widened bit. This is
    // the idiom prepareCompound uses for staged operands.
    const int shift = (8 - src.widthBytes) * 8;
    auto *sal = new asmc::Sal();
    sal->logicalLine = logicalLine();
    sal->size = asmc::QWord;
    sal->op1 = "$" + std::to_string(shift);
    sal->op2 = full;
    OutputFile.text << sal;
    auto *sar = new asmc::Sar();
    sar->logicalLine = logicalLine();
    sar->size = asmc::QWord;
    sar->op1 = "$" + std::to_string(shift);
    sar->op2 = full;
    OutputFile.text << sar;
  } else if (src.widthBytes == 4) {
    // A 32-bit register write zeroes the upper half of the register.
    auto *mov = new asmc::Mov();
    mov->logicalLine = logicalLine();
    mov->size = asmc::DWord;
    mov->from = registers()[reg]->get(asmc::DWord);
    mov->to = mov->from;
    OutputFile.text << mov;
  } else {
    const std::uint64_t bits = (std::uint64_t(1) << (src.widthBytes * 8)) - 1;
    auto *mask = new asmc::And();
    mask->logicalLine = logicalLine();
    mask->size = asmc::QWord;
    mask->op1 = "$" + std::to_string(bits);
    mask->op2 = full;
    OutputFile.text << mask;
  }
}
