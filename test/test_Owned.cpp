#include <filesystem>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

TEST_CASE("CallExpr ownership", "[owned]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  ast::Function func;
  func.ident.ident = "foo";
  func.type = ast::Type("int", asmc::DWord);
  gen.nameTable.push(func);

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

TEST_CASE("Assignments transfer ownership to destination", "[owned]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());
  auto *scope = gen::scope::ScopeManager::getInstance();
  scope->reset();

  ast::Type t;
  t.typeName = "Foo";
  t.size = asmc::QWord;

  scope->assign("dst", t, false);
  scope->assign("src", t, false);

  auto *srcSym = scope->get("src");
  REQUIRE(srcSym != nullptr);
  srcSym->owned = true;

  auto *rhs = new ast::Var();
  rhs->Ident = "src";
  rhs->logicalLine = 1;

  ast::Assign assign;
  assign.Ident = "dst";
  assign.expr = rhs;
  assign.logicalLine = 1;

  assign.generate(gen);

  auto *dstSym = scope->get("dst");
  REQUIRE(dstSym != nullptr);
  CHECK(dstSym->owned);

  scope->reset();
}
