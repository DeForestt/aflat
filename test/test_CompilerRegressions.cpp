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
bool runConfig(cfg::Config &config, const std::string &libPath, char pmode);
std::string getExePath();

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

TEST_CASE("rebuilt string libraries preserve addresses and inline receivers",
          "[codegen][runtime][reference][local]") {
  namespace fs = std::filesystem;
  // CI starts a.test from bin/. Match the compiler's executable-relative
  // library lookup, and restore the caller's working directory on any exit.
  struct RestoreWorkingDirectory {
    fs::path previous = fs::current_path();
    ~RestoreWorkingDirectory() { fs::current_path(previous); }
  } restoreWorkingDirectory;
  fs::current_path(fs::path(getExePath()).parent_path().parent_path());
  const auto dir = fs::path("tmp/compiler_rebuilt_strings_regression");
  fs::remove_all(dir);
  fs::create_directories(dir / "std");
  // Use freshly compiled implementations of the complete formatting path.
  // Existing library assembly alone can hide address-generation regressions.
  for (const auto &entry : fs::directory_iterator("libraries/std")) {
    if (entry.is_regular_file() && entry.path().extension() == ".s")
      fs::copy_file(entry.path(), dir / "std" / entry.path().filename());
  }
  for (const std::string module : {"strings", "String", "std"}) {
    INFO("rebuilding " << module);
    REQUIRE(build("libraries/std/src/" + module + ".af",
                  (dir / "std" / (module + ".s")).string(),
                  cfg::Mutability::Promiscuous, false));
  }
  std::ofstream(dir / "main.af") << R"(.needs <std>
import {print} from "String" under uni;
import {ascii, int_toString, str_comp} from "strings" under str;
unique class Point {
  mutable int x = 0;
  mutable int y = 0;
  fn init() -> Self { return my; };
  fn sum() -> int { return my.x + my.y; };
};
unique class Rectangle {
  local Point topLeft;
  local Point bottomRight;
  fn init() -> Self {
    my.topLeft.x = 10;
    my.topLeft.y = 20;
    my.bottomRight.x = 110;
    my.bottomRight.y = 220;
    return my;
  };
  fn width() -> int { return my.bottomRight.x - my.topLeft.x; };
  fn height() -> int { return my.bottomRight.y - my.topLeft.y; };
};
fn readReference(const int& value) -> int { return value; };
fn main() -> int {
  const int value = 65;
  const adr address = ?value;
  if (address as int) != 65 { return 1; };
  const int& alias = value;
  if alias != 65 { return 6; };
  if readReference(value) != 65 { return 7; };
  if str.ascii(65) != 'A' { return 2; };
  const char[32] buffer;
  str.int_toString(123, buffer);
  if str.str_comp(buffer, "123") != 1 { return 3; };
  let rectangle = new Rectangle();
  if rectangle.width() != 100 { return 4; };
  if rectangle.height() != 200 { return 5; };
  if rectangle.bottomRight.sum() != 330 { return 8; };
  const adr embedded = ?rectangle.bottomRight;
  if (embedded as int) != 110 { return 9; };
  uni.print(`plain template\n`);
  uni.print(`dimensions {rectangle.width()} : {rectangle.height()}\n`);
  return 0;
};
)";
  cfg::Config config;
  config.entryPoint = "../" + (dir / "main").string();
  config.outPutFile = (dir / "main").string();
  REQUIRE(runConfig(config, (dir / "std").string() + "/", 'e'));
  const int ran = std::system(
      (config.outPutFile + " > " + (dir / "stdout.txt").string()).c_str());
  const auto output = readFile(dir / "stdout.txt");
  CHECK(ran == 0);
  CHECK(output == "plain template\ndimensions 100 : 200\n");
  fs::remove_all(dir);
}

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

