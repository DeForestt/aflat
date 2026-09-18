#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "Configs.hpp"
#include "Parser/AST/Statements/Function.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);
bool runConfig(cfg::Config &config, const std::string &libPath, char pmode);
std::string getExePath();

TEST_CASE("float return metadata follows the effective wrapped type",
          "[call][float][return]") {
  parse::Parser parser;
  test::mockGen::CodeGenerator generator(
      "mod", parser, "", std::filesystem::current_path().string());
  ast::Function function;
  function.type = ast::Type("float", asmc::DWord);
  std::string expectedType;
  SECTION("async scalar") {
    function.isAsync = true;
    expectedType = "task<float>";
  }
  SECTION("async option") {
    function.isAsync = function.optional = true;
    expectedType = "task<option<float>>";
  }
  SECTION("async result") {
    function.isAsync = function.error = true;
    expectedType = "task<result<float>>";
  }
  SECTION("loaned optional payload") {
    function.optional = function.returnPayloadLoan = true;
    expectedType = "option<&float>";
  }
  SECTION("loaned result payload") {
    function.error = function.returnPayloadLoan = true;
    expectedType = "result<&float>";
  }
  SECTION("effective scalar type") {
    function.type = ast::Type("T", asmc::DWord);
    function.useType = ast::Type("float", asmc::DWord);
    const auto result = function.toExpr(generator);
    CHECK(result.type == "float");
    CHECK(result.access == "%xmm0");
    CHECK(result.size == asmc::DWord);
    CHECK(result.op == asmc::Float);
    return;
  }
  const auto result = function.toExpr(generator);
  CHECK(result.type == expectedType);
  CHECK(result.access == "%rax");
  CHECK(result.size == asmc::QWord);
  CHECK(result.op == asmc::Hard);
}

TEST_CASE("wrapped float returns preserve their object representation",
          "[call][float][return][runtime]") {
  namespace fs = std::filesystem;
  struct RestoreWorkingDirectory {
    fs::path previous = fs::current_path();
    ~RestoreWorkingDirectory() { fs::current_path(previous); }
  } restoreWorkingDirectory;
  fs::current_path(fs::path(getExePath()).parent_path().parent_path());

  std::string functions;
  std::string body;
  SECTION("local optional float") {
    functions = R"(fn sample(bool present) -> local float? {
  if !present return opt.None::<float>();
  return 1.5;
};
)";
  }
  SECTION("heap optional float") {
    functions = R"(fn sample(bool present) -> float? {
  if !present return opt.None::<float>();
  return 1.5;
};
)";
  }
  SECTION("explicit local option of float") {
    functions = R"(fn sample(bool present) -> local option::<float> {
  if !present return opt.None::<float>();
  return opt.Some::<float>(1.5);
};
)";
  }
  SECTION("result of float") {
    std::string storage;
    SECTION("local") { storage = "local "; }
    SECTION("heap") { storage = ""; }
    functions = "fn sample(bool present) -> " + storage + R"(float! {
  if !present return res.reject::<float>(new Error("missing"));
  return 1.5;
};
)";
    body = R"(  let value = sample(true);
  match value {
    Ok(v) => { if v != 1.5 return 1; },
    Err(e) => return 2,
  };
  let missing = sample(false);
  match missing { Ok(v) => return 3, Err(e) => {} };
  match sample(true) {
    Ok(v) => { if v != 1.5 return 4; },
    Err(e) => return 5,
  };
)";
  }
  SECTION("async option of float") {
    functions = R"(async fn sample() -> option::<float> {
  return opt.Some::<float>(1.5);
};
async fn forward() -> option::<float> { return await sample(); };
)";
    body = R"(  let pending = sample();
  let value = run pending;
  match value {
    Some(v) => { if v != 1.5 return 1; }, None => return 2,
  };
  match run forward() {
    Some(v) => { if v != 1.5 return 3; }, None => return 4,
  };
)";
  }
  SECTION("async result of float") {
    functions = R"(async fn sample() -> result::<float> {
  return res.accept::<float>(1.5);
};
async fn forward() -> result::<float> { return await sample(); };
)";
    body = R"(  let pending = sample();
  let value = run pending;
  match value {
    Ok(v) => { if v != 1.5 return 1; }, Err(e) => return 2,
  };
  match run forward() {
    Ok(v) => { if v != 1.5 return 3; }, Err(e) => return 4,
  };
)";
  }
  if (body.empty()) {
    body = R"(  let value = sample(true);
  match value {
    Some(v) => { if v != 1.5 return 1; },
    None => return 2,
  };
  let missing = sample(false);
  match missing { Some(v) => return 3, None => {} };
  match sample(true) {
    Some(v) => { if v != 1.5 return 4; },
    None => return 5,
  };
  match sample(false) { Some(v) => return 6, None => {} };
)";
  }

  const auto dir = fs::path("tmp/wrapped_float_returns");
  fs::remove_all(dir);
  fs::create_directories(dir);
  std::ofstream(dir / "main.af") << R"(.needs <std>
