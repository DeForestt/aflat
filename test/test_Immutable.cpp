#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

TEST_CASE("immutable variables are readOnly", "[immutable]") {
  auto sm = gen::scope::ScopeManager::getInstance();
  sm->reset();
  ast::Type t;
  t.typeName = "int";
  t.size = asmc::DWord;
  t.arraySize = 1;
  sm->assign("x", t, false, false, true);
  auto sym = sm->get("x");
  REQUIRE(sym != nullptr);
  CHECK(sym->mutable_ == false);
  CHECK(sym->readOnly == true);
  sm->reset();
}
