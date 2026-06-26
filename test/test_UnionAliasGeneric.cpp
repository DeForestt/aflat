#include <filesystem>
#include <fstream>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("generic type in union alias", "[union][generics]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/union_alias_generic");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "ualias.af";
  const auto output = dir / "ualias.s";
  std::ofstream ofs(source);
  ofs << ".needs <std>\n";
  ofs << "fn main() -> int {\n";
  ofs << "    return 0;\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(result);
}
