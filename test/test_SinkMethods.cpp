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
  std::string assembly;
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
  if (result.success) {
    std::ifstream generated(output);
    result.assembly.assign(std::istreambuf_iterator<char>(generated),
                           std::istreambuf_iterator<char>());
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

TEST_CASE("owning unions require explicit nonprimitive payload transfer",
          "[union][ownership][transfer]") {
  const auto borrowed = buildSinkProgram("union_unsold_payload", R"(
.needs <std>
unique class Payload {
  fn init() -> Self { return my; };
};
unique union Choice { Item(Payload) };
fn wrap(const Payload value) -> Choice {
  return new Choice->Item(value);
};
)");

  CHECK_FALSE(borrowed.success);
  CHECK(hasDiagnostic(borrowed, "cannot store unsold nonprimitive value"));

  const auto copied = buildSinkProgram("union_copied_payload", R"(
.needs <std>
unique class Payload {
  int value = value;
  fn init(const int value) -> Self { return my; };
  fn __copy__() -> Payload { return new Payload(my.value); };
};
unique union Choice { Item(Payload) };
fn wrap(const Payload value) -> Choice {
  return new Choice->Item(value);
};
fn main() -> int {
  const Payload value = new Payload(7);
  const Choice wrapped = wrap(value);
  return 0;
};
)");

  INFO(diagnosticsText(copied));
  CHECK(copied.success);

  const auto transferred = buildSinkProgram("union_sold_payload", R"(
.needs <std>
unique class Payload {
  fn init() -> Self { return my; };
};
unique union Choice { Item(Payload) };
fn wrap(const Payload &&value) -> Choice {
  return new Choice->Item($value);
};
fn main() -> int {
  const Choice direct = new Choice->Item(new Payload());
  const Choice wrapped = wrap(new Payload());
  return 0;
};
)");

  INFO(diagnosticsText(transferred));
  CHECK(transferred.success);
}

TEST_CASE("loaned generic union payloads store non-owning references",
          "[union][ownership][loan][generics]") {
  const auto borrowed = buildSinkProgram("union_loan_payload", R"(
.needs <std>
import {Some, None} from "Utils/option" under opt;
import option from "Utils/option";

unique class Payload {
  int value = value;
  fn init(const int value) -> Self { return my; };
  fn read() -> int { return my.value; };
};

fn inspect(const Payload value) -> int {
  const option::<&Payload> wrapped = opt.Some::<&Payload>(value);
  match wrapped {
    Some(item) => return item.read(),
    None => return 0
  };
};

fn rewrap(const Payload value) -> option::<&Payload> {
  const option::<&Payload> wrapped = opt.Some::<&Payload>(value);
  return wrapped;
};

fn main() -> int {
  const Payload value = new Payload(7);
  return inspect(value) - 7;
};
)");

  INFO(diagnosticsText(borrowed));
  CHECK(borrowed.success);
  CHECK(borrowed.assembly.find("option.Some.__std__loan__Payload") !=
        std::string::npos);
}

TEST_CASE("loaned generic union payloads cannot outlive local referents",
          "[union][ownership][loan][generics]") {
  const auto escaped = buildSinkProgram("union_loan_payload_escape", R"(
.needs <std>
import {Some} from "Utils/option" under opt;
import option from "Utils/option";

unique class Payload {
  fn init() -> Self { return my; };
};

fn escape() -> option::<&Payload> {
  const Payload local = new Payload();
  const option::<&Payload> wrapped = opt.Some::<&Payload>(local);
  return wrapped;
};

fn main() -> int { return 0; };
)");

  INFO(diagnosticsText(escaped));
  CHECK_FALSE(escaped.success);
  CHECK(hasDiagnostic(escaped, "cannot return a loan whose referent"));
}

TEST_CASE("question and bang return shorthand support loaned payloads",
          "[parser][return][union][loan]") {
  const auto result = buildSinkProgram("loan_payload_return_shorthand", R"(
.needs <std>
import option from "Utils/option";
import {optionWrapper} from "Utils/option" under option;
import result from "Utils/result";
import {resultWrapper, reject} from "Utils/result" under result;
import Error from "Utils/Error";

unique class Value {
  int number = number;
  fn init(const int number) -> Self { return my; };
};

fn find(const Value value, bool found) -> &Value? {
  if found { return value; };
  return;
};

fn load(const Value value, bool loaded) -> &Value! {
  if loaded { return value; };
  return new Error("not loaded");
};

fn main() -> int {
  const Value value = new Value(7);
  const option::<&Value> found = find(value, true);
  const result::<&Value> loaded = load(value, true);
  return 0;
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
  CHECK(result.assembly.find("option.optionWrapper.__std__loan__Value") !=
        std::string::npos);
  CHECK(result.assembly.find("result.resultWrapper.__std__loan__Value") !=
        std::string::npos);
}

TEST_CASE("loan payload return shorthand rejects local referent escape",
          "[parser][return][union][loan]") {
  const auto result = buildSinkProgram("loan_payload_shorthand_escape", R"(
.needs <std>
import option from "Utils/option";
import {optionWrapper} from "Utils/option" under option;

unique class Value {
  fn init() -> Self { return my; };
};

fn escape() -> &Value? {
  const Value local = new Value();
  return local;
};

fn main() -> int { return 0; };
)");

  CHECK_FALSE(result.success);
  CHECK(hasDiagnostic(result, "cannot return a loan whose referent"));
}

TEST_CASE("async loan payload shorthand preserves referent provenance",
          "[async][parser][return][union][loan]") {
  const auto result = buildSinkProgram("async_loan_payload_escape", R"(
.needs <std>
.needs <Async.gs>
import option from "Utils/option";
import {optionWrapper} from "Utils/option" under option;

unique class Value {
  fn init() -> Self { return my; };
};

async fn borrow(const Value value) -> &Value? {
  return value;
};

async fn escape() -> &Value? {
  const Value local = new Value();
  return await borrow(local);
};

fn main() -> int { return 0; };
)");

  CHECK_FALSE(result.success);
  CHECK(hasDiagnostic(result, "cannot return a loan whose referent"));
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

TEST_CASE("awaited owned results can chain into sink methods",
          "[async][owned][sink][receiver]") {
  const auto result = buildSinkProgram("sink_awaited_owned_result", R"(
.needs <std>
.needs <Async.gs>
import {resultWrapper} from "Utils/result" under result;

unique class Value {
  int number = number;

  fn init(int number) -> Self { return my; };
  safe fn read() -> int { return my.number; };
};

async fn make() -> Value! {
  return new Value(7);
};

async fn consume() -> int {
  let value = (await make()).expect("make failed");
  return value.read() - 7;
};

fn main() -> int { return 0; };
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
}

TEST_CASE("await preserves a loaned async return",
          "[async][loan][sink][receiver]") {
  const auto result = buildSinkProgram("sink_awaited_loan", R"(
.needs <std>
.needs <Async.gs>

unique class Value {
  fn init() -> Self { return my; };
  sink fn finish() -> int { return 7; };
};

async fn borrow(const Value value) -> loan Value {
  return value;
};

async fn consume(const Value value) -> int {
  return (await borrow(value)).finish();
};

fn main() -> int { return 0; };
)");

  CHECK_FALSE(result.success);
  CHECK(hasDiagnostic(result, "requires an owned receiver"));
}

TEST_CASE("consuming match materializes an awaited owned union",
          "[async][owned][match][regression]") {
  const auto result = buildSinkProgram("match_awaited_owned_result", R"(
.needs <std>
.needs <Async.gs>
import {resultWrapper} from "Utils/result" under result;

unique class Value {
  int number = number;
  fn init(const int number) -> Self { return my; };
  safe fn read() -> int { return my.number; };
};

async fn make() -> Value! {
  return new Value(7);
};

async fn consume() -> int {
  let value = match $(await make()) {
    Ok(&&payload) => payload,
    Err => return 1
  };
  return value.read() - 7;
};

fn main() -> int { return 0; };
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
}

TEST_CASE("borrowed call arguments materialize and clean owned temporaries",
          "[owned][call][temporary][regression]") {
  const auto result = buildSinkProgram("borrowed_call_temporary", R"(
.needs <std>

unique class Value {
  fn init() -> Self { return my; };
};

fn make() -> Value { return new Value(); };
fn inspect(const Value value) -> int { return 7; };

fn main() -> int {
  return inspect(make()) - 7;
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
  const auto inspectCall = result.assembly.find("call\tinspect");
  REQUIRE(inspectCall != std::string::npos);
  CHECK(result.assembly.find("call\taf_free", inspectCall) !=
        std::string::npos);
}

TEST_CASE("assignment refreshes root binding after RHS adds temporaries",
          "[owned][assignment][temporary][regression]") {
  const auto result = buildSinkProgram("assignment_binding_refresh", R"(
.needs <std>

unique class Value {
  fn init(const int number) -> Self { return my; };
};

fn identity(const int value) -> int { return value; };
fn make(const int value) -> Value { return new Value(value); };

fn main() -> int {
  mutable Value target = NULL;
  target = make(
      identity(
          identity(
              identity(
                  identity(
                      identity(
                          identity(
                              identity(
                                  identity(7)))))))));
  delete target;
  return 0;
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
  CHECK(result.assembly.find("call\taf_free") != std::string::npos);
}

TEST_CASE("assignment operators resolve borrowed and consuming overloads",
          "[owned][operator][overload][regression]") {
  const auto result = buildSinkProgram("assignment_operator_overloads", R"(
.needs <std>

unique class Box {
  mutable int value = value;
  fn init(const int value) -> Self { return my; };

  loan Box cpy<<=>>(const Box other) {
    my.value = other.value;
    return my;
  };

  loan Box cpy<<=>>(const Box &&other) {
    my.value = other.value;
    delete other;
    return my;
  };

  bool compare<<==>>(const Box other) {
    return my.value == other.value;
  };

  bool compare<<==>>(const Box &&other) {
    const bool same = my.value == other.value;
    delete other;
    return same;
  };
};

fn main() -> int {
  mutable Box target = new Box(0);
  const Box source = new Box(1);
  target = source;
  target = new Box(2);
  const bool borrowedEqual = target == source;
  const bool consumedEqual = target == new Box(2);
  return if borrowedEqual & consumedEqual 0 else 1;
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
  CHECK(result.assembly.find("call\tpub_Box_cpy") != std::string::npos);
  CHECK(result.assembly.find("call\tpub_Box_cpy_ovl1") != std::string::npos);
  CHECK(result.assembly.find("call\tpub_Box_compare") != std::string::npos);
  CHECK(result.assembly.find("call\tpub_Box_compare_ovl1") !=
        std::string::npos);
}

TEST_CASE("explicit transfer retries specialized generic overload families",
          "[owned][generic][overload][regression]") {
  const auto result = buildSinkProgram("generic_sink_overload_retry", R"(
.needs <std>
import string from "String";

fn make() -> string! {
  const string value = new string("ready");
  return $value;
};

fn main() -> int { return 0; };
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
  CHECK(result.assembly.find("call\tresult.resultWrapper_ovl2.string") !=
        std::string::npos);
}

TEST_CASE("overloaded constructors retain owned receivers",
          "[owned][constructor][overload][regression]") {
  const auto result = buildSinkProgram("constructor_receiver_ownership", R"(
.needs <std>
import string from "String";

unique class Value {
  fn init(const int value) -> Self { return my; };

  fn init(const string &&value) -> Self {
    delete value;
    return my;
  };
};

fn main() -> int {
  const Value value = new Value(new string("ready"));
  delete value;
  return 0;
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
  CHECK(result.assembly.find("pub_Value_init_ovl1:") != std::string::npos);
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

TEST_CASE("primitive result extraction borrows and rejects receiver sales",
          "[owned][sink][receiver][result][primitive]") {
  const auto borrowed = buildSinkProgram("result_primitive_borrow", R"(
.needs <std>
import result from "Utils/result";
import {accept} from "Utils/result" under res;

fn main() -> int {
  let outcome = res.accept::<int>(7);
  const int first = outcome.unwrap();
  const int second = outcome.expect("expected result");
  return outcome.expect("expected result"$adr) - first + second - 7;
};
)");
  const auto soldUnwrap = buildSinkProgram("result_primitive_sold_unwrap", R"(
.needs <std>
import result from "Utils/result";
import {accept} from "Utils/result" under res;

fn main() -> int {
  let outcome = res.accept::<int>(7);
  return $outcome.unwrap();
};
)");
  const auto soldStringExpect =
      buildSinkProgram("result_primitive_sold_string_expect", R"(
.needs <std>
import result from "Utils/result";
import {accept} from "Utils/result" under res;

fn main() -> int {
  let outcome = res.accept::<int>(7);
  return $outcome.expect("expected result");
};
)");
  const auto soldExpect = buildSinkProgram("result_primitive_sold_expect", R"(
.needs <std>
import result from "Utils/result";
import {accept} from "Utils/result" under res;

fn main() -> int {
  let outcome = res.accept::<int>(7);
  return $outcome.expect("expected result"$adr);
};
)");

  INFO(diagnosticsText(borrowed));
  CHECK(borrowed.success);
  CHECK_FALSE(soldUnwrap.success);
  CHECK(hasDiagnostic(soldUnwrap, "requires a compatible sink overload"));
  CHECK_FALSE(soldStringExpect.success);
  CHECK(hasDiagnostic(soldStringExpect, "requires a compatible sink overload"));
  CHECK_FALSE(soldExpect.success);
  CHECK(hasDiagnostic(soldExpect, "requires a compatible sink overload"));
}

TEST_CASE("non-primitive result extraction retains its sink overload",
          "[owned][sink][receiver][result][nonprimitive]") {
  const auto sold = buildSinkProgram("result_nonprimitive_sold_unwrap", R"(
.needs <std>
import result from "Utils/result";
import {accept} from "Utils/result" under res;

unique class Value {
  int number = number;
  fn init(int number) -> Self { return my; };
  safe fn read() -> int { return my.number; };
};

fn main() -> int {
  let value = new Value(7);
  let outcome = res.accept::<Value>($value);
  let extracted = $outcome.unwrap();
  return extracted.read() - 7;
};
)");

  INFO(diagnosticsText(sold));
  CHECK(sold.success);
}

TEST_CASE("consuming mixed unions copy primitives and move owned payloads",
          "[owned][sink][receiver][union][mixed]") {
  const auto valid = buildSinkProgram("mixed_union_consuming_match", R"(
.needs <std>

unique class Payload {
  int number = number;
  fn init(int number) -> Self { return my; };
  safe fn read() -> int { return my.number; };
};

unique union Mixed {
  Number(int),
  Item(Payload)

  sink fn consume() -> int {
    mutable int answer = 0;
    match my {
      Number(value) => answer = value,
      Item(&&value) => answer = value.read()
    };
    return answer;
  };
};

fn main() -> int {
  let value = new Mixed->Number(7);
  return $value.consume() - 7;
};
)");
  const auto primitiveMove = buildSinkProgram("mixed_union_primitive_move", R"(
.needs <std>

unique class Payload { fn init() -> Self { return my; }; };
unique union Mixed {
  Number(int),
  Item(Payload)

  sink fn consume() -> int {
    match my {
      Number(&&value) => return value,
      Item() => return 0
    };
  };
};
)");
  const auto soldPrimitiveUnion =
      buildSinkProgram("sold_all_primitive_union_match", R"(
.needs <std>

unique union PrimitiveChoice {
  Number(int),
  Flag(bool)

  sink fn consume() -> int {
    match $my {
      Number(value) => return value,
      Flag(value) => {
        if value { return 1; };
        return 0;
      }
    };
  };
};

fn main() -> int {
  let value = new PrimitiveChoice->Number(7);
  return $value.consume() - 7;
};
)");

  INFO(diagnosticsText(valid));
  CHECK(valid.success);
  INFO(diagnosticsText(soldPrimitiveUnion));
  CHECK(soldPrimitiveUnion.success);
  CHECK_FALSE(primitiveMove.success);
  CHECK(hasDiagnostic(primitiveMove, "cannot carry ownership"));
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
  REQUIRE(result.success);
  const auto propagate = result.assembly.find("propagate:");
  REQUIRE(propagate != std::string::npos);
  const auto nextArm = result.assembly.find(".match_next_", propagate);
  REQUIRE(nextArm != std::string::npos);
  CHECK(result.assembly.substr(propagate, nextArm - propagate)
            .find("call\taf_free") == std::string::npos);
}

TEST_CASE("bubble borrows loaned payloads from owned results",
          "[owned][bubble][result][loan]") {
  const auto result = buildSinkProgram("bubble_loaned_result_payload", R"(
.needs <std>
import {resultWrapper, reject} from "Utils/result" under result;
import Error from "Utils/Error";

unique class Value {
  int number = number;
  fn init(const int number) -> Self { return my; };
  safe fn read() -> int { return my.number; };
};

fn borrow(const Value value) -> &Value! {
  return value;
};

fn forward(const Value value) -> &Value! {
  return borrow(value)!;
};

fn inspect(const Value value) -> int! {
  let borrowed = borrow(value)!;
  return borrowed.read();
};

fn main() -> int { return 0; };
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
}

TEST_CASE("bubbling a named owned result consumes the source variable",
          "[owned][bubble][result][descope]") {
  const auto result = buildSinkProgram("bubble_named_result_consumed", R"(
.needs <std>
import {reject, resultWrapper} from "Utils/result" under result;

fn make() -> int! {
  return 7;
};

fn propagate() -> int! {
  let outcome = make();
  let value = outcome!;
  let reused = outcome.isOk();
  if reused { return value; };
  return 0;
};
)");

  INFO(diagnosticsText(result));
  CHECK_FALSE(result.success);
  CHECK(hasDiagnostic(result, "variable outcome was sold"));
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

TEST_CASE(
    "explicit object transfer to an address does not create a hidden owner",
    "[owned][sink][address][regression]") {
  const auto result = buildSinkProgram("sold_address_no_hidden_owner", R"(
.needs <std>
unique class Payload {
  fn init() -> Self { return my; };
};
fn consume(const Payload &&value) -> int {
  af_free($value);
  return 0;
};
fn main() -> int {
  return consume(new Payload());
};
)");

  INFO(diagnosticsText(result));
  REQUIRE(result.success);
  const auto consumeStart = result.assembly.find("consume:");
  const auto consumeEnd = result.assembly.find("\nmain:", consumeStart);
  REQUIRE(consumeStart != std::string::npos);
  REQUIRE(consumeEnd != std::string::npos);
  const auto consumeBody =
      result.assembly.substr(consumeStart, consumeEnd - consumeStart);
  const auto firstFree = consumeBody.find("call\taf_free");
  REQUIRE(firstFree != std::string::npos);
  CHECK(consumeBody.find("call\taf_free", firstFree + 1) == std::string::npos);
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

TEST_CASE("field ownership state does not leak between generic class methods",
          "[owned][fields][generics][regression]") {
  const auto result = buildSinkProgram("generic_field_ownership_isolation", R"(
.needs <std>
import owned from "Memory";

unique class Payload {
  int number = number;
  fn init(const int number) -> Self { return my; };
  safe fn read() -> int { return my.number; };
};

types(T)
unique class ReassigningOwner {
  private mutable T value = $value;

  fn init(const T &&value) -> Self { return my; };

  fn replace(const T &&next) -> loan T {
    my.value = $next;
    return my.value;
  };

  safe fn view() -> loan T { return my.value; };

  fn del() { delete my.value; };
};

fn main() -> int {
  let standardOwner = new owned::<Payload>(new Payload(7));
  let first = standardOwner.view().read();

  let reassigned = new ReassigningOwner::<Payload>(new Payload(1));
  let replacement = reassigned.replace(new Payload(8)).read();
  let observed = reassigned.view().read();
  return first + replacement + observed - 23;
};
)");

  INFO(diagnosticsText(result));
  CHECK(result.success);
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
