#include <cstdint>

#include "CodeGenerator/IntTypes.hpp"
#include "CodeGenerator/TargetInfo.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Parser.hpp"
#include "catch.hpp"

using gen::utils::intTypeInfo;
using gen::utils::intTypeMax;
using gen::utils::intTypeMin;

TEST_CASE("target descriptor describes the current target", "[inttypes]") {
  const gen::TargetInfo &target = gen::currentTarget();
  REQUIRE(target.pointerWidthBytes == 8);
  REQUIRE(target.name == "x86-64");
}

TEST_CASE("int type metadata covers the canonical types", "[inttypes]") {
  const auto *i8 = intTypeInfo("i8");
  REQUIRE(i8 != nullptr);
  REQUIRE(i8->widthBytes == 1);
  REQUIRE(i8->isSigned);
  REQUIRE(intTypeMin(*i8) == -128);
  REQUIRE(intTypeMax(*i8) == 127);

  const auto *i16 = intTypeInfo("i16");
  REQUIRE(i16 != nullptr);
  REQUIRE(i16->widthBytes == 2);
  REQUIRE(i16->isSigned);
  REQUIRE(intTypeMin(*i16) == -32768);
  REQUIRE(intTypeMax(*i16) == 32767);

  const auto *i32 = intTypeInfo("i32");
  REQUIRE(i32 != nullptr);
  REQUIRE(i32->widthBytes == 4);
  REQUIRE(i32->isSigned);
  REQUIRE(intTypeMin(*i32) == INT32_MIN);
  REQUIRE(intTypeMax(*i32) == INT32_MAX);

  const auto *i64 = intTypeInfo("i64");
  REQUIRE(i64 != nullptr);
  REQUIRE(i64->widthBytes == 8);
  REQUIRE(i64->isSigned);
  REQUIRE(intTypeMin(*i64) == INT64_MIN);
  REQUIRE(intTypeMax(*i64) == static_cast<std::uint64_t>(INT64_MAX));

  const auto *u8 = intTypeInfo("u8");
  REQUIRE(u8 != nullptr);
  REQUIRE(u8->widthBytes == 1);
  REQUIRE_FALSE(u8->isSigned);
  REQUIRE(intTypeMin(*u8) == 0);
  REQUIRE(intTypeMax(*u8) == 255);

  const auto *u16 = intTypeInfo("u16");
  REQUIRE(u16 != nullptr);
  REQUIRE(u16->widthBytes == 2);
  REQUIRE_FALSE(u16->isSigned);
  REQUIRE(intTypeMax(*u16) == 65535);

  const auto *u32 = intTypeInfo("u32");
  REQUIRE(u32 != nullptr);
  REQUIRE(u32->widthBytes == 4);
  REQUIRE_FALSE(u32->isSigned);
  REQUIRE(intTypeMax(*u32) == 4294967295ULL);

  const auto *u64 = intTypeInfo("u64");
  REQUIRE(u64 != nullptr);
  REQUIRE(u64->widthBytes == 8);
  REQUIRE_FALSE(u64->isSigned);
  REQUIRE(intTypeMax(*u64) == UINT64_MAX);
}

TEST_CASE("aliases resolve to canonical metadata", "[inttypes]") {
  REQUIRE(intTypeInfo("char") == intTypeInfo("i8"));
  REQUIRE(intTypeInfo("short") == intTypeInfo("i16"));
  REQUIRE(intTypeInfo("int") == intTypeInfo("i32"));
  REQUIRE(intTypeInfo("long") == intTypeInfo("i64"));
  REQUIRE(intTypeInfo("byte") == intTypeInfo("u8"));
  REQUIRE(intTypeInfo("adr") == intTypeInfo("usize"));
}

TEST_CASE("non integer names have no metadata", "[inttypes]") {
  REQUIRE(intTypeInfo("float") == nullptr);
  REQUIRE(intTypeInfo("bool") == nullptr);
  REQUIRE(intTypeInfo("number") == nullptr);
  REQUIRE(intTypeInfo("string") == nullptr);
  REQUIRE(intTypeInfo("") == nullptr);
}

TEST_CASE("target sized widths come from the target descriptor", "[inttypes]") {
  const int pointerWidth = gen::currentTarget().pointerWidthBytes;
  const auto *isize = intTypeInfo("isize");
  REQUIRE(isize != nullptr);
  REQUIRE(isize->widthBytes == pointerWidth);
  REQUIRE(isize->isSigned);
  REQUIRE(intTypeMin(*isize) == INT64_MIN);
  const auto *usize = intTypeInfo("usize");
  REQUIRE(usize != nullptr);
  REQUIRE(usize->widthBytes == pointerWidth);
  REQUIRE_FALSE(usize->isSigned);
  REQUIRE(intTypeMax(*usize) == UINT64_MAX);
  // Fixed-width types do not depend on the target.
  REQUIRE(intTypeInfo("u32")->widthBytes == 4);
  REQUIRE(intTypeInfo("u64")->widthBytes == 8);
}

TEST_CASE("signedness predicates read the metadata table", "[inttypes]") {
  REQUIRE(gen::utils::isIntegerTypeName("int"));
  REQUIRE(gen::utils::isIntegerTypeName("usize"));
  REQUIRE_FALSE(gen::utils::isIntegerTypeName("float"));
  REQUIRE_FALSE(gen::utils::isIntegerTypeName("bool"));
  REQUIRE(gen::utils::isUnsignedIntegerTypeName("byte"));
  REQUIRE(gen::utils::isUnsignedIntegerTypeName("adr"));
  REQUIRE(gen::utils::isUnsignedIntegerTypeName("u32"));
  REQUIRE_FALSE(gen::utils::isUnsignedIntegerTypeName("long"));
  REQUIRE_FALSE(gen::utils::isUnsignedIntegerTypeName("isize"));
  REQUIRE_FALSE(gen::utils::isUnsignedIntegerTypeName("float"));
}

TEST_CASE("parser primitive widths agree with the target", "[inttypes]") {
  const int pointerWidth = gen::currentTarget().pointerWidthBytes;
  REQUIRE(parse::PRIMITIVE_TYPES.at("adr") == pointerWidth);
  REQUIRE(parse::PRIMITIVE_TYPES.at("isize") == pointerWidth);
  REQUIRE(parse::PRIMITIVE_TYPES.at("usize") == pointerWidth);
  REQUIRE(parse::PRIMITIVE_TYPES.at("int") == 4);
  REQUIRE(parse::PRIMITIVE_TYPES.at("u32") == 4);
  REQUIRE(parse::PRIMITIVE_TYPES.at("u64") == 8);
}
