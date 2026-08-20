#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Configs.hpp"
#include "Exceptions.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

namespace {
std::string readFile(const std::filesystem::path &path) {
  std::ifstream input(path);
  return {std::istreambuf_iterator<char>(input),
          std::istreambuf_iterator<char>()};
}

std::int64_t parseLongLiteral(const std::string &source) {
  lex::Lexer lexer;
  auto tokens = lexer.Scan(source, 1);
  tokens.invert();
  parse::Parser parser;
  auto *expr = parser.parseExpr(tokens);
  auto *literal = dynamic_cast<ast::LongLiteral *>(expr);
  REQUIRE(literal != nullptr);
  return literal->val;
}
} // namespace

TEST_CASE("long literals cover the signed 64-bit range", "[parser][long]") {
  CHECK(parseLongLiteral("#9000000000") == INT64_C(9000000000));
  CHECK(parseLongLiteral("#9223372036854775807") == INT64_MAX);
  CHECK(parseLongLiteral("#0x7fffffffffffffff") == INT64_MAX);
  CHECK(parseLongLiteral("#-9223372036854775808") == INT64_MIN);

  try {
    (void)parseLongLiteral("#9223372036854775808");
    FAIL("one-above-maximum long literal was accepted");
  } catch (const err::Exception &error) {
    CHECK(error.errorMsg.find("#9223372036854775808") != std::string::npos);
    CHECK(error.errorMsg.find("signed 64-bit range") != std::string::npos);
  }
}

TEST_CASE("float literals remain SSE values in every expression context",
          "[codegen][float][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_float_literal_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto object = dir / "main.o";

  std::ofstream(source) << R"(.needs <std>
union Value { Float(float) };
fn echo(float value) -> float { return value; };
fn literal() -> float { return 1.25; };
fn main() -> int {
  mutable float assigned = 0.0;
  assigned = 3.14;
  let arithmetic = assigned + echo(literal());
  if arithmetic > 4.0 { return 1; };
  let wrapped = new Value->Float(3.14);
  match wrapped {
    Float(value) => { if value == 3.14 { return 0; }; }
  };
  return 1;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  bool integerMoveTouchesXmm = false;
  std::istringstream lines(text);
  for (std::string line; std::getline(lines, line);) {
    if (line.find("movl") != std::string::npos &&
        line.find("%xmm") != std::string::npos)
      integerMoveTouchesXmm = true;
  }
  const int assembled = built ? std::system(("gcc -c " + assembly.string() +
                                             " -o " + object.string())
                                                .c_str())
                              : -1;

  fs::remove_all(dir);
  REQUIRE(built);
  CHECK_FALSE(integerMoveTouchesXmm);
  CHECK(assembled == 0);
}

TEST_CASE("imported union aliases preserve float operand metadata",
          "[imports][union][float][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_imported_union_float_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto object = dir / "main.o";

  std::ofstream(dir / "Value.af") << R"(export unique union DBValue {
  Float(float)
};
)";
  std::ofstream(source) << R"(.needs <std>
import DBValue from "./Value";
fn main() -> int {
  let wrapped = new DBValue->Float(3.14);
  match wrapped {
    Float(value) => { if value == 3.14 { return 0; }; }
  };
  return 1;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  bool integerMoveTouchesXmm = false;
  std::istringstream lines(text);
  for (std::string line; std::getline(lines, line);) {
    if (line.find("movl") != std::string::npos &&
        line.find("%xmm") != std::string::npos)
      integerMoveTouchesXmm = true;
  }
  const int assembled = built ? std::system(("gcc -c " + assembly.string() +
                                             " -o " + object.string())
                                                .c_str())
                              : -1;
  fs::remove_all(dir);
  REQUIRE(built);
  CHECK_FALSE(integerMoveTouchesXmm);
  CHECK(text.find("movss") != std::string::npos);
  CHECK(assembled == 0);
}

TEST_CASE("explicitly sold union payload is evaluated once",
          "[union][ownership][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_union_sold_payload_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
import uni_string from "uni_string";

unique union Value { Text(uni_string) };

fn wrap(uni_string &&value) -> Value {
  return new Value->Text($value);
};

fn main() -> int {
  const Value wrapped = wrap(new uni_string("text"));
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);
  REQUIRE(built);
  CHECK(text.find("pub_Value_del:") != std::string::npos);
  CHECK(text.find("call\tpub_uni_string_del") != std::string::npos);
  CHECK(text.find("call\tpub_uni_string___transfer_to__") != std::string::npos);
  CHECK(text.find("call\taf_free") != std::string::npos);
}

