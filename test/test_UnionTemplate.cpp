#include <filesystem>
#include <fstream>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("generic union construction", "[union][generics]") {
  namespace fs = std::filesystem;
  fs::create_directories("tmp");
  std::ofstream ofs("tmp/union.af");
  ofs << ".needs <std>\n";
  ofs << "types(T)\n";
  ofs << "union Choice { A(T), B(int) };\n";
  ofs << "types(T)\n";
  ofs << "fn makeA(T v) -> Choice::<T> { return new Choice::<T>->A(v); };\n";
  ofs << "fn main() -> int {\n";
  ofs << "    let c = makeA::<int>(5);\n";
  ofs << "    return 0;\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build("tmp/union.af", "tmp/union.s", cfg::Mutability::Strict, false);
  if (fs::exists("tmp/union.af"))
    fs::remove("tmp/union.af");
  if (fs::exists("tmp/union.s"))
    fs::remove("tmp/union.s");
  fs::remove("tmp");

  REQUIRE(result);
}
