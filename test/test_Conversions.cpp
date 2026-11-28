#include <cstdint>
#include <string>

#include "CodeGenerator/Conversions.hpp"
#include "CodeGenerator/IntTypes.hpp"
#include "Exceptions.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

using gen::utils::classifyConversion;
using gen::utils::ConversionKind;
using gen::utils::intTypeInfo;
using gen::utils::literalFits;

namespace {

std::int64_t parseIntLiteralValue(const std::string &source) {
  lex::Lexer lexer;
  auto tokens = lexer.Scan(source, 1);
  tokens.invert();
  parse::Parser parser;
  auto *expr = parser.parseExpr(tokens);
  auto *literal = dynamic_cast<ast::IntLiteral *>(expr);
  REQUIRE(literal != nullptr);
  return literal->val;
}

} // namespace

TEST_CASE("conversion classification follows range containment",
          "[conversions]") {
  REQUIRE(classifyConversion("i32", "i64") == ConversionKind::Lossless);
  REQUIRE(classifyConversion("int", "long") == ConversionKind::Lossless);
  REQUIRE(classifyConversion("u32", "i64") == ConversionKind::Lossless);
  REQUIRE(classifyConversion("u32", "usize") == ConversionKind::Lossless);
  REQUIRE(classifyConversion("u8", "i16") == ConversionKind::Lossless);
  REQUIRE(classifyConversion("i64", "isize") == ConversionKind::Lossless);
  REQUIRE(classifyConversion("u64", "usize") == ConversionKind::Lossless);
  // A wider destination alone is not enough: sign coverage decides.
  REQUIRE(classifyConversion("i32", "usize") ==
          ConversionKind::CheckedRequired);
  REQUIRE(classifyConversion("i32", "u32") == ConversionKind::CheckedRequired);
  REQUIRE(classifyConversion("u32", "i32") == ConversionKind::CheckedRequired);
  REQUIRE(classifyConversion("i8", "u8") == ConversionKind::CheckedRequired);
  REQUIRE(classifyConversion("i64", "u64") == ConversionKind::CheckedRequired);
  REQUIRE(classifyConversion("u64", "i64") == ConversionKind::CheckedRequired);
  REQUIRE(classifyConversion("usize", "u32") ==
          ConversionKind::CheckedRequired);
  REQUIRE(classifyConversion("i64", "i32") == ConversionKind::CheckedRequired);
  // Alias pairs are the identity conversion.
  REQUIRE(classifyConversion("int", "i32") == ConversionKind::Identity);
  REQUIRE(classifyConversion("adr", "usize") == ConversionKind::Identity);
  REQUIRE(classifyConversion("byte", "u8") == ConversionKind::Identity);
  // Non-integers do not participate.
  REQUIRE(classifyConversion("float", "i32") == ConversionKind::NotConvertible);
  REQUIRE(classifyConversion("i32", "string") ==
          ConversionKind::NotConvertible);
}

TEST_CASE("literal values are judged against the destination range",
          "[conversions]") {
  const auto *i8Info = intTypeInfo("i8");
  const auto *u8Info = intTypeInfo("u8");
  const auto *i32Info = intTypeInfo("i32");
  const auto *u32Info = intTypeInfo("u32");
  const auto *u64Info = intTypeInfo("u64");
  REQUIRE(i8Info != nullptr);
  REQUIRE(literalFits(-128, *i8Info));
  REQUIRE(literalFits(127, *i8Info));
  REQUIRE_FALSE(literalFits(-129, *i8Info));
  REQUIRE_FALSE(literalFits(128, *i8Info));
  REQUIRE(literalFits(255, *u8Info));
  REQUIRE_FALSE(literalFits(256, *u8Info));
  REQUIRE_FALSE(literalFits(-1, *u8Info));
  REQUIRE(literalFits(2147483647, *i32Info));
  REQUIRE_FALSE(literalFits(2147483648LL, *i32Info));
  REQUIRE(literalFits(4294967295LL, *u32Info));
  REQUIRE_FALSE(literalFits(4294967295LL, *i32Info));
  REQUIRE_FALSE(literalFits(-1, *u64Info));
  REQUIRE(literalFits(INT64_MAX, *intTypeInfo("i64")));
  REQUIRE(literalFits(INT64_MIN, *intTypeInfo("i64")));
}