TEST_CASE("owning union falls back to byte transfer without a transfer hook",
          "[union][ownership][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_union_raw_transfer_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
class Payload {
  int value = value;
  fn init(int value) -> Self { return my; };
  fn endScope() -> void { return; };
};

unique union Value { Item(Payload) };

fn main() -> int {
  const Value wrapped = new Value->Item(new Payload(7));
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("pub_Value_del:") != std::string::npos);
  CHECK(text.find("call\tpub_Payload_endScope") != std::string::npos);
  CHECK(text.find("pub_Payload___transfer_to__") == std::string::npos);
  CHECK(text.find("call\taf_free") != std::string::npos);
}

TEST_CASE("automatic transfers copy receiver bytes instead of its stack slot",
          "[ownership][transfer][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_transfer_receiver_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
unique class Payload {
  int value = value;
  fn init(int value) -> Self { return my; };
};

fn main() -> int {
  const Payload payload = new Payload(7);
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  const auto transferStart = text.find("pub_Payload___transfer_to__:");
  REQUIRE(transferStart != std::string::npos);
  const auto copyCall = text.find("call\taf_memcpy", transferStart);
  REQUIRE(copyCall != std::string::npos);
  const auto transferPrefix =
      text.substr(transferStart, copyCall - transferStart);
  CHECK(transferPrefix.find("movq\t-8(%rbp),%r15") != std::string::npos);
  CHECK(transferPrefix.find("lea\t-8(%rbp),%rax") == std::string::npos);
}

TEST_CASE("integer immediates use the width of long comparisons",
          "[codegen][long][comparison][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_long_comparison_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(fn differs(long value) -> bool {
  return value != -1;
};
fn main() -> int {
  if differs(#-1) { return 1; };
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("movq\t$-1") != std::string::npos);
  CHECK(text.find("movl\t$4294967295") == std::string::npos);
}

TEST_CASE("class allocations use the realized decorated layout",
          "[codegen][class][layout][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_decorated_layout_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
import Render from "Utils/Error/Render";
import Error from "Utils/Error";
fn make() -> Error { return new Error("layout"); };
fn main() -> int { return 0; };
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("movl\t$40,%eax") != std::string::npos);
  CHECK(text.find("call\tpub_Error_init") != std::string::npos);
}

TEST_CASE("nested new uses finalized class layouts", "[codegen][new]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_nested_new_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto innerImportedSource = dir / "inner_imported.af";
  const auto innerImportedAssembly = dir / "inner_imported.s";
  const auto bothImportedSource = dir / "both_imported.af";
  const auto bothImportedAssembly = dir / "both_imported.s";

  std::ofstream(source) << R"(.needs <std>
unique class View {
  adr data = data;
  int size = size;
  fn init(adr data, int size) -> Self { return my; };
};
unique class Owner {
  adr data = af_malloc(view.size);
  int size = view.size;
  fn init(immutable View view) -> Self {
    af_memcpy(my.data, view.data, my.size);
    return my;
  };
};
fn make(adr bytes, int size) -> Owner {
  return new Owner(new View(bytes, size));
};
fn main() -> int { return 0; };
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();

  std::ofstream(dir / "ViewOnly.af") << R"(.needs <std>
unique class View {
  adr data = data;
  int size = size;
  fn init(adr data, int size) -> Self { return my; };
};
)";
  std::ofstream(innerImportedSource) << R"(.needs <std>
import View from "./ViewOnly";
unique class Owner {
  adr data = af_malloc(view.size);
  int size = view.size;
  fn init(immutable View view) -> Self {
    af_memcpy(my.data, view.data, my.size);
    return my;
  };
};
fn make(adr bytes, int size) -> Owner {
  return new Owner(new View(bytes, size));
};
fn main() -> int { return 0; };
)";
  const bool innerImportedBuilt =
      build(innerImportedSource.string(), innerImportedAssembly.string(),
            cfg::Mutability::Strict, false);
  const auto innerImportedText =
      innerImportedBuilt ? readFile(innerImportedAssembly) : std::string();

  std::ofstream(dir / "Models.af") << R"(.needs <std>
unique class ImportedView {
  adr data = data;
  int size = size;
  fn init(adr data, int size) -> Self { return my; };
};
unique class ImportedOwner {
  adr data = af_malloc(view.size);
  int size = view.size;
  fn init(ImportedView&& view) -> Self {
    af_memcpy(my.data, view.data, my.size);
    return my;
  };
};
)";
  std::ofstream(bothImportedSource) << R"(.needs <std>
