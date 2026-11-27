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

