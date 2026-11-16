#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);
bool runConfig(cfg::Config &config, const std::string &libPath, char pmode);
std::string getExePath();

TEST_CASE("match expressions resolve semicolon-free arms independently",
          "[codegen][match][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_match_expression_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(shared union Choice {
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

TEST_CASE("match expressions return resolved operation results",
          "[codegen][match][runtime][regression]") {
  namespace fs = std::filesystem;
  struct RestoreWorkingDirectory {
    fs::path previous = fs::current_path();
    ~RestoreWorkingDirectory() { fs::current_path(previous); }
  } restoreWorkingDirectory;
  fs::current_path(fs::path(getExePath()).parent_path().parent_path());
  const auto dir = fs::path("tmp/compiler_match_operation_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);

  // Adapted from aflat_examples/oporation: exercise every explicit resolve arm.
  std::ofstream(dir / "main.af") << R"(.needs <std>
union Operation {
  Add(int),
  Multiply(int),
  Subtract(int),
  Empty
};

fn calculate(int curr, Operation op) -> int {
  return match op {
    Add(x) => {
      resolve curr + x;
    },
    Multiply(x) => {
      resolve curr * x;
    },
    Subtract(x) => {
      resolve curr - x;
    },
    Empty => {
      resolve 0;
    },
  };
};

fn main() -> int {
  let add = Operation->Add(10);
  if calculate(10, add) != 20 { return 1; };
  let multiply = Operation->Multiply(10);
  if calculate(10, multiply) != 100 { return 2; };
  let subtract = Operation->Subtract(3);
  if calculate(10, subtract) != 7 { return 3; };
  let empty = Operation->Empty();
  if calculate(10, empty) != 0 { return 4; };
  return 0;
};
)";

  cfg::Config config;
  config.entryPoint = "../" + (dir / "main").string();
  config.outPutFile = (dir / "main").string();
  REQUIRE(runConfig(config, "libraries/std/", 'e'));
  CHECK(std::system(config.outPutFile.c_str()) == 0);
  fs::remove_all(dir);
}