TEST_CASE("integer literals parse at 64-bit precision", "[conversions]") {
  REQUIRE(parseIntLiteralValue("2147483647") == 2147483647);
  REQUIRE(parseIntLiteralValue("5000000000") == 5000000000LL);
  REQUIRE(parseIntLiteralValue("-2147483648") == INT32_MIN);
  REQUIRE(parseIntLiteralValue("0xFFFFFFFF") == 4294967295LL);
  // err::Exception is not a std::exception (it carries errorMsg), so
  // the out-of-range diagnostic is checked with a manual catch.
  bool threw = false;
  try {
    parseIntLiteralValue("99999999999999999999");
  } catch (const err::Exception &error) {
    threw = true;
    REQUIRE(error.errorMsg.find("signed 64-bit") != std::string::npos);
  }
  REQUIRE(threw);
}

TEST_CASE("common type covers both operand ranges, order-free",
          "[conversions]") {
  auto typed = [](const char *name) {
    gen::utils::OperandInfo info;
    info.present = true;
    info.typeInfo = gen::utils::intTypeInfo(name);
    return info;
  };
  auto literal = [](std::int64_t value) {
    gen::utils::OperandInfo info;
    info.present = true;
    info.isLiteral = true;
    info.literalValue = value;
    return info;
  };
  auto common = [](const gen::utils::OperandInfo &a,
                   const gen::utils::OperandInfo &b) {
    const auto *info = gen::utils::commonIntType(a, b);
    return info == nullptr ? std::string("") : info->canonical;
  };
  REQUIRE(common(typed("i32"), typed("u32")) == "i64");
  REQUIRE(common(typed("u32"), typed("i32")) == "i64");
  REQUIRE(common(typed("i8"), typed("u8")) == "i16");
  REQUIRE(common(typed("u8"), typed("i8")) == "i16");
  REQUIRE(common(typed("u32"), typed("i64")) == "i64");
  REQUIRE(common(typed("u32"), typed("usize")) == "usize");
  REQUIRE(common(typed("usize"), typed("u32")) == "usize");
  REQUIRE(common(typed("i64"), typed("isize")) == "i64");
  REQUIRE(common(typed("isize"), typed("i64")) == "i64");
  REQUIRE(common(typed("u64"), typed("u32")) == "u64");
  REQUIRE(common(typed("i64"), typed("u64")) == "");
  REQUIRE(common(typed("isize"), typed("usize")) == "");
  REQUIRE(common(typed("i64"), typed("usize")) == "");
  // A literal participates by value.
  REQUIRE(common(typed("u64"), literal(5)) == "u64");
  REQUIRE(common(typed("i32"), literal(5000000000LL)) == "i64");
  REQUIRE(common(typed("u32"), literal(-1)) == "i64");
  REQUIRE(common(typed("u64"), literal(-1)) == "");
}

TEST_CASE("conversion diagnostics name both types and the reason",
          "[conversions]") {
  const std::string implicit =
      gen::utils::implicitConversionDiagnostic("int", "usize");
  REQUIRE(implicit.find("cannot implicitly convert") != std::string::npos);
  REQUIRE(implicit.find("int") != std::string::npos);
  REQUIRE(implicit.find("usize") != std::string::npos);
  REQUIRE(implicit.find("cannot represent negative") != std::string::npos);
  REQUIRE(implicit.find("checked conversion") != std::string::npos);

  const std::string literal = gen::utils::literalRangeDiagnostic(-1, "byte");
  REQUIRE(literal.find("-1") != std::string::npos);
  REQUIRE(literal.find("byte") != std::string::npos);
  REQUIRE(literal.find("0..255") != std::string::npos);
}