.needs <Async.gs>
import {option, Some, None, optionWrapper} from "Utils/option" under opt;
import {result, accept, reject, resultWrapper} from "Utils/result" under res;
import Error from "Utils/Error";
)" << functions << "fn main() -> int {\n"
                                 << body << "  return 0;\n};\n";

  cfg::Config config;
  config.entryPoint = "../" + (dir / "main").string();
  config.outPutFile = (dir / "main").string();
  REQUIRE(runConfig(config, "libraries/std/", 'e'));
  CHECK(std::system(config.outPutFile.c_str()) == 0);
  fs::remove_all(dir);
}

TEST_CASE("float arguments and return values survive nested calls",
          "[call][float]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/float_calls");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "float_calls.af";
  const auto assembly = dir / "float_calls.s";
  const auto executable = dir / "float_calls";

  std::ofstream(source) << R"(fn echo(const float value) -> float {
  return value;
};

fn add(const float left, const float right) -> float {
  return left + right;
};

fn main() -> int {
  const let actual = echo(add(1.25, 2.25));
  if actual == 3.5 {
    return 0;
  };
  return 1;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  int linked = -1;
  int ran = -1;
  if (built) {
    const auto linkCommand =
        "gcc -no-pie " + assembly.string() + " -o " + executable.string();
    linked = std::system(linkCommand.c_str());
    if (linked == 0)
      ran = std::system(executable.string().c_str());
  }

  fs::remove_all(dir);
  REQUIRE(built);
  REQUIRE(linked == 0);
  REQUIRE(ran == 0);
}

TEST_CASE("calls pass arguments beyond six through stack slots",
          "[call][stack-arguments]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/stack_arguments");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "stack_arguments.af";
  const auto assembly = dir / "stack_arguments.s";
  const auto executable = dir / "stack_arguments";

  std::ofstream(source)
      << R"(fn sum(const int one, const int two, const int three, const int four, const int five, const int six, const int seven, const int eight, const int nine) -> int {
  return one + two + three + four + five + six + seven + eight + nine;
};

fn main() -> int {
  if sum(1, 2, 3, 4, 5, 6, 7, 8, 9) == 45 {
    return 0;
  };
  return 1;
};
 )";

  const bool built = build(source.string(), assembly.string(),
                           cfg::Mutability::Promiscuous, false);
  const auto text = built ? [&] {
    std::ifstream input(assembly);
    return std::string(std::istreambuf_iterator<char>(input),
                       std::istreambuf_iterator<char>());
  }() : std::string();
  const int linked = built ? std::system(("gcc -no-pie " + assembly.string() +
                                          " -o " + executable.string())
                                             .c_str())
                           : -1;
  const int ran = linked == 0 ? std::system(executable.string().c_str()) : -1;

  fs::remove_all(dir);
  REQUIRE(built);
  REQUIRE(text.find("16(%rbp)") != std::string::npos);
  REQUIRE(text.find("24(%rbp)") != std::string::npos);
  REQUIRE(linked == 0);
  REQUIRE(ran == 0);
}

TEST_CASE("chained float arithmetic preserves every intermediate result",
          "[codegen][float]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/chained_float_arithmetic");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "main.af";
  const auto assembly = dir / "main.s";
  const auto executable = dir / "main";

  std::ofstream(source) << R"(fn main() -> int {
  const float dot = (1.5 * 4.0) + (-2.0 * 0.5) + (3.0 * -2.0);
  if dot == -1.0 return 0;
  return 1;
};
)";

  const bool built =
      build(source.string(), assembly.string(), cfg::Mutability::Strict, false);
  const int linked = built ? std::system(("gcc -no-pie " + assembly.string() +
                                          " -o " + executable.string())
                                             .c_str())
                           : -1;
  const int ran = linked == 0 ? std::system(executable.string().c_str()) : -1;
  fs::remove_all(dir);

  REQUIRE(built);
  REQUIRE(linked == 0);
  CHECK(ran == 0);
}