TEST_CASE("stack-constructed unique classes run del without freeing the stack",
          "[codegen][ownership][stack]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_stack_drop_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
class StackBuffer {
  int size = size;
  fn init(const int size) -> Self { return my; };
  fn del() -> void { return; };
};
fn main() -> int {
  let buffer = StackBuffer(256);
  let alias = buffer;
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  REQUIRE(text.find("lea\tpub_StackBuffer_del(%rip)") != std::string::npos);
  REQUIRE(text.find("call\t*16(%rax)") != std::string::npos);
  CHECK(text.find("call\taf_free") == std::string::npos);
}

TEST_CASE("local class returns use a caller-owned stack destination",
          "[codegen][return][stack]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_local_return_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
class Thing {
  mutable int value = value;
  fn init(const int value) -> Self { my.value = value; return my; };
  safe fn add(immutable Thing other) -> local Thing {
    let result = Thing(my.value + other.value);
    return result;
  };
  fn del() -> void { return; };
};
class Factory {
  fn init() -> Self { return my; };
  fn give() -> local Thing {
    let value = Thing(77);
    return value;
  };
};
fn give() -> local Thing {
  let value = Thing(42);
  return value;
};
fn main() -> int {
  if give().value != 42 { return 1; };
  let factory = Factory();
  if factory.give().value != 77 { return 2; };
  let left = Thing(1);
  let right = Thing(2);
  left.add(right);
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  const auto freeReturn = text.find("give:");
  const auto main = text.find("main:", freeReturn);
  REQUIRE(freeReturn != std::string::npos);
  REQUIRE(main != std::string::npos);
  const auto freeBody = text.substr(freeReturn, main - freeReturn);
  CHECK(freeBody.find("movq\t%rdi,-") != std::string::npos);
  CHECK(freeBody.find("movq\t-16(%rbp),%rdi") != std::string::npos);
  CHECK(freeBody.find(".stack_cleanup_") == std::string::npos);

  const auto methodReturn = text.find("pub_Factory_give:");
  REQUIRE(methodReturn != std::string::npos);
  const auto methodEnd = text.find("\tleave", methodReturn);
  const auto methodBody = text.substr(methodReturn, methodEnd - methodReturn);
  CHECK(methodBody.find("movq\t%rdi,-") != std::string::npos);
  CHECK(methodBody.find("movq\t%rsi,-") != std::string::npos);

  const auto addCall = text.rfind("call\tpub_Thing_add");
  REQUIRE(addCall != std::string::npos);
  const auto addSetup = text.substr(addCall - 800, 800);
  const auto explicitArg = addSetup.rfind(",%rdx");
  const auto receiver = addSetup.rfind(",%rsi");
  const auto destination = addSetup.rfind(",%rdi");
  REQUIRE(explicitArg != std::string::npos);
  REQUIRE(receiver != std::string::npos);
  REQUIRE(destination != std::string::npos);
  CHECK(explicitArg < receiver);
  CHECK(receiver < destination);
}

TEST_CASE("local union and struct returns execute from caller stack storage",
          "[codegen][runtime][return][stack]") {
  namespace fs = std::filesystem;
  struct RestoreWorkingDirectory {
    fs::path previous = fs::current_path();
    ~RestoreWorkingDirectory() { fs::current_path(previous); }
  } restoreWorkingDirectory;
  fs::current_path(fs::path(getExePath()).parent_path().parent_path());

  const auto dir = fs::path("tmp/compiler_local_aggregate_return_runtime");
  fs::remove_all(dir);
  fs::create_directories(dir / "std");
  for (const auto &entry : fs::directory_iterator("libraries/std")) {
    if (entry.is_regular_file() && entry.path().extension() == ".s")
      fs::copy_file(entry.path(), dir / "std" / entry.path().filename());
  }

  std::ofstream(dir / "main.af") << R"(.needs <std>
import {print} from "uni_string" under uni;
struct Pair { int first; int second; };
union Value { Number(int) };
fn givePair() -> local Pair {
  Pair value;
  return value;
};
fn giveValue() -> local Value {
  let value = Value->Number(42);
  return value;
};
fn main() -> int {
  givePair();
  const float progress = 1.0;
  uni.print(`{progress} `);
  match giveValue() {
    Number(value) => { if value == 42 { return 0; }; }
  };
  return 1;
};
)";

  cfg::Config config;
  config.entryPoint = "../" + (dir / "main").string();
  config.outPutFile = (dir / "main").string();
  const bool built = runConfig(config, (dir / "std").string() + "/", 'e');
  const int ran = built ? std::system(config.outPutFile.c_str()) : -1;
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(ran == 0);
}

TEST_CASE("heap-constructed unique classes still run del and af_free",
          "[codegen][ownership][heap]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_heap_drop_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
class HeapBuffer {
  fn del() -> void { return; };
};
fn main() -> int {
  let buffer = new HeapBuffer();
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("call\tpub_HeapBuffer_del") != std::string::npos);
  CHECK(text.find("call\taf_free") != std::string::npos);
}

TEST_CASE("stack-constructed values are cleaned at function exit",
          "[codegen][ownership][stack]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_stack_escape_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
class StackBuffer { fn del() -> void { return; }; };
fn main() -> int {
  mutable StackBuffer alias;
  const bool condition = true;
  if condition {
    let buffer = StackBuffer();
    alias = buffer;
  };
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("lea\tpub_StackBuffer_del(%rip)") != std::string::npos);
  CHECK(text.find("call\t*16(%rax)") != std::string::npos);
}

TEST_CASE("local class fields use embedded storage",
          "[codegen][class][local]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_local_field_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
class Child { int value; };
unique class Parent {
  local Child child;
  fn get() -> int { return my.child.value; };
};
fn main() -> int {
  let parent = new Parent();
  return parent.get();
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();

  fs::remove_all(dir);
  REQUIRE(built);
  CHECK(text.find("lea") != std::string::npos);
}

TEST_CASE("unique local fields transfer through nested lifecycle hooks",
          "[codegen][class][local][ownership][transfer]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_local_field_transfer_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
fn record_del() -> void;
unique class Child {
  int value = 7;
  fn init() -> Self { return my; };
  fn del() -> void { record_del(); return; };
};
unique class Parent {
  local Child child;
  fn init() -> Self { my.child.init(); return my; };
  fn get() -> int { return my.child.value; };
};
fn make() -> Parent {
  let parent = new Parent();
  return parent;
};
fn main() -> int {
  let moved = make();
  return moved.get();
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("pub_Parent___transfer_to__:") != std::string::npos);
  CHECK(text.find("call\tpub_Child___transfer_to__") != std::string::npos);
  CHECK(text.find("call\tpub_Child_del") != std::string::npos);
}

TEST_CASE("assignment into unique local fields transfers the source object",
          "[codegen][class][local][ownership][assignment]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_local_field_assignment_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
unique class Child {
  mutable int value = 7;
  fn init() -> Self { return my; };
};
unique class Parent {
  mutable local Child child;
};
fn main() -> int {
  let parent = new Parent();
  let source = new Child();
  parent.child = source;
  return parent.child.value;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  const auto firstTransfer = text.find("call\tpub_Child___transfer_to__");
  REQUIRE(firstTransfer != std::string::npos);
  CHECK(text.find("call\tpub_Child___transfer_to__", firstTransfer + 1) !=
        std::string::npos);
}

TEST_CASE("local fields can be initialized from class constructor calls",
          "[codegen][class][local][initializer]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_local_field_initializer_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
class Child {
  int value = value;
  fn init(const int value) -> Self { return my; };
};
class Parent {
  local Child child = Child(42);
  fn init() -> Self { return my; };
};
fn main() -> int {
  let parent = new Parent();
  return parent.child.value;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("call\tpub_Child___transfer_to__") != std::string::npos);
  CHECK(text.find("call\taf_free") != std::string::npos);
}

TEST_CASE("local fields can transfer from other local fields",
          "[codegen][class][local][assignment]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_local_field_to_field_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
unique class Child {
  mutable int value = value;
  fn init(const int value) -> Self { return my; };
};
unique class Parent {
  mutable local Child source = Child(42);
  mutable local Child destination;
  fn init() -> Self {
    my.destination = my.source;
    return my;
  };
};
fn main() -> int {
  let parent = new Parent();
  return parent.destination.value;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  REQUIRE(built);
}

TEST_CASE("local fields cannot escape through owning returns",
          "[codegen][class][local][ownership]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_local_field_return_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
class Child {};
unique class Parent {
  local Child child;
  fn leak() -> Child { return my.child; };
};
fn main() -> int { return 0; };
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);
  CHECK_FALSE(built);
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
import uni_string from "uni_string";
union Value { Float(float) };
fn echo(float value) -> float { return value; };
fn literal() -> float { return 1.25; };
fn main() -> int {
  mutable float assigned = 0.0;
  assigned = 3.14;
  let formatted = `value = {assigned}`;
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
  CHECK(text.find(".asciz\t\"value = %f\"") != std::string::npos);
  CHECK(assembled == 0);
}

TEST_CASE("classes with no owning fields emit a no-op destructor",
          "[class][ownership][destructor][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_noop_destructor_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto object = dir / "main.o";

  std::ofstream(source) << R"(.needs <std>
class Vec3 {};
fn main() -> int {
  const Vec3 value = new Vec3();
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  const int assembled = built ? std::system(("gcc -c " + assembly.string() +
                                             " -o " + object.string())
                                                .c_str())
                              : -1;

  fs::remove_all(dir);
  REQUIRE(built);
  CHECK(text.find("pub_Vec3_del:") != std::string::npos);
  CHECK(text.find("call\tpub_Vec3_del") != std::string::npos);
  CHECK(assembled == 0);
}

TEST_CASE("imported constructors retain their required argument count",
          "[imports][class][constructor][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_imported_constructor_args");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(dir / "Vec3.af") << R"(class Vec3 {
  fn init(float x, float y, float z) -> Self { return my; };
};
)";
  std::ofstream(source) << R"(.needs <std>
import Vec3 from "./Vec3";
fn main() -> int {
  const Vec3 value = new Vec3();
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  fs::remove_all(dir);

  CHECK_FALSE(built);
}

TEST_CASE("f-strings keep owned toString results alive while formatting",
          "[codegen][fstring][ownership][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_fstring_owned_tostring");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto object = dir / "main.o";

  std::ofstream(source) << R"(.needs <std>
import uni_string from "uni_string";
class Vec3 {
  fn init() -> Self { return my; };
  fn toString() -> uni_string { return `Vec3`; };
};
fn main() -> int {
  const Vec3 value = new Vec3();
  const uni_string rendered = `{value}`;
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  const int assembled = built ? std::system(("gcc -c " + assembly.string() +
                                             " -o " + object.string())
                                                .c_str())
                              : -1;
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("call\tpub_Vec3_toString") != std::string::npos);
  CHECK(text.find("call\tpub_uni_string_cstr") != std::string::npos);
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
shared class Payload {
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
  const auto receiverLoad = transferPrefix.find("movq\t-");
  REQUIRE(receiverLoad != std::string::npos);
  const auto receiverEnd = transferPrefix.find(",%r15", receiverLoad);
  REQUIRE(receiverEnd != std::string::npos);
  const auto receiverSlot = transferPrefix.substr(
      receiverLoad + std::string("movq\t").size(),
      receiverEnd - (receiverLoad + std::string("movq\t").size()));
  CHECK(receiverSlot.find("(%rbp)") != std::string::npos);
  CHECK(transferPrefix.find("lea\t" + receiverSlot + ",%rax") ==
        std::string::npos);
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
  fn init(immutable View&& view) -> Self {
    af_memcpy(my.data, view.data, my.size);
    delete view;
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
  fn init(immutable View&& view) -> Self {
    af_memcpy(my.data, view.data, my.size);
    delete view;
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
    delete view;
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
int af_free(void *pointer) { (void)pointer; return 0; }
int af_memcpy(void *destination, const void *source, int size) {
  unsigned char *out = destination;
  const unsigned char *in = source;
  for (int index = 0; index < size; ++index)
    out[index] = in[index];
  return 0;
}
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

TEST_CASE("return await does not consume a continuation state while probing",
          "[async][return][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_return_await_state_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto object = dir / "main.o";

  std::ofstream(source) << R"(.needs <std>
.needs <Async.gs>
async fn value() -> int { return 7; };
async fn forward() -> int { return await value(); };
fn main() -> int { return 0; };
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  const int assembled = built ? std::system(("gcc -c " + assembly.string() +
                                             " -o " + object.string())
                                                .c_str())
                              : -1;
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(assembled == 0);
  CHECK(text.find("__af_async_state___af_async_body_forward_1:") !=
        std::string::npos);
  CHECK(text.find("__af_async_state___af_async_body_forward_2") ==
        std::string::npos);
}

TEST_CASE("return probing does not discard generic function specializations",
          "[generics][return][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_return_generic_probe_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto object = dir / "main.o";

  std::ofstream(source) << R"(.needs <std>
import {Some, optionWrapper} from "Utils/option" under opt;
unique class Value { fn init() -> Self { return my; }; };
fn wrap(Value &&value) -> Value? { return opt.Some($value); };
fn main() -> int {
  const Value value = new Value();
  const let wrapped = wrap($value);
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  const int assembled = built ? std::system(("gcc -c " + assembly.string() +
                                             " -o " + object.string())
                                                .c_str())
                              : -1;
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(assembled == 0);
  CHECK(text.find("option.Some") != std::string::npos);
  CHECK(
      text.find(
          "pub_option__std__generic__start__Value__std__generic__end___del:") !=
      std::string::npos);
}

TEST_CASE("classes and unions own by default with an explicit shared opt-out",
          "[ownership][defaults][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_ownership_defaults_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
class Resource { fn init() -> Self { return my; }; fn del() -> void { return; }; };
union Choice { Item(Resource), Empty };
shared class SharedResource {
  fn init() -> Self { return my; };
  fn endScope() -> void { return; };
};
fn main() -> int {
  const Resource resource = new Resource();
  const Choice choice = new Choice->Empty();
  const SharedResource shared = new SharedResource();
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(text.find("call\tpub_Resource_del") != std::string::npos);
  CHECK(text.find("pub_Choice_del:") != std::string::npos);
  CHECK(text.find("call\tpub_SharedResource_endScope") != std::string::npos);
}

TEST_CASE("union lifecycle dispatch covers scope delete and nested payloads",
          "[union][ownership][lifecycle][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_union_lifecycle_dispatch_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto object = dir / "main.o";

  std::ofstream(source) << R"(.needs <std>
class Payload {
  fn init() -> Self { return my; };
  fn del() -> void { return; };
};

union Inner {
  Resource(Payload),
  Primitive(int)
};

union Outer {
  Nested(Inner),
  Primitive(int)
};

fn scoped() -> void {
  const Outer value = new Outer->Nested(
    new Inner->Resource(new Payload()));
  return;
};

fn main() -> int {
  scoped();
  const Outer value = new Outer->Primitive(7);
  delete value;
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  const int assembled = built ? std::system(("gcc -c " + assembly.string() +
                                             " -o " + object.string())
                                                .c_str())
                              : -1;
  fs::remove_all(dir);

  REQUIRE(built);
  CHECK(assembled == 0);
  const auto countCalls = [&](const std::string &needle) {
    std::size_t count = 0;
    for (std::size_t pos = 0;
         (pos = text.find(needle, pos)) != std::string::npos;
         pos += needle.size())
      ++count;
    return count;
  };
  CHECK(text.find("pub_Inner_del:") != std::string::npos);
  CHECK(text.find("pub_Outer_del:") != std::string::npos);
  CHECK(text.find("call\tpub_Payload_del") != std::string::npos);
  CHECK(text.find("call\tpub_Inner_del") != std::string::npos);
  CHECK(countCalls("call\tpub_Outer_del") == 2);
  CHECK(text.find("call\taf_free") != std::string::npos);
}

TEST_CASE("explicit delete releases an owned class-valued field shell",
          "[class][ownership][delete][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_class_field_delete_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
shared safe class Resource {
  fn init() -> Self { return my; };
  fn del() -> void { return; };
};

unique class Owner {
  private Resource resource = new Resource();
  fn init() -> Self { return my; };
  fn del() -> void {
    delete my.resource;
  };
};

fn main() -> int {
  const Owner owner = new Owner();
  delete owner;
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  const auto destructor = text.find("pub_Owner_del:");
  REQUIRE(destructor != std::string::npos);
  const auto resourceDelete = text.find("call\tpub_Resource_del", destructor);
  REQUIRE(resourceDelete != std::string::npos);
  const auto freeShell = text.find("call\taf_free", resourceDelete);
  REQUIRE(freeShell != std::string::npos);
  CHECK(freeShell - resourceDelete < 500);
}

TEST_CASE("owned chained union receivers are cleaned after primitive results",
          "[union][ownership][temporary][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_union_chain_cleanup_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
unique class Payload {
  fn init() -> Self { return my; };
};

unique union Mixed {
  Number(int),
  Item(Payload)

  safe fn read() -> int {
    match my {
      Number(value) => return value,
      Item() => return 0
    };
  };
};

fn main() -> int {
  return new Mixed->Number(7).read() - 7;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  const auto mainStart = text.find("main:");
  REQUIRE(mainStart != std::string::npos);
  const auto readCall = text.find("call\tpub_Mixed_read", mainStart);
  REQUIRE(readCall != std::string::npos);
  const auto delCall = text.find("call\tpub_Mixed_del", readCall);
  REQUIRE(delCall != std::string::npos);
  const auto freeCall = text.find("call\taf_free", delCall);
  CHECK(freeCall != std::string::npos);
}

TEST_CASE("inline vector moves release source shells and destroy owned fields",
          "[vector][ownership][transfer][regression]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/compiler_vector_inline_move_regression");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";

  std::ofstream(source) << R"(.needs <std>
import vector from "Collections/Vector";

unique class Child {
  int value = value;
  fn init(int value) -> Self { return my; };
};

unique class Item {
  Child child = $child;
  fn init(Child &&child) -> Self { return my; };
};

fn main() -> int {
  let items = new vector::<Item>();
  items.push_back(new Item(new Child(7)));
  items.set(0, new Item(new Child(8)));
  items.insert(0, new Item(new Child(9)));
  delete items;
  return 0;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const auto text = built ? readFile(assembly) : std::string();
  fs::remove_all(dir);

  REQUIRE(built);
  const auto itemDel = text.find("pub_Item_del:");
  const auto itemInvalidate = text.find("pub_Item___invalidate__:", itemDel);
  REQUIRE(itemDel != std::string::npos);
  REQUIRE(itemInvalidate != std::string::npos);
  const auto itemDelBody = text.substr(itemDel, itemInvalidate - itemDel);
  const auto firstFieldLoad = itemDelBody.find("movq\t0(%r14),%r15");
  REQUIRE(firstFieldLoad != std::string::npos);
  CHECK(itemDelBody.find("movq\t0(%r14),%r15", firstFieldLoad + 1) !=
        std::string::npos);
  CHECK(itemDelBody.find("call\taf_free") != std::string::npos);

  const auto push = text.find(
      "pub_vector__std__generic__start__Item__std__generic__end___push_back:");
  REQUIRE(push != std::string::npos);
  const auto transfer = text.find("call\tpub_Item___transfer_to__", push);
  REQUIRE(transfer != std::string::npos);
  const auto releaseShell = text.find("call\taf_free", transfer);
  CHECK(releaseShell != std::string::npos);
  const auto set = text.find(
      "pub_vector__std__generic__start__Item__std__generic__end___set:");
  REQUIRE(set != std::string::npos);
  const auto setTransfer = text.find("call\tpub_Item___transfer_to__", set);
  REQUIRE(setTransfer != std::string::npos);
  CHECK(text.find("call\taf_free", setTransfer) != std::string::npos);
  const auto insert = text.find(
      "pub_vector__std__generic__start__Item__std__generic__end___insert:");
  REQUIRE(insert != std::string::npos);
  const auto insertTransfer =
      text.find("call\tpub_Item___transfer_to__", insert);
  REQUIRE(insertTransfer != std::string::npos);
  CHECK(text.find("call\taf_free", insertTransfer) != std::string::npos);
  const auto vectorDel = text.find(
      "pub_vector__std__generic__start__Item__std__generic__end___del:");
  REQUIRE(vectorDel != std::string::npos);
  CHECK(text.find("call\tpub_Item_del", vectorDel) != std::string::npos);
}
