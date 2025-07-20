#include <filesystem>
#include <fstream>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("union alias reference generics", "[union][generics][ref]") {
  namespace fs = std::filesystem;
  fs::create_directories("tmp");
  std::ofstream ofs("tmp/uref.af");
  ofs << ".needs <std>\n";
  ofs << "types(T)\n";
  ofs << "union RefUnion { A(&T), B(int) };\n";
  ofs << "types(T)\n";
  ofs << "fn makeRef(&T v) -> RefUnion::<T> { return new RefUnion::<T>->A(v); "
         "};\n";
  ofs << "fn main() -> int {\n";
  ofs << "    int x = 5;\n";
  ofs << "    let u = makeRef::<int>(&x);\n";
  ofs << "    return 0;\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build("tmp/uref.af", "tmp/uref.s", cfg::Mutability::Strict, false);
  if (fs::exists("tmp/uref.af")) fs::remove("tmp/uref.af");
  if (fs::exists("tmp/uref.s")) fs::remove("tmp/uref.s");
  fs::remove("tmp");

  REQUIRE(result);
}
