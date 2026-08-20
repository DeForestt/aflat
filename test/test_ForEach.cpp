#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

#include "Configs.hpp"
#include "ErrorReporter.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

namespace {

std::string asyncIteratorProgram(bool asyncMain) {
  return R"(.needs <std>
import option from "Utils/option";

class AsyncIterator {
    private mutable int index = 0;

    async fn next() -> option::<int> {
        if my.index == 0 {
            my.index = 1;
            return new option::<int>->Some(7);
        };
        return new option::<int>->None();
    };
};
)" + std::string(asyncMain ? "async " : "") +
         R"(fn main() -> int {
    let iterator = new AsyncIterator();
    mutable int total = 0;
)" +
         std::string(asyncMain ? "    foreach value in iterator {\n"
                               : "    async foreach value in iterator {\n") +
         R"(
        total = total + value;
    };
    return total - 7;
};
)";
}

std::string buildAsyncIteratorProgram(const std::string &name, bool asyncMain) {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp") / name;
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / (name + ".af");
  const auto output = dir / (name + ".s");

  std::ofstream(source) << asyncIteratorProgram(asyncMain);
  REQUIRE(
      build(source.string(), output.string(), cfg::Mutability::Strict, false));

  std::ifstream assemblyFile(output);
  const std::string assembly((std::istreambuf_iterator<char>(assemblyFile)),
                             std::istreambuf_iterator<char>());
  fs::remove_all(dir);
  return assembly;
}

} // namespace

TEST_CASE("async foreach runs iterator tasks in a synchronous function",
          "[foreach][async]") {
  const auto assembly =
      buildAsyncIteratorProgram("foreach_async_iterator_sync", false);

  REQUIRE(assembly.find("\tcall\taf_task_run\n") != std::string::npos);
}

TEST_CASE("asynchronous foreach awaits an async iterator task",
          "[foreach][async]") {
  const auto assembly =
      buildAsyncIteratorProgram("foreach_async_iterator_async", true);

  REQUIRE(assembly.find("\tcall\taf_task_await_suspend\n") !=
          std::string::npos);
}

TEST_CASE(
    "synchronous foreach clearly requires async foreach for async iterators",
    "[foreach][async][diagnostics]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/foreach_async_iterator_missing_async");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "missing_async.af";
  const auto output = dir / "missing_async.s";
  auto program = asyncIteratorProgram(false);
  const auto explicitLoop = program.find("async foreach");
  REQUIRE(explicitLoop != std::string::npos);
  program.erase(explicitLoop, std::string("async ").size());
  std::ofstream(source) << program;

  std::string diagnostic;
  error::DiagnosticCaptureScope capture(
      [&](const std::string &, int, const std::string &message,
          const std::string &, bool) { diagnostic = message; });
  REQUIRE_FALSE(
      build(source.string(), output.string(), cfg::Mutability::Strict, false));

  CHECK(diagnostic.find("cannot consume async iterator type `AsyncIterator` "
                        "from a synchronous function") != std::string::npos);
  CHECK(diagnostic.find("Write `async foreach value in ...`") !=
        std::string::npos);
  fs::remove_all(dir);
}
