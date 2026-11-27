
#include <filesystem>
#include <fstream>
#include <string>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

namespace {
void dumpAsm(const std::string &name, const std::string &source) {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp") / ("dump_" + name);
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto input = dir / "main.af";
  const auto assembly = dir / "main.s";
  std::ofstream(input) << source;
  if (build(input.string(), assembly.string(), cfg::Mutability::Strict,
            false)) {
    std::error_code ec;
    fs::copy_file(assembly, fs::path("asm_dump_" + name + ".s"),
                  fs::copy_options::overwrite_existing, ec);
  }
  fs::remove_all(dir);
}
} // namespace

TEST_CASE("dump assembly for failing operations", "[asm-dump]") {
  dumpAsm("i64mod", R"(
fn main() -> int {
  const i64 w = #100;
  if (w % 3) != 1 { return 1; };
  return 0;
};
)");
  dumpAsm("i64div", R"(
fn main() -> int {
  const i64 w = #100;
  if (w / 3) != 33 { return 1; };
  return 0;
};
)");
  dumpAsm("isize_ge", R"(
fn main() -> int {
  const isize a = 0 - 5;
  if a >= 0 { return 9; };
  return 0;
};
)");
  dumpAsm("i32mod", R"(
fn main() -> int {
  const i32 w = 100;
  if (w % 3) != 1 { return 1; };
  return 0;
};
)");
  CHECK(true);
}
