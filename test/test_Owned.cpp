#include <filesystem>
#include <fstream>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Configs.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("CallExpr ownership", "[owned]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  ast::Function func;
  func.ident.ident = "foo";
  func.type = ast::Type("int", asmc::DWord);
  gen.nameTable().push(func);

  auto call = new ast::CallExpr();
  call->call = new ast::Call();
  call->call->ident = "foo";
  call->call->Args = links::LinkedList<ast::Expr *>();

  asmc::File file;
  auto expr = gen.GenExpr(call, file);
  REQUIRE(expr.owned);
}

TEST_CASE("Var ownership reflects symbol", "[owned]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  gen::scope::ScopeManager::getInstance()->reset();
  ast::Type t;
  t.typeName = "int";
  t.size = asmc::DWord;
  gen::scope::ScopeManager::getInstance()->assign("x", t, false);
  auto sym = gen::scope::ScopeManager::getInstance()->get("x");
  sym->owned = true;

  ast::Var var;
  var.Ident = "x";
  var.logicalLine = 1;

  asmc::File file;
  auto expr = gen.GenExpr(&var, file);
  REQUIRE(expr.owned);
}

TEST_CASE("reference-valued field access is always a loan", "[owned][field]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  auto scope = gen::scope::ScopeManager::getInstance();
  scope->reset();
  scope->pushScope(true);

  auto child = new gen::Class();
  child->Ident = "Child";
  gen.addType(child);

  auto container = new gen::Class();
  container->Ident = "Container";
  gen::Symbol field;
  field.symbol = "child";
  field.type = ast::Type("Child", asmc::QWord);
  field.byteMod = 8;
  field.owned = true;
  container->publicSymbols.push(field);
  gen.addType(container);

  scope->assign("obj", ast::Type("Container", asmc::QWord), false);
  auto obj = scope->get("obj");
  obj->owned = true;
  obj->usable = true;

  ast::Var access;
  access.Ident = "obj";
  access.modList.push("child");
  access.logicalLine = 1;

  asmc::File file;
  const auto expr = gen.GenExpr(&access, file);
  CHECK(expr.type == "Child");
  CHECK_FALSE(expr.owned);

  scope->reset();
}

TEST_CASE("callers cannot buy a field directly", "[owned][field]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  auto scope = gen::scope::ScopeManager::getInstance();
  scope->reset();
  scope->pushScope(true);

  auto child = new gen::Class();
  child->Ident = "Child";
  gen.addType(child);

  auto container = new gen::Class();
  container->Ident = "Container";
  gen::Symbol field;
  field.symbol = "child";
  field.type = ast::Type("Child", asmc::QWord);
  field.byteMod = 8;
  container->publicSymbols.push(field);
  gen.addType(container);

  scope->assign("obj", ast::Type("Container", asmc::QWord), false);
  auto obj = scope->get("obj");
  obj->owned = true;
  obj->usable = true;

  auto access = new ast::Var();
  access->Ident = "obj";
  access->modList.push("child");
  access->logicalLine = 1;
  ast::Buy buy;
  buy.expr = access;
  buy.logicalLine = 1;

  asmc::File file;
  CHECK_THROWS(gen.GenExpr(&buy, file));

  scope->reset();
}

TEST_CASE("ownership-bearing fields reject unowned values", "[owned][field]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  auto scope = gen::scope::ScopeManager::getInstance();
  scope->reset();
  scope->pushScope(true);

  auto child = new gen::Class();
  child->Ident = "Child";
  gen.addType(child);

  auto container = new gen::Class();
  container->Ident = "Container";
  gen::Symbol field;
  field.symbol = "child";
  field.type = ast::Type("Child", asmc::QWord);
  field.byteMod = 8;
  field.mutable_ = true;
  container->publicSymbols.push(field);
  gen.addType(container);

  scope->assign("obj", ast::Type("Container", asmc::QWord), false);
  auto obj = scope->get("obj");
  obj->owned = true;
  obj->usable = true;
  scope->assign("borrowed", ast::Type("Child", asmc::QWord), false);
  auto borrowed = scope->get("borrowed");
  borrowed->owned = false;
  borrowed->usable = true;

  auto value = new ast::Var();
  value->Ident = "borrowed";
  value->logicalLine = 1;
  ast::Assign assign;
  assign.Ident = "obj";
  assign.modList.push("child");
  assign.expr = value;
  assign.logicalLine = 1;

  CHECK_THROWS(assign.generate(gen));

  scope->reset();
}

TEST_CASE("loan fields explicitly accept unowned class references",
          "[owned][field][loan]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  auto scope = gen::scope::ScopeManager::getInstance();
  scope->reset();
  scope->pushScope(true);

  auto child = new gen::Class();
  child->Ident = "Child";
  gen.addType(child);

  auto container = new gen::Class();
  container->Ident = "Container";
  gen::Symbol field;
  field.symbol = "child";
  field.type = ast::Type("Child", asmc::QWord);
  field.type.isLoan = true;
  field.byteMod = 8;
  field.mutable_ = true;
  container->publicSymbols.push(field);
  gen.addType(container);

  scope->assign("obj", ast::Type("Container", asmc::QWord), false);
  auto obj = scope->get("obj");
  obj->owned = true;
  obj->usable = true;
  scope->assign("borrowed", ast::Type("Child", asmc::QWord), false);
  auto borrowed = scope->get("borrowed");
  borrowed->owned = false;
  borrowed->usable = true;

  auto value = new ast::Var();
  value->Ident = "borrowed";
  value->logicalLine = 1;
  ast::Assign assign;
  assign.Ident = "obj";
  assign.modList.push("child");
  assign.expr = value;
  assign.logicalLine = 1;

  CHECK_NOTHROW(assign.generate(gen));
  const auto stored = container->publicSymbols.peek();
  CHECK_FALSE(stored.owned);
  CHECK(borrowed->sold == -1);

  scope->reset();
}

