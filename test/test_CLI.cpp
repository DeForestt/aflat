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

TEST_CASE("CLI name flag", "[cli]") {
  const char *argv[] = {"aflat", "--name", "foo", "install", "repo.git"};
  CommandLineOptions opts;
  REQUIRE(parseCommandLine(5, (char **)argv, opts));
  REQUIRE(opts.installName == "foo");
  REQUIRE(opts.command == "install");
}

TEST_CASE("CLI library flag", "[cli]") {
  const char *argv[] = {"aflat", "--lib", "make", "proj"};
  CommandLineOptions opts;
  REQUIRE(parseCommandLine(4, (char **)argv, opts));
  REQUIRE(opts.library == true);
  REQUIRE(opts.command == "make");
  REQUIRE(opts.args.size() == 1);
  REQUIRE(opts.args[0] == std::string("proj"));
}

TEST_CASE("CLI cache flags", "[cli]") {
  const char *argv[] = {"aflat", "--no-cache", "--clean-cache", "build"};
  CommandLineOptions opts;
  REQUIRE(parseCommandLine(4, (char **)argv, opts));
  REQUIRE(opts.noCache == true);
  REQUIRE(opts.cleanCache == true);
  REQUIRE(opts.command == "build");
}

TEST_CASE("CLI concurrent build flag", "[cli]") {
  const char *argv[] = {"aflat", "-j", "build"};
  CommandLineOptions opts;
  REQUIRE(parseCommandLine(3, (char **)argv, opts));
  REQUIRE(opts.concurrent == true);
  REQUIRE(opts.command == "build");
}