import ImportedView, ImportedOwner from "./Models";
fn make(adr bytes, int size) -> ImportedOwner {
  return new ImportedOwner(new ImportedView(bytes, size));
};
fn main() -> int { return 0; };
)";
  const bool bothImportedBuilt =
      build(bothImportedSource.string(), bothImportedAssembly.string(),
            cfg::Mutability::Strict, false);
  const auto bothImportedText =
      bothImportedBuilt ? readFile(bothImportedAssembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("$12") != std::string::npos);
  CHECK(text.find("movl\t$1,%eax") == std::string::npos);
  REQUIRE(innerImportedBuilt);
  CHECK(innerImportedText.find("$12") != std::string::npos);
  CHECK(innerImportedText.find("movl\t$1,%eax") == std::string::npos);
  REQUIRE(bothImportedBuilt);
  CHECK(bothImportedText.find("$12") != std::string::npos);
  CHECK(bothImportedText.find("movl\t$1,%eax") == std::string::npos);
}

TEST_CASE("non-callable fields are rejected before indirect call lowering",
          "[codegen][call][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_noncallable_field_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);

  const std::vector<std::pair<std::string, std::string>> fields = {
      {"int", "int value = 1;"},
      {"class", "Child value = new Child();"},
  };
  for (const auto &[name, declaration] : fields) {
    const auto source = dir / (name + ".af");
    const auto assembly = dir / (name + ".s");
    std::ofstream(source)
        << ".needs <std>\n"
        << "class Child { fn init() -> Self { return my; }; };\n"
        << "unique class Buffer {\n  " << declaration << "\n"
        << "  fn init() -> Self { return my; };\n};\n"
        << "fn main() -> int {\n"
        << "  let buffer = new Buffer();\n"
        << "  let invalid = buffer.value();\n"
        << "  return 0;\n};\n";

    const bool built = build(source.string(), assembly.string(),
                             cfg::Mutability::Strict, false);
    const auto text = fs::exists(assembly) ? readFile(assembly) : std::string();
    CHECK_FALSE(built);
    CHECK(text.find("call\t*%r11") == std::string::npos);
  }
  fs::remove_all(dir);
}

TEST_CASE("adr fields remain callable as untyped function pointers",
          "[codegen][call][function-pointer][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_adr_field_call_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto runtime = dir / "runtime.c";
  const auto executable = dir / "main";

  std::ofstream(source) << R"(.needs <std>
class Callable {
  int bias = bias;
  adr foo = foo;
  fn init(int bias, adr foo) -> Self { return my; };
  adr invoke(int value) { return my.foo(value); };
};
adr memberCallback(Callable receiver, int value);
adr standaloneCallback(int value);
fn main() -> int {
  let callable = new Callable(37, memberCallback);
  const adr memberRaw = callable.invoke(5);
  adr standalone = standaloneCallback;
  const adr standaloneRaw = standalone(5);
  if (memberRaw as int) != 42 { return 1; };
  if (standaloneRaw as int) != 6 { return 2; };
  return 0;
};
)";
  std::ofstream(runtime) << R"(#include <stddef.h>
struct Callable { int bias; void *foo; };
static unsigned char storage[64];
static int memberResult;
static int standaloneResult;
void *af_malloc(int size) { (void)size; return storage; }
void *memberCallback(struct Callable *receiver, int value) {
  memberResult = receiver->bias + value;
  return &memberResult;
}
void *standaloneCallback(int value) {
  standaloneResult = value + 1;
  return &standaloneResult;
}
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  const int linked =
      built ? std::system(("gcc -no-pie " + assembly.string() + " " +
                           runtime.string() + " -o " + executable.string())
                              .c_str())
            : -1;
  const int ran = linked == 0 ? std::system(executable.string().c_str()) : -1;
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("call\t*%r11") != std::string::npos);
  REQUIRE(linked == 0);
  CHECK(ran == 0);
}

TEST_CASE("computed pointer expressions are valid store targets",
          "[parser][codegen][pointer-store]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_pointer_store_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto object = dir / "main.o";

  std::ofstream(source) << R"(.needs <std>
fn end(adr bytes, int size) -> adr { return bytes + size; };
fn main() -> int {
  adr bytes = af_malloc(4);
  bytes =: 'a';
  (bytes) =: 'b';
  (bytes + 1) =: 'c';
  end(bytes, 2) =: 'd';
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const int assembled = built ? std::system(("gcc -c " + assembly.string() +
                                             " -o " + object.string())
                                                .c_str())
                              : -1;
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(assembled == 0);
  CHECK(text.find("movb\t%al,(%r11)") != std::string::npos);
}
