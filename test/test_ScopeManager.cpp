#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

TEST_CASE("ScopeManager assign and get", "[scopemanager]") {
  auto sm = gen::scope::ScopeManager::getInstance();
  sm->reset();
  ast::Type t;
  t.size = asmc::DWord;
  t.arraySize = 1;
  int offset = sm->assign("x", t, false);
  REQUIRE(offset == 4);
  auto sym = sm->get("x");
  REQUIRE(sym != nullptr);
  REQUIRE(sym->symbol == "x");
  REQUIRE(sm->getStackAlignment() == 16);
}

TEST_CASE("ScopeManager isolated scope", "[scopemanager]") {
  auto sm = gen::scope::ScopeManager::getInstance();
  sm->reset();
  ast::Type t;
  t.size = asmc::DWord;
  t.arraySize = 1;
  sm->assign("a", t, false);
  sm->pushIsolated();
  sm->assign("b", t, false);
  REQUIRE(sm->get("a") == nullptr);
  REQUIRE(sm->get("b") != nullptr);
  sm->popIsolated();
  REQUIRE(sm->get("b") == nullptr);
  REQUIRE(sm->get("a") != nullptr);
}
