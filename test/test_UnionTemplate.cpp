#include <filesystem>
#include <fstream>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("generic union construction", "[union][generics]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/union_template");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "union.af";
  const auto output = dir / "union.s";
  std::ofstream ofs(source);
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
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(result);
}
