#include "CodeGenerator/MockCodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"
#include "catch.hpp"

TEST_CASE("Definition compares name", "[preprocessor]") {
  Definition d;
  d.name = "FOO";
  d.value = "BAR";
  REQUIRE(Definition::compair(d, "FOO"));
  REQUIRE_FALSE(Definition::compair(d, "BAR"));
}

TEST_CASE("PreProcessor replaces defined values", "[preprocessor]") {
  PreProcessor pp;
  std::string output = pp.PreProcess(".def X = Y\nX\n", "");
  REQUIRE(output == "Y\n");
}

TEST_CASE("getLibPath forms correct suffix", "[cgutils]") {
  using gen::utils::getLibPath;
  std::string path = getLibPath("foo");
  REQUIRE(path.size() >= 9);
  REQUIRE(path.rfind("/libraries/std/foo/", path.size() - 1) !=
          std::string::npos);
}

TEST_CASE("getBytes returns expected values", "[codegen]") {
  auto parser = parse::Parser();
  gen::CodeGenerator gen("mod", parser);
  REQUIRE(gen.getBytes(asmc::QWord) == 8);
  REQUIRE(gen.getBytes(asmc::Word) == 2);
  REQUIRE(gen.getBytes(asmc::Byte) == 1);
  REQUIRE(gen.getBytes(asmc::DWord) == 4);
}
