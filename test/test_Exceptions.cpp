#include "Exceptions.hpp"
#include "catch.hpp"

TEST_CASE("Exception stores message", "[exceptions]") {
  err::Exception e("oops");
  REQUIRE(e.errorMsg == "oops");
}
