#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("float arguments and return values survive nested calls",
          "[call][float]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/float_calls");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "float_calls.af";
  const auto assembly = dir / "float_calls.s";
  const auto executable = dir / "float_calls";

  std::ofstream(source) << R"(fn echo(const float value) -> float {
  return value;
};

fn add(const float left, const float right) -> float {
  return left + right;
};

fn main() -> int {
  const let actual = echo(add(1.25, 2.25));
  if actual == 3.5 {
    return 0;
  };
  return 1;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  int linked = -1;
  int ran = -1;
  if (built) {
    const auto linkCommand =
        "gcc -no-pie " + assembly.string() + " -o " + executable.string();
    linked = std::system(linkCommand.c_str());
    if (linked == 0)
      ran = std::system(executable.string().c_str());
  }

  fs::remove_all(dir);
  REQUIRE(built);
  REQUIRE(linked == 0);
  REQUIRE(ran == 0);
}
