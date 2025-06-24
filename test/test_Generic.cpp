#include <filesystem>
#include <fstream>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("generic type variable declaration", "[generics]") {
  namespace fs = std::filesystem;
  fs::create_directories("tmp");
  std::ofstream ofs("tmp/generic.af");
  ofs << "types(A)\n";
  ofs << "class Box {\n";
  ofs << "    A value = value;\n";
  ofs << "    fn init(A value) -> Self { return my; };\n";
  ofs << "    fn get() -> A { return my.value; };\n";
  ofs << "};\n";
  ofs << "fn main() -> int {\n";
  ofs << "    Box::<int> b = Box::<int>(5);\n";
  ofs << "    return 0;\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build("tmp/generic.af", "tmp/generic.s", cfg::Mutability::Strict, false);
  if (fs::exists("tmp/generic.af")) fs::remove("tmp/generic.af");
  if (fs::exists("tmp/generic.s")) fs::remove("tmp/generic.s");
  fs::remove("tmp");

  REQUIRE(result);
}
