#include <filesystem>
#include <fstream>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("new allocates method-only classes", "[codegen][class]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/method_only_class");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "method_only.af";
  const auto output = dir / "method_only.s";
  std::ofstream ofs(source);
  ofs << ".needs <std>\n";
  ofs << "class Empty {\n";
  ofs << "    fn init() -> Self { return my; };\n";
  ofs << "    fn value() -> int { return 1; };\n";
  ofs << "};\n";
  ofs << "fn main() -> int {\n";
  ofs << "    let e = new Empty();\n";
  ofs << "    return e.value();\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(result);
}

TEST_CASE("transform can lower generic decorator fields", "[transform]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/generic_decorator_transform");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "generic_decorator_transform.af";
  const auto output = dir / "generic_decorator_transform.s";
  std::ofstream ofs(source);
  ofs << ".needs <std>\n";
  ofs << "types(T)\n";
  ofs << "class Wrapper {\n";
  ofs << "    adr getter = getter;\n";
  ofs << "    any context = context;\n";
  ofs << "    fn init(adr getter, any context) -> Self { return my; };\n";
  ofs << "};\n";
  ofs << "transform computed\n";
  ofs << "~\n";
  ofs << "fn ${ident}() -> int : Wrapper::<${type}> { return ${expr}; };\n";
  ofs << "~;\n";
  ofs << "class Test {\n";
  ofs << "    @computed\n";
  ofs << "    int i = 1 + 1;\n";
  ofs << "    fn init() -> Self { return my; };\n";
  ofs << "};\n";
  ofs << "fn main() -> int {\n";
  ofs << "    let t = new Test();\n";
  ofs << "    return 0;\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(result);
}

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

TEST_CASE("function pointer type can be used as generic argument",
          "[generics][function-pointer]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/generic_function_pointer_type");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "generic_function_pointer_type.af";
  const auto output = dir / "generic_function_pointer_type.s";
  std::ofstream ofs(source);
  ofs << ".needs <std>\n";
  ofs << "import unordered_map from \"Collections/unordered_map\";\n";
  ofs << "fn add(int left, int right) -> int { return left + right; };\n";
  ofs << "fn main() -> int {\n";
  ofs << "    unordered_map::<adr, int<int, int>> callbacks = "
         "new unordered_map::<adr, int<int, int>>();\n";
  ofs << "    callbacks.set(\"add\", add);\n";
  ofs << "    const int<int, int> callback = "
         "callbacks.get(\"add\").expect(\"add\");\n";
  ofs << "    return callback(20, 22);\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(result);
}

TEST_CASE(
    "vector insert drop and pop_front compile for primitive and unique types",
    "[generics][vector]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/vector_mutators");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "vector_mutators.af";
  const auto output = dir / "vector_mutators.s";
  std::ofstream ofs(source);
  ofs << ".needs <std>\n";
  ofs << "import vector from \"Collections/Vector\";\n";
  ofs << "import option from \"Utils/option\";\n";
  ofs << "import {Some, None, optionWrapper} from \"Utils/option\" under "
         "opt;\n";
  ofs << "import {accept, reject, resultWrapper} from \"Utils/result\" under "
         "res;\n";
  ofs << "unique class Probe {\n";
  ofs << "    fn init() -> Self { return my; };\n";
  ofs << "};\n";
  ofs << "fn main() -> int {\n";
  ofs << "    let values = new vector::<int>();\n";
  ofs << "    values.push_back(1);\n";
  ofs << "    values.push_back(3);\n";
  ofs << "    values.insert(1, 2).expect(\"insert int\");\n";
  ofs << "    values.drop(0).expect(\"drop int\");\n";
  ofs << "    values.pop_front().expect(\"pop int\");\n";
  ofs << "    delete values;\n";
  ofs << "    let probes = new vector::<Probe>();\n";
  ofs << "    let first = new Probe();\n";
  ofs << "    let second = new Probe();\n";
  ofs << "    probes.push_back($first);\n";
  ofs << "    probes.insert(0, $second).expect(\"insert probe\");\n";
  ofs << "    let popped = probes.pop_front().expect(\"pop probe\");\n";
  ofs << "    delete popped;\n";
  ofs << "    probes.drop(0).expect(\"drop probe\");\n";
  ofs << "    delete probes;\n";
  ofs << "    return 0;\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(result);
}

TEST_CASE("result unwrap compiles for object payloads", "[generics][result]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/result_object_payload");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "result_object_payload.af";
  const auto output = dir / "result_object_payload.s";
  std::ofstream ofs(source);
  ofs << ".needs <std>\n";
  ofs << "import result from \"Utils/result\";\n";
  ofs << "import {accept, reject, resultWrapper} from \"Utils/result\" under "
         "res;\n";
  ofs << "class Box {\n";
  ofs << "    mutable int value = 0;\n";
  ofs << "    fn init(int value) -> Self { my.value = value; return my; };\n";
  ofs << "    fn get() -> int { return my.value; };\n";
  ofs << "    fn toString() -> string { return \"box\"; };\n";
  ofs << "};\n";
  ofs << "fn make() -> result::<Box> {\n";
  ofs << "    return res.accept::<Box>(new Box(7));\n";
  ofs << "};\n";
  ofs << "fn main() -> int {\n";
  ofs << "    const Box box = make().expect(\"box\");\n";
  ofs << "    return box.get();\n";
  ofs << "};\n";
  ofs.close();

  bool result =
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(result);
}
