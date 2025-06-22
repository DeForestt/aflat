#include "CLI.hpp"
#include "catch.hpp"

TEST_CASE("CLI parses flags", "[cli]") {
  const char *argv[] = {"aflat", "-d", "-t", "-q", "-o", "foo.s", "build"};
  CommandLineOptions opts;
  REQUIRE(parseCommandLine(7, (char **)argv, opts));
  REQUIRE(opts.debug == true);
  REQUIRE(opts.traceAlerts == true);
  REQUIRE(opts.quiet == true);
  REQUIRE(opts.outputFile == "foo.s");
  REQUIRE(opts.command == "build");
}

TEST_CASE("CLI default output empty without flag", "[cli]") {
  const char *argv[] = {"aflat", "build"};
  CommandLineOptions opts;
  REQUIRE(parseCommandLine(2, (char **)argv, opts));
  REQUIRE(opts.outputFile.empty());
  REQUIRE(opts.command == "build");
}

TEST_CASE("CLI dependency flags", "[cli]") {
  const char *argv[] = {"aflat", "--update-deps", "--clean-deps", "build"};
  CommandLineOptions opts;
  REQUIRE(parseCommandLine(4, (char **)argv, opts));
  REQUIRE(opts.updateDeps == true);
  REQUIRE(opts.cleanDeps == true);
  REQUIRE(opts.command == "build");
}
