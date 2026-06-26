#include <filesystem>
#include <fstream>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("generic type variable declaration", "[generics]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/generic_type_variable");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "generic.af";
  const auto output = dir / "generic.s";
  std::ofstream ofs(source);
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
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(result);
}

TEST_CASE("nested generic member access resolves instantiated type",
          "[generics]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/nested_generic_member");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "nested_generic_member.af";
  const auto output = dir / "nested_generic_member.s";
  std::ofstream ofs(source);
  ofs << ".needs <std>\n";
  ofs << "import vector from \"Collections/Vector\";\n";
  ofs << "import {Some, None, optionWrapper} from \"Utils/option\" under "
         "opt;\n";
  ofs << "import {accept, reject, resultWrapper} from \"Utils/result\" under "
         "res;\n";
  ofs << "types(K, V)\n";
  ofs << "class Entry {\n";
  ofs << "    K key = key;\n";
  ofs << "    V value = value;\n";
  ofs << "    fn init(K key, V value) -> Self { return my; };\n";
  ofs << "};\n";
  ofs << "types(K, V)\n";
  ofs << "class Bucket {\n";
  ofs << "    vector::<Entry::<K, V>> entries = new vector::<Entry::<K, "
         "V>>();\n";
  ofs << "    fn init() -> Self { return my; };\n";
  ofs << "};\n";
  ofs << "fn main() -> int {\n";
  ofs << "    let e = new Entry::<adr, int>(\"test\", 42);\n";
  ofs << "    let b = new Bucket::<adr, int>();\n";
  ofs << "    b.entries.push_back($e);\n";
  ofs << "    return 0;\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(result);
}
