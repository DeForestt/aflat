#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

TEST_CASE("namespaceSwap updates call identifiers", "[namespace]") {
  auto call = new ast::Call();
  call->ident = "foo";
  ast::Statement *stmt = call;
  std::unordered_map<std::string, std::string> map;
  map["foo"] = "bar";
  stmt->namespaceSwap(map);
  REQUIRE(call->ident == "bar");
}
