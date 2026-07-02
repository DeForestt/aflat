#include "CompilerUtils.hpp"
#include "catch.hpp"

#include <cstdlib>

namespace {
void unsetPortableOutput() { unsetenv("AFLAT_PORTABLE_OUTPUT"); }
void setPortableOutput() { setenv("AFLAT_PORTABLE_OUTPUT", "1", 1); }
} // namespace

TEST_CASE("compile command uses optimization when not debugging",
          "[compilerutils]") {
  unsetPortableOutput();
  auto cmd = compilerutils::buildCompileCmd("a.c", "a.s", false);
  REQUIRE(cmd.find("-O3") != std::string::npos);
  REQUIRE(cmd.find("-march=native") != std::string::npos);
  REQUIRE(cmd.find("-g") == std::string::npos);
}

TEST_CASE("link command uses optimization when not debugging",
          "[compilerutils]") {
  unsetPortableOutput();
  auto cmd = compilerutils::buildLinkCmd("a.out", "foo.s", false);
  REQUIRE(cmd.find("-O3") != std::string::npos);
  REQUIRE(cmd.find("-march=native") != std::string::npos);
  REQUIRE(cmd.find("-g") == std::string::npos);
}

TEST_CASE("debug flags omit optimizations", "[compilerutils]") {
  unsetPortableOutput();
  auto cmd = compilerutils::buildLinkCmd("a.out", "foo.s", true);
  REQUIRE(cmd.find("-g") != std::string::npos);
  REQUIRE(cmd.find("-O3") == std::string::npos);
}

TEST_CASE("portable output avoids host native CPU flags", "[compilerutils]") {
  setPortableOutput();
  auto compileCmd = compilerutils::buildCompileCmd("a.c", "a.s", false);
  auto linkCmd = compilerutils::buildLinkCmd("a.out", "foo.s", false);
  REQUIRE(compileCmd.find("-march=native") == std::string::npos);
  REQUIRE(linkCmd.find("-march=native") == std::string::npos);
  REQUIRE(compileCmd.find("-march=x86-64") != std::string::npos);
  REQUIRE(linkCmd.find("-mtune=generic") != std::string::npos);
  unsetPortableOutput();
}
