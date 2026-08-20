#include <filesystem>
#include <fstream>
#include <string>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("match expressions resolve semicolon-free arms independently",
          "[codegen][match][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_match_expression_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(union Choice {
  Number(int),
  Empty
};
fn select(Choice choice) -> bool {
  let selected = match choice {
    Number(value) => value,
    Empty => {
      let fallback = 7;
      fallback
    }
  };
  return selected == 7;
};
fn returnFromMatch(Choice choice) -> int {
  match choice {
    Number(value) => value,
    Empty => {
      let zero = 0;
      zero
    }
  };
};
fn explicitlyResolve(Choice choice) -> bool {
  let selected = match choice {
    Number(value) => resolve value,
    Empty => resolve 0
  };
  return selected == 0;
};
fn main() -> int { return 0; };
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(built);
}
