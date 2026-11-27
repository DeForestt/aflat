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

TEST_CASE("signed integers wrap and use signed division, comparison and "
          "shift",
          "[sized-types][signed]") {
  const int ran = runSizedProgram("sized_types_signed", R"(
fn main() -> int {
  mutable i8 tiny = 127;
  tiny = tiny + 1;
  if tiny != -128 { return 1; };
  if tiny >= 0 { return 2; };
  mutable i16 neg = -300;
  neg = neg / 2;
  if neg != -150 { return 3; };
  const i32 negOne = 0 - 1;
  if (negOne >> 1) != -1 { return 4; };
  const i32 negEight = 0 - 8;
  if negEight / 2 != -4 { return 5; };
  if (negEight >> 2) != -2 { return 6; };
  const i64 wide = #6442450944;
  if wide / 2 != #3221225472 { return 7; };
  if (wide % 2) != 0 { return 8; };
  const isize negSize = 0 - 5;
  if negSize >= 0 { return 9; };
  if negSize / 2 != -2 { return 10; };
  return 0;
};
)");
  CHECK(ran == 0);
}

TEST_CASE("mixed width arithmetic follows the left operand type",
          "[sized-types][mixed]") {
  const int ran = runSizedProgram("sized_types_mixed", R"(
fn main() -> int {
  const i64 wide = #4294967296;
  const i32 narrow = 1;
  const i64 sumWide = wide + narrow;
  if sumWide != #4294967297 { return 1; };
  const i32 twoB = 2000000000;
  const i64 twoB64 = #2000000000;
  const i32 wrapped = twoB + twoB64;
  if wrapped != -294967296 { return 2; };
  const int legacy = 7;
  const i32 sized = 8;
  if legacy + sized != 15 { return 3; };
  if sized + legacy != 15 { return 4; };
  const long legacyLong = #9000000000;
  const i64 sizedLong = #1;
  if legacyLong + sizedLong != #9000000001 { return 5; };
  return 0;
};
)");
  CHECK(ran == 0);
}


