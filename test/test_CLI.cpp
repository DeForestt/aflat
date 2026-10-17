#include "CLI.hpp"
#include "catch.hpp"

#include <cstdlib>
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

TEST_CASE("CLI coverage flag", "[cli][coverage]") {
  const char *argv[] = {"aflat", "test", "--coverage"};
  CommandLineOptions opts;
  REQUIRE(parseCommandLine(3, (char **)argv, opts));
  REQUIRE(opts.coverage == true);
  REQUIRE(opts.command == "test");
}

TEST_CASE("CLI coverage reports uncovered production lines",
          "[cli][coverage]") {
  namespace fs = std::filesystem;
  const fs::path project = fs::current_path() / "tmp/coverage_cli";
  fs::remove_all(project);
  fs::create_directories(project / "src/test");
  fs::create_directories(project / "src/vendor");
  fs::create_directories(project / "bin");
  fs::create_directories(project / ".cache/coverage/vendor");

  std::ofstream staleCoverage(project /
                              ".cache/coverage/vendor/mod.s.coverage");
  staleCoverage << "__stale_vendor_counter\n./src/vendor/mod.af\n4\n";
  staleCoverage.close();

  std::ofstream config(project / "aflat.cfg");
  config << "[build]\nmain = calc\ntest = test/test\n"
            "[dependencies]\nvendor = "
            "\"https://example.invalid/vendor.git\"\n";
  config.close();

  std::ofstream vendorConfig(project / "vendor.aflat.cfg");
  vendorConfig << "[dependencies]\nvendor = \"./src/mod.af\"\n";
  vendorConfig.close();

  std::ofstream vendor(project / "src/vendor/mod.af");
  vendor << ".needs <std>\n\n"
            "export fn identity(const int value) -> int {\n"
            "  return value;\n"
            "};\n";
  vendor.close();

  std::ofstream production(project / "src/calc.af");
  production << ".needs <std>\n\n"
                "import {identity} from \"./vendor\" under vendor;\n\n"
                "export fn classify(const int value) -> int {\n"
                "  if vendor.identity(value) > 0 {\n"
                "    return 1;\n"
                "  };\n"
                "  return 0;\n"
                "};\n";
  production.close();

  std::ofstream tests(project / "src/test/test.af");
  tests << ".needs <std>\n.needs <test>\n"
           "import {classify} from \"../calc\" under calc;\n"
           "import {describe, it, assertEqual, summary} from \"ATest\" under "
           "test;\n"
           "fn main() -> int {\n"
           "  test.describe(\"coverage\", fn () {\n"
           "    test.it(\"covers one branch\", fn () {\n"
           "      test.assertEqual(calc.classify(1), 1);\n"
           "    });\n"
           "  });\n"
           "  test.summary();\n"
           "  return 0;\n"
           "};\n";
  tests.close();

  const fs::path compiler =
      fs::canonical("/proc/self/exe").parent_path() / "aflat";
  const fs::path output = project / "coverage.out";
  const std::string command =
      "cd \"" + project.string() + "\" && \"" + compiler.string() +
      "\" test --coverage --quiet > \"" + output.string() + "\" 2>&1";
  REQUIRE(std::system(command.c_str()) == 0);

  std::ifstream report(output);
  const std::string content((std::istreambuf_iterator<char>(report)),
                            std::istreambuf_iterator<char>());
  CHECK(content.find("./src/calc.af: 2/3 (66.7%)") != std::string::npos);
  CHECK(content.find("missing: 9") != std::string::npos);
  CHECK(content.find("may be dead code or missing test coverage") !=
        std::string::npos);
  CHECK(content.find("src/test/test.af:") == std::string::npos);
  CHECK(content.find("src/vendor/mod.af:") == std::string::npos);

  fs::remove_all(project);
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
  CHECK(readme.find("aflat test --coverage") != std::string::npos);
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
