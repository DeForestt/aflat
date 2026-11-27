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

