#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "Configs.hpp"
#include "ErrorReporter.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

namespace {

struct BuildResult {
  bool success = false;
  std::vector<std::string> diagnostics;
};

BuildResult buildSinkProgram(const std::string &name,
                             const std::string &source) {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp") / name;
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto input = dir / "main.af";
  const auto output = dir / "main.s";
  std::ofstream(input) << source;

  BuildResult result;
  {
    error::DiagnosticCaptureScope capture(
        [&](const std::string &, int, const std::string &message,
            const std::string &,
            bool) { result.diagnostics.push_back(message); });
    result.success =
        build(input.string(), output.string(), cfg::Mutability::Strict, false);
  }

  fs::remove_all(dir);
  return result;
}

bool hasDiagnostic(const BuildResult &result, const std::string &text) {
  for (const auto &diagnostic : result.diagnostics) {
    if (diagnostic.find(text) != std::string::npos)
      return true;
  }
  return false;
}

std::string diagnosticsText(const BuildResult &result) {
  std::string output;
  for (const auto &diagnostic : result.diagnostics)
    output += diagnostic + "\n";
  return output;
}

const char *valueClass = R"(
.needs <std>
unique class Value {
  int number = number;

  fn init(int number) -> Self { return my; };
  safe fn read() -> int { return my.number; };
  safe fn view() -> loan Self { return my; };
  sink fn finish() -> int { return my.number; };
  sink fn keep() -> Self { return my; };
  sink fn sell() -> Self { return $my; };
};
)";

} // namespace

