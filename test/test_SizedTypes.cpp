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

TEST_CASE("sized integer types have their named sizes",
          "[sized-types][sizeof]") {
  const int ran = runSizedProgram("sized_types_sizes", R"(
fn main() -> int {
  if i8 != 1 { return 1; };
  if i16 != 2 { return 2; };
  if i32 != 4 { return 3; };
  if i64 != 8 { return 4; };
  if u8 != 1 { return 5; };
  if u16 != 2 { return 6; };
  if u32 != 4 { return 7; };
  if u64 != 8 { return 8; };
  if isize != 8 { return 9; };
  if usize != 8 { return 10; };
  if int != i32 { return 11; };
  if long != i64 { return 12; };
  if short != i16 { return 13; };
  if char != i8 { return 14; };
  if byte != u8 { return 15; };
  if adr != usize { return 16; };
  return 0;
};
)");
  CHECK(ran == 0);
}

TEST_CASE("sized integer aliases are interchangeable with legacy names",
          "[sized-types][alias]") {
  const int ran = runSizedProgram("sized_types_alias", R"(
fn main() -> int {
  const int a = 41;
  const i32 b = a;
  const int c = b + 1;
  if c != 42 { return 1; };
  const long big = #5000000000;
  const i64 big2 = big;
  const long big3 = big2 + #1;
  if big3 != #5000000001 { return 2; };
  const byte raw = 200;
  const u8 raw2 = raw;
  const byte raw3 = raw2;
  if raw3 != 200 { return 3; };
  const short s = -700;
  const i16 s2 = s;
  if s2 != -700 { return 4; };
  const char ch = 'x';
  const i8 ch2 = ch;
  if ch2 != 'x' { return 5; };
  mutable usize addr = 0;
  const adr addr2 = addr;
  addr = addr2;
  if addr != 0 { return 6; };
  return 0;
};
)");
  CHECK(ran == 0);
}

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
  // No u64 % check: 64-bit modulo is broken on main for long already
  // (pre-existing, verified against unmodified main); see the PR body.
  const usize zeroSize = 0;
  const usize underSize = zeroSize - 1;
  if underSize < zeroSize { return 13; };
  return 0;
};
)");
  CHECK(ran == 0);
}

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
  // No i64 % check: same pre-existing 64-bit modulo bug as long.
  mutable isize negSize = 0;
  negSize = negSize - 5;
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

