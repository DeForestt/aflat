#include "PreProcessor.hpp"
#include "catch.hpp"

TEST_CASE("PreProcessor define replacement", "[preprocessor]") {
  PreProcessor pp;
  std::string input = ".def X = Y\nX\n";
  REQUIRE(pp.PreProcess(input, "", "") == "Y\n");
}
