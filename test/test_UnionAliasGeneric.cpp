#include <filesystem>
#include <fstream>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("generic type in union alias", "[union][generics]") {
  namespace fs = std::filesystem;
  fs::create_directories("tmp");
  std::ofstream ofs("tmp/ualias.af");
  ofs << ".needs <std>\n";
  ofs << "fn main() -> int {\n";
  ofs << "    return 0;\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build("tmp/ualias.af", "tmp/ualias.s", cfg::Mutability::Strict, false);
  if (fs::exists("tmp/ualias.af")) fs::remove("tmp/ualias.af");
  if (fs::exists("tmp/ualias.s")) fs::remove("tmp/ualias.s");
  fs::remove("tmp");

  REQUIRE(result);
}
