#include <filesystem>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include "catch.hpp"

TEST_CASE("buy operator allocates copy and marks symbol sold", "[sell]") {
  auto parser = parse::Parser();
  gen::CodeGenerator gen("mod", parser, "",
                         std::filesystem::current_path().string());
  gen::scope::ScopeManager::getInstance()->reset();

  ast::Type t;
  t.typeName = "int";
  t.size = asmc::DWord;
  t.arraySize = 1;
  gen::scope::ScopeManager::getInstance()->assign("a", t, false);

  auto var = new ast::Var();
  var->Ident = "a";
  var->logicalLine = 1;
  var->modList = links::LinkedList<std::string>();

  auto buy = new ast::Buy();
  buy->expr = var;
  buy->logicalLine = 1;

  asmc::File file;
  auto res = gen.GenExpr(buy, file);

  auto sym = gen::scope::ScopeManager::getInstance()->get("a");
  REQUIRE(sym->sold != -1);
  REQUIRE(res.size == asmc::QWord);
  REQUIRE_FALSE(res.access.empty());
}
