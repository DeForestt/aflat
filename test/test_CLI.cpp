#include "CLI.hpp"
#include "catch.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>

void buildTemplate(std::string value);

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

TEST_CASE("generated project README documents native async resources",
          "[cli][readme][async]") {
  namespace fs = std::filesystem;
  const auto project = fs::path("tmp/generated_async_readme");
  fs::remove_all(project);

  buildTemplate(project.string());

  std::ifstream input(project / "README.md");
  REQUIRE(input.is_open());
  const std::string readme((std::istreambuf_iterator<char>(input)),
                           std::istreambuf_iterator<char>());
  CHECK(readme.find("aflat docs Async") != std::string::npos);
  CHECK(readme.find("async fn") != std::string::npos);
  CHECK(readme.find("Async.md") != std::string::npos);

  fs::remove_all(project);
}

TEST_CASE("CLI lsp command", "[cli]") {
  const char *argv[] = {"aflat", "lsp"};
  CommandLineOptions opts;
  REQUIRE(parseCommandLine(2, (char **)argv, opts));
  REQUIRE(opts.command == "lsp");
  REQUIRE(opts.args.empty());
}
