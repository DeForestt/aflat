#include <filesystem>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

TEST_CASE("deScope cleans owned non-primitives", "[deScope]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  ast::Function freeFunc;
  freeFunc.ident.ident = "free";
  gen.nameTable.push(freeFunc);

  gen::Symbol sym;
  sym.symbol = "obj";
  sym.type = ast::Type("Foo", asmc::QWord);
  sym.owned = true;

  auto file = gen.deScope(sym);
  REQUIRE(file != nullptr);
}

TEST_CASE("deScope skips primitives and sold symbols", "[deScope]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  ast::Function freeFunc;
  freeFunc.ident.ident = "free";
  gen.nameTable.push(freeFunc);

  gen::Symbol primitive;
  primitive.symbol = "x";
  primitive.type = ast::Type("int", asmc::DWord);
  primitive.owned = true;
  REQUIRE(gen.deScope(primitive) == nullptr);

  gen::Symbol sold;
  sold.symbol = "y";
  sold.type = ast::Type("Foo", asmc::QWord);
  sold.owned = true;
  sold.sold = 1;
  REQUIRE(gen.deScope(sold) == nullptr);
}
