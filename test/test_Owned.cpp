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
