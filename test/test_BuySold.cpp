#include <filesystem>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

TEST_CASE("buy marks a symbol as sold", "[buy]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  parser.addType("Foo", asmc::Hard, asmc::QWord, false, true);
  auto foo = new gen::Class();
  foo->Ident = "Foo";
  foo->uniqueType = true;
  gen.addType(foo);

  auto scope = gen::scope::ScopeManager::getInstance();
  scope->reset();
  scope->pushScope(true);
  scope->assign("obj", ast::Type("Foo", asmc::QWord), false);
  auto sym = scope->get("obj");
  sym->owned = true;
  sym->usable = true;

  auto var = new ast::Var();
  var->Ident = "obj";
  var->logicalLine = 1;

  auto buy = new ast::Buy();
  buy->expr = var;
  buy->logicalLine = 1;

  asmc::File output;
  gen.GenExpr(buy, output);

  REQUIRE(sym->sold != -1);

  ast::Function freeFunc;
  freeFunc.ident.ident = "free";
  gen.nameTable().push(freeFunc);

  asmc::File scopeFile;
  gen::scope::ScopeManager::getInstance()->softPop(&gen, scopeFile);
  REQUIRE(scopeFile.text.head == nullptr);

  scope->popScope(&gen, scopeFile);
  REQUIRE(scopeFile.text.head == nullptr);

  scope->reset();
}