TEST_CASE("plain assignment does not implicitly sell an owned variable",
          "[owned][assign]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  auto scope = gen::scope::ScopeManager::getInstance();
  scope->reset();
  scope->pushScope(true);

  auto valueType = new gen::Class();
  valueType->Ident = "Value";
  gen.addType(valueType);

  scope->assign("source", ast::Type("Value", asmc::QWord), false);
  auto source = scope->get("source");
  source->owned = true;
  source->usable = true;
  scope->assign("target", ast::Type("Value", asmc::QWord), false);
  auto target = scope->get("target");
  target->owned = false;
  target->usable = true;

  auto value = new ast::Var();
  value->Ident = "source";
  value->logicalLine = 1;
  ast::Assign assign;
  assign.Ident = "target";
  assign.expr = value;
  assign.logicalLine = 1;

  CHECK_NOTHROW(assign.generate(gen));
  CHECK(source->sold == -1);
  CHECK(source->owned);
  CHECK_FALSE(target->owned);

  scope->reset();
}

TEST_CASE("sell operator explicitly transfers assignment ownership",
          "[owned][assign]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  auto scope = gen::scope::ScopeManager::getInstance();
  scope->reset();
  scope->pushScope(true);

  auto valueType = new gen::Class();
  valueType->Ident = "Value";
  gen.addType(valueType);

  scope->assign("source", ast::Type("Value", asmc::QWord), false);
  auto source = scope->get("source");
  source->owned = true;
  source->usable = true;
  scope->assign("target", ast::Type("Value", asmc::QWord), false);
  auto target = scope->get("target");
  target->owned = false;
  target->usable = true;

  auto value = new ast::Var();
  value->Ident = "source";
  value->logicalLine = 1;
  auto buy = new ast::Buy();
  buy->expr = value;
  buy->logicalLine = 1;
  ast::Assign assign;
  assign.Ident = "target";
  assign.expr = buy;
  assign.logicalLine = 1;

  CHECK_NOTHROW(assign.generate(gen));
  CHECK(source->sold == 1);
  CHECK(target->owned);
  CHECK(target->sold == -1);

  scope->reset();
}

TEST_CASE("owning methods can explicitly transfer their own fields",
          "[owned][field]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  auto scope = gen::scope::ScopeManager::getInstance();
  scope->reset();
  scope->pushScope(true);

  auto child = new gen::Class();
  child->Ident = "Child";
  gen.addType(child);

  auto container = new gen::Class();
  container->Ident = "Container";
  gen::Symbol field;
  field.symbol = "child";
  field.type = ast::Type("Child", asmc::QWord);
  field.byteMod = 8;
  container->SymbolTable.push(field);
  gen.addType(container);
  gen.scope() = container;

  ast::Function transferMethod;
  transferMethod.ident.ident = "take";
  gen.currentFunction() = &transferMethod;

  scope->assign("my", ast::Type("Container", asmc::QWord), false);
  auto my = scope->get("my");
  my->owned = true;
  my->usable = true;

  auto access = new ast::Var();
  access->Ident = "my";
  access->modList.push("child");
  access->logicalLine = 1;
  ast::Buy buy;
  buy.expr = access;
  buy.logicalLine = 1;

  asmc::File file;
  const auto expr = gen.GenExpr(&buy, file);
  CHECK(expr.type == "Child");
  CHECK(expr.owned);
  CHECK(container->SymbolTable.peek().sold == -1);

  gen.currentFunction() = nullptr;
  gen.scope() = nullptr;
  scope->reset();
}

TEST_CASE("sell checking analyzes conditional branches independently",
          "[owned][branch]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/branch_aware_sell");
  fs::remove_all(dir);
  fs::create_directories(dir);

  const auto validSource = dir / "valid.af";
  const auto invalidSource = dir / "invalid.af";
  std::ofstream(validSource) << R"(.needs <std>
unique class Value { fn init() -> Self { return my; }; };
fn consume(const Value &&value) -> int { delete value; return 0; };
fn choose(const bool first) -> int {
  const Value value = new Value();
  if first {
    return consume($value);
  } else {
    return consume($value);
  };
};
fn main() -> int { return choose(true); };
)";
  std::ofstream(invalidSource) << R"(.needs <std>
unique class Value { fn init() -> Self { return my; }; };
fn consume(const Value &&value) -> int { delete value; return 0; };
fn choose(const bool first) -> int {
  const Value value = new Value();
  if first { consume($value); };
  return consume($value);
};
fn main() -> int { return choose(true); };
)";

  CHECK(build(validSource.string(), (dir / "valid.s").string(),
              cfg::Mutability::Strict, false));
  CHECK_FALSE(build(invalidSource.string(), (dir / "invalid.s").string(),
                    cfg::Mutability::Strict, false));

  fs::remove_all(dir);
}
