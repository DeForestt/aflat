#include <filesystem>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

TEST_CASE("returning variable marks it sold", "[return]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  parser.addType("Foo", asmc::Hard, asmc::QWord, false, true);
  auto foo = new gen::Class();
  foo->Ident = "Foo";
  foo->uniqueType = true;
  gen.addType(foo);

  ast::Function func;
  func.ident.ident = "func";
  func.type = ast::Type("Foo", asmc::QWord);
  gen.currentFunction = &func;
  gen.inFunction = true;
  gen.returnType = func.type;

  auto scope = gen::scope::ScopeManager::getInstance();
  scope->reset();
  scope->pushScope(true);
  scope->assign("obj", ast::Type("Foo", asmc::QWord), false);
  auto sym = scope->get("obj");
  sym->owned = true;

  auto var = new ast::Var();
  var->Ident = "obj";
  var->logicalLine = 1;

  ast::Return ret;
  ret.expr = var;
  ret.logicalLine = 1;

  ret.generate(gen);
  int soldLine = sym->sold;
  scope->reset();
  REQUIRE(soldLine != -1);
}
