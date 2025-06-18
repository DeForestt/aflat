#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"
#include "catch.hpp"

TEST_CASE("namespaceSwap replaces call identifiers", "[namespaceswap]") {
  ast::Call call;
  call.ident = "foo";
  std::unordered_map<std::string, std::string> map;
  map["foo"] = "bar";
  call.namespaceSwap(map);
  REQUIRE(call.imbeddedNamespace == "bar");
}

TEST_CASE("namespaceSwap recurses into sequences", "[namespaceswap]") {
  auto *call = new ast::Call();
  call->ident = "foo";
  auto *seq = new ast::Sequence();
  seq->Statement1 = call;
  seq->Statement2 = nullptr;
  std::unordered_map<std::string, std::string> map;
  map["foo"] = "bar";
  seq->namespaceSwap(map);
  REQUIRE(call->imbeddedNamespace == "bar");
  delete seq;
}
