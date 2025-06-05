#include "CompilerUtils.hpp"
#include "catch.hpp"

TEST_CASE("compile command uses optimization when not debugging", "[compilerutils]") {
  auto cmd = compilerutils::buildCompileCmd("a.c", "a.s", false);
  REQUIRE(cmd.find("-O3") != std::string::npos);
  REQUIRE(cmd.find("-march=native") != std::string::npos);
  REQUIRE(cmd.find("-g") == std::string::npos);
}

TEST_CASE("link command uses optimization when not debugging", "[compilerutils]") {
  auto cmd = compilerutils::buildLinkCmd("a.out", "foo.s", false);
  REQUIRE(cmd.find("-O3") != std::string::npos);
  REQUIRE(cmd.find("-march=native") != std::string::npos);
  REQUIRE(cmd.find("-g") == std::string::npos);
}

TEST_CASE("debug flags omit optimizations", "[compilerutils]") {
  auto cmd = compilerutils::buildLinkCmd("a.out", "foo.s", true);
  REQUIRE(cmd.find("-g") != std::string::npos);
  REQUIRE(cmd.find("-O3") == std::string::npos);
}
