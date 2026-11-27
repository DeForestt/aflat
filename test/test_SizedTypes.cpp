#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

namespace {

// Compile an AFlat program to assembly, link it with gcc, and run it.
// The program returns 0 on success and a distinct nonzero code for each
// failed expectation, so a failing CHECK identifies the broken behavior
// in the test source.
int runSizedProgram(const std::string &name, const std::string &source) {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp") / name;
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto input = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto executable = dir / "main";
  std::ofstream(input) << source;
  if (!build(input.string(), assembly.string(), cfg::Mutability::Strict,
             false)) {
    fs::remove_all(dir);
    return -1000;
  }
  if (std::system(("gcc -no-pie " + assembly.string() + " -o " +
                   executable.string())
                      .c_str()) != 0) {
    fs::remove_all(dir);
    return -2000;
  }
  const int ran = std::system(executable.c_str());
  fs::remove_all(dir);
  return ran;
}

} // namespace

TEST_CASE("unsigned integers wrap and use unsigned division, comparison "
          "and shift",
          "[sized-types][unsigned]") {
  const int ran = runSizedProgram("sized_types_unsigned", R"(
fn main() -> int {
  mutable u8 small = 255;
  small = small + 1;
  if small != 0 { return 1; };
  mutable u16 mid = 65535;
  mid = mid + 1;
  if mid != 0 { return 2; };
  const u32 two = 2000000000;
  const u32 big = two + two;
  if big < two { return 3; };
  if big / 2 != 2000000000 { return 4; };
  if (big % 2) != 0 { return 5; };
  const u32 zero32 = 0;
  const u32 under = zero32 - 1;
  if under < zero32 { return 6; };
  if (under >> 1) != 2147483647 { return 7; };
  if under / 2 != 2147483647 { return 8; };
  if (under % 2) != 1 { return 9; };
  const u64 zero64 = 0;
  const u64 under64 = zero64 - 1;
  if under64 < zero64 { return 10; };
  if under64 / 2 != #9223372036854775807 { return 11; };
  if (under64 % 2) != 1 { return 12; };
  const usize zeroSize = 0;
  const usize underSize = zeroSize - 1;
  if underSize < zeroSize { return 13; };
  return 0;
};
)");
  CHECK(ran == 0);
}

