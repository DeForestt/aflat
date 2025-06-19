#include "CLI.hpp"
#include "catch.hpp"

TEST_CASE("CLI parses flags", "[cli]") {
  const char *argv[] = {"aflat", "-d", "-t", "-o", "foo.s", "build"};
  CommandLineOptions opts;
  REQUIRE(parseCommandLine(6, (char **)argv, opts));
  REQUIRE(opts.debug == true);
  REQUIRE(opts.traceAlerts == true);
  REQUIRE(opts.outputFile == "foo.s");
  REQUIRE(opts.command == "build");
}
