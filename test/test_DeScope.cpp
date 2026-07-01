#include <filesystem>
#include <string>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

namespace {
bool compareFunction(const ast::Function &function, const std::string &ident) {
  return function.ident.ident == ident;
}
} // namespace

TEST_CASE("deScope cleans owned non-primitives", "[deScope]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  parser.addType("Foo", asmc::Hard, asmc::QWord, false, true);
  auto foo = new gen::Class();
  foo->Ident = "Foo";
  foo->uniqueType = true;
  gen.addType(foo);

  ast::Function freeFunc;
  freeFunc.ident.ident = "free";
  gen.nameTable().push(freeFunc);

  gen::Symbol sym;
  sym.symbol = "obj";
  sym.type = ast::Type("Foo", asmc::QWord);
  sym.owned = true;

  auto file = gen.deScope(sym);
  REQUIRE(file != nullptr);
}

TEST_CASE("deScope skips primitives, sold, and returned symbols", "[deScope]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  parser.addType("Foo", asmc::Hard, asmc::QWord, false, true);
  auto foo = new gen::Class();
  foo->Ident = "Foo";
  foo->uniqueType = true;
  gen.addType(foo);

  ast::Function freeFunc;
  freeFunc.ident.ident = "free";
  gen.nameTable().push(freeFunc);

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

  gen::Symbol returned;
  returned.symbol = "z";
  returned.type = ast::Type("Foo", asmc::QWord);
  returned.owned = true;
  returned.returned = true;
  REQUIRE(gen.deScope(returned) == nullptr);
}

TEST_CASE("deScope emits cleanup against the symbol slot", "[deScope]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  parser.addType("Foo", asmc::Hard, asmc::QWord, false, false);
  auto stringType = new gen::Class();
  stringType->Ident = "Foo";
  stringType->uniqueType = false;
  stringType->nameTable.foo = compareFunction;
  ast::Function endScope;
  endScope.ident.ident = "endScope";
  endScope.scopeName = "Foo";
  stringType->nameTable.push(endScope);
  gen.addType(stringType);

  gen::Symbol sym;
  sym.symbol = "path";
  sym.type = ast::Type("Foo", asmc::QWord);
  sym.byteMod = 984;
  sym.owned = true;

  auto file = gen.deScope(sym);
  REQUIRE(file != nullptr);

  std::string emitted;
  for (auto *inst : file->text) {
    emitted += inst->toString();
  }

  REQUIRE(emitted.find("-984(%rbp)") != std::string::npos);
  REQUIRE(emitted.find("pub_Foo_endScope") != std::string::npos);
  REQUIRE(emitted.find("path") == std::string::npos);
}
