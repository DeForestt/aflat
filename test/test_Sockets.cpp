#include "CodeGenerator/CodeGenerator.hpp"
#include "Configs.hpp"
#include "Parser/Parser.hpp"
#include "catch.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>

bool build(std::string, std::string, cfg::Mutability, bool);
bool runConfig(cfg::Config &, const std::string &, char);
std::string getExePath();

TEST_CASE("temporary receiver addresses survive enclosing calls and bindings",
          "[codegen][ownership][runtime][regression]") {
  namespace fs = std::filesystem;
  struct RestoreDirectory {
    fs::path previous = fs::current_path();
    ~RestoreDirectory() { fs::current_path(previous); }
  } restore;
  fs::current_path(fs::path(getExePath()).parent_path().parent_path());
  const auto directory = fs::path("tmp/socket_receiver_lifetime");
  fs::create_directories(directory);
  std::ofstream(directory / "main.af") << R"(
.needs <std>
import string from "String";
import {str_comp} from "strings" under str;
fn text() -> string { return new string("temporary receiver"); };
fn copy(const adr value) -> string { return new string(value); };
fn exercise() -> int {
    const int before = af_live_blocks();
    const adr borrowed = text().cstr();
    if af_live_blocks() <= before return 3;
    if str.str_comp(borrowed, "temporary receiver") != 1 return 1;
    const let copied = copy(text().cstr());
    if str.str_comp(copied.cstr(), "temporary receiver") != 1 return 2;
    return 0;
};
fn main() -> int {
    const int blocks = af_live_blocks();
    for mutable int i = 0; i < 100; i = i + 1 {
        if exercise() != 0 return 1;
        if af_live_blocks() != blocks return 2;
    };
    return 0;
};
)";
  cfg::Config config;
  config.entryPoint = "../" + (directory / "main").string();
  config.outPutFile = (directory / "main").string();
  REQUIRE(runConfig(config, "libraries/std/", 'e'));
  CHECK(std::system(config.outPutFile.c_str()) == 0);
  fs::remove_all(directory);
}

TEST_CASE("repeated pointer stores do not consume stack space",
          "[codegen][pointer][runtime][regression]") {
  namespace fs = std::filesystem;
  const auto directory = fs::path("tmp/socket_pointer_stores");
  fs::create_directories(directory);
  const auto source = directory / "main.af";
  const auto assembly = directory / "main.s";
  const auto executable = directory / "main";
  std::ofstream(source) << R"(
fn main() -> int {
    const char byte = '\0';
    const adr address = ?byte;
    for mutable int i = 0; i < 2000000; i = i + 1 {
        address =: 'x';
    };
    if (address as char) != 'x' return 1;
    return 0;
};
)";
  REQUIRE(build(source.string(), assembly.string(), cfg::Mutability::Strict,
                false));
  REQUIRE(std::system(("gcc -no-pie " + assembly.string() + " -o " +
                       executable.string())
                          .c_str()) == 0);
  CHECK(std::system(("sh -c 'ulimit -s 1024; exec " + executable.string() + "'")
                        .c_str()) == 0);
  fs::remove_all(directory);
}

TEST_CASE("array reads preserve surrounding comparison and arithmetic operands",
          "[codegen][array][runtime][regression]") {
  namespace fs = std::filesystem;
  const auto directory = fs::path("tmp/socket_array_operands");
  fs::create_directories(directory);
  const auto source = directory / "main.af";
  const auto assembly = directory / "main.s";
  const auto executable = directory / "main";
  std::ofstream(source) << R"(
fn main() -> int {
    mutable int[2] values;
    values[0] = 7;
    values[1] = 0;
    if values[1] != 0 return 1;
    if values[0] + 3 != 10 return 2;
    if values[0] + values[1] != 7 return 3;
    mutable long[2] clock;
    clock[0] = #12345;
    clock[1] = #123000000;
    const long milliseconds = clock[0] * #1000 + clock[1] / #1000000;
    if milliseconds != #12345123 return 4;
    return 0;
};
)";
  REQUIRE(build(source.string(), assembly.string(), cfg::Mutability::Strict,
                false));
  REQUIRE(std::system(("gcc -no-pie " + assembly.string() + " -o " +
                       executable.string())
                          .c_str()) == 0);
  CHECK(std::system(executable.string().c_str()) == 0);
  fs::remove_all(directory);
}

TEST_CASE("callback argument prefixes do not read beyond their signatures",
          "[codegen][function-pointer][regression]") {
  parse::Parser parser;
  gen::CodeGenerator generator("mod", parser, "",
                               std::filesystem::current_path().string());
  ast::Type integer("int", asmc::DWord);
  ast::Type actual("actual~callback~", asmc::QWord);
  actual.fPointerArgs.returnType = &integer;
  actual.fPointerArgs.requiredArgs = 3;
  actual.fPointerArgs.argTypes = {integer, integer, integer};
  generator.TypeList().push(actual);
  for (int prefix : {0, 1, 2}) {
    ast::Type expected("expected~callback~", asmc::QWord);
    expected.fPointerArgs.returnType = &integer;
    expected.fPointerArgs.requiredArgs = prefix;
    expected.fPointerArgs.argTypes.resize(prefix, integer);
    CHECK(generator.canAssign(expected, actual.typeName, "", false, false));
    expected.fPointerArgs.optConvertionIndices = {0};
    CHECK_FALSE(
        generator.canAssign(expected, actual.typeName, "", false, false));
  }
}

TEST_CASE("socket syscall ABI and shim-free AFlat TCP transfers",
          "[socket][runtime]") {
  const auto script =
      std::filesystem::path(getExePath()).parent_path().parent_path() /
      "test/runtime/socket_probe/run.py";
  REQUIRE(std::system(("python3 \"" + script.string() + "\"").c_str()) == 0);
}

TEST_CASE("Aflat HTTP transport preserves server behavior and bounds requests",
          "[http][server][runtime]") {
  const auto script =
      std::filesystem::path(getExePath()).parent_path().parent_path() /
      "test/runtime/http_transport/run.py";
  REQUIRE(std::system(("python3 \"" + script.string() + "\"").c_str()) == 0);
}
