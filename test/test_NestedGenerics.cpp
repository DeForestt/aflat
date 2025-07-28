#include <filesystem>
#include <fstream>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("nested template instantiation", "[generics]") {
  namespace fs = std::filesystem;
  fs::create_directories("tmp");
  std::ofstream ofs("tmp/nested.af");
  ofs << "types(T)\n";
  ofs << "class Box {\n";
  ofs << "    T value = value;\n";
  ofs << "    fn init(T value) -> Self { return my; };\n";
  ofs << "};\n";
  ofs << "fn main() -> int {\n";
  ofs << "    Box::<Box::<int>> b = Box::<Box::<int>>(Box::<int>(5));\n";
  ofs << "    return 0;\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build("tmp/nested.af", "tmp/nested.s", cfg::Mutability::Strict, false);
  if (fs::exists("tmp/nested.af")) fs::remove("tmp/nested.af");
  if (fs::exists("tmp/nested.s")) fs::remove("tmp/nested.s");
  fs::remove("tmp");

  REQUIRE(result);
}