TEST_CASE("ordinary methods borrow their receiver", "[owned][sink][receiver]") {
  const auto result =
      buildSinkProgram("sink_borrowed_receiver", std::string(valueClass) + R"(
fn main() -> int {
  let value = new Value(7);
  let first = value.read();
  return value.read() - first;
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
}

TEST_CASE("owned temporaries may fall back to borrowing overloads",
          "[owned][sink][receiver][overload]") {
  const auto result = buildSinkProgram("sink_temporary_borrow_fallback",
                                       std::string(valueClass) + R"(
fn main() -> int {
  return new Value(7).read() - 7;
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
}

TEST_CASE("explicit receiver sales require a sink overload",
          "[owned][sink][receiver][overload]") {
  const auto result = buildSinkProgram("sink_explicit_no_fallback",
                                       std::string(valueClass) + R"(
fn main() -> int {
  let value = new Value(7);
  return $value.read();
};
)");

  CHECK_FALSE(result.success);
  CHECK(hasDiagnostic(result, "requires a compatible sink overload"));
}

TEST_CASE("sink methods require an explicit receiver sale",
          "[owned][sink][receiver]") {
  const auto missingSale =
      buildSinkProgram("sink_requires_sale", std::string(valueClass) + R"(
fn main() -> int {
  let value = new Value(7);
  let result = value.finish();
  return result;
};
)");
  const auto soldReceiver =
      buildSinkProgram("sink_sells_receiver", std::string(valueClass) + R"(
fn main() -> int {
  let value = new Value(7);
  let result = $value.finish();
  return value.read() - result;
};
)");

  CHECK_FALSE(missingSale.success);
  CHECK(hasDiagnostic(missingSale, "requires ownership transfer"));
  CHECK_FALSE(soldReceiver.success);
  CHECK(hasDiagnostic(soldReceiver, "variable value was sold"));
}

TEST_CASE("sink methods can return or sell my to preserve ownership",
          "[owned][sink][receiver]") {
  const auto returned =
      buildSinkProgram("sink_returns_my", std::string(valueClass) + R"(
fn main() -> int {
  let value = new Value(7);
  let kept = $value.keep();
  return kept.read() - 7;
};
)");
  const auto sold =
      buildSinkProgram("sink_sells_my", std::string(valueClass) + R"(
fn main() -> int {
  let value = new Value(7);
  let kept = $value.sell();
  return kept.read() - 7;
};
)");

  INFO(diagnosticsText(returned));
  CHECK(returned.success);
  INFO(diagnosticsText(sold));
  CHECK(sold.success);
}

TEST_CASE("owned results can chain into sink methods",
          "[owned][sink][receiver]") {
  const auto temporary =
      buildSinkProgram("sink_owned_temporary", std::string(valueClass) + R"(
fn main() -> int {
  return new Value(7).finish() - 7;
};
)");
  const auto returned =
      buildSinkProgram("sink_owned_result_chain", std::string(valueClass) + R"(
fn main() -> int {
  let value = new Value(7);
  return $value.keep().finish() - 7;
};
)");

  INFO(diagnosticsText(temporary));
  CHECK(temporary.success);
  INFO(diagnosticsText(returned));
  CHECK(returned.success);
}

TEST_CASE("return conversion evaluates a sold sink receiver once",
          "[owned][sink][receiver][return]") {
  const auto result = buildSinkProgram("sink_return_result_conversion", R"(
.needs <std>
import {reject} from "Utils/result" under res;
import {resultWrapper} from "Utils/result" under result;
import Error from "Utils/Error";

unique class Value {
  fn init() -> Self { return my; };
};

fn make() -> Value! {
  let outcome = res.reject::<Value>(new Error("failed"));
  if outcome.isErr() {
    return $outcome.unwrapErr();
  };
  return $outcome.unwrap();
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
}

TEST_CASE("bubble owns variants extracted from an owned result",
          "[owned][bubble][result]") {
  const auto result = buildSinkProgram("bubble_owned_result_variants", R"(
.needs <std>
import {reject, resultWrapper} from "Utils/result" under result;
import Error from "Utils/Error";

unique class Value {
  fn init() -> Self { return my; };
};

fn fail() -> Value! {
  return new Error("failed");
};

fn propagate() -> Value! {
  return fail()!;
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
}

TEST_CASE("loaned results cannot chain into sink methods",
          "[owned][sink][receiver]") {
  const auto result =
      buildSinkProgram("sink_loaned_result", std::string(valueClass) + R"(
fn main() -> int {
  let value = new Value(7);
  return value.view().finish();
};
)");

  CHECK_FALSE(result.success);
  CHECK(hasDiagnostic(result, "requires an owned receiver"));
}

TEST_CASE("receiver ownership selects between otherwise identical overloads",
          "[owned][sink][receiver][overload]") {
  const auto borrowingFirst = buildSinkProgram("sink_receiver_overload", R"(
.needs <std>
class BorrowedResult {};
class SunkResult {};
unique class Values {
  fn init() -> Self { return my; };
  fn push_back(int value) -> BorrowedResult {
    return new BorrowedResult();
  };
  sink fn push_back(int value) -> SunkResult {
    return new SunkResult();
  };
};
fn main() -> int {
  let borrowedValues = new Values();
  const BorrowedResult borrowed = borrowedValues.push_back(1);
  let sunkValues = new Values();
  const SunkResult sunk = $sunkValues.push_back(2);
  return 0;
};
)");

  INFO(diagnosticsText(borrowingFirst));
  CHECK(borrowingFirst.success);
}

TEST_CASE("receiver overload selection does not depend on declaration order",
          "[owned][sink][receiver][overload]") {
  const auto sinkingFirst = buildSinkProgram("sink_receiver_overload_order", R"(
.needs <std>
class BorrowedResult {};
class SunkResult {};
unique class Values {
  fn init() -> Self { return my; };
  sink fn push_back(int value) -> SunkResult {
    return new SunkResult();
  };
  fn push_back(int value) -> BorrowedResult {
    return new BorrowedResult();
  };
};
fn main() -> int {
  let borrowedValues = new Values();
  const BorrowedResult borrowed = borrowedValues.push_back(1);
  let sunkValues = new Values();
  const SunkResult sunk = $sunkValues.push_back(2);
  return 0;
};
)");

  INFO(diagnosticsText(sinkingFirst));
  CHECK(sinkingFirst.success);
}

TEST_CASE("vector push_back exposes borrowing and sinking overloads",
          "[owned][sink][receiver][overload][vector]") {
  const auto result = buildSinkProgram("vector_sink_receiver_overload", R"(
.needs <std>
import vector from "Collections/Vector";
fn main() -> int {
  let borrowed = new vector::<int>();
  borrowed.push_back(1);

  let transferred = new vector::<int>();
  const vector::<int> consumed = $transferred.push_back(2);
  return consumed.count() - 1;
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
}

TEST_CASE("ordinary methods cannot return their borrowed receiver",
          "[owned][sink][receiver]") {
  const auto result = buildSinkProgram("borrowed_receiver_return",
                                       R"(
.needs <std>
unique class Value {
  fn init() -> Self { return my; };
  fn keep() -> Self { return my; };
};
fn main() -> int {
  let value = new Value();
  let kept = value.keep();
  return 0;
};
)");

  CHECK_FALSE(result.success);
  CHECK(hasDiagnostic(result, "cannot return a non-owned reference"));
}

TEST_CASE("sink is rejected on free functions", "[owned][sink][receiver]") {
  const auto result = buildSinkProgram("sink_free_function",
                                       R"(
.needs <std>
sink fn invalid() -> int { return 0; };
fn main() -> int { return invalid(); };
)");

  CHECK_FALSE(result.success);
  CHECK(hasDiagnostic(result, "sink can only be used on class methods"));
}
