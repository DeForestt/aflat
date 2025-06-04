#include "catch.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"

TEST_CASE("ScopeManager assign and get", "[scopemanager]") {
    auto sm = gen::scope::ScopeManager::getInstance();
    sm->reset();
    ast::Type t; t.size = asmc::DWord; t.arraySize = 1;
    int offset = sm->assign("x", t, false);
    REQUIRE(offset == 4);
    auto sym = sm->get("x");
    REQUIRE(sym != nullptr);
    REQUIRE(sym->symbol == "x");
    REQUIRE(sm->getStackAlignment() == 16);
}
