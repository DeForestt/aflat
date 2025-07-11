#include <filesystem>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

#define MOCKGEN                                \
  auto parser = parse::Parser();               \
  auto mockGen = test::mockGen::CodeGenerator( \
      "mod", parser, "", std::filesystem::current_path().string());

bool compareFunc(ast::Function F, std::string input) {
  if (input == F.ident.ident) {
    return true;
  }
  return false;
}

TEST_CASE("canAssign will not assign to types with different names",
          "[canAssign]") {
  auto testType = ast::Type();
  MOCKGEN

  testType.typeName = "testType";

  CHECK_THROWS(mockGen.canAssign(testType, "int", "ERROR"));
}

TEST_CASE("canAssign assigns types with the same name", "[canAssing]") {
  auto testType = ast::Type();
  MOCKGEN

  testType.typeName = "testType";

  CHECK(mockGen.canAssign(testType, "testType", "ERROR"));
}

TEST_CASE("cannAssign returns false if it can implicit cast", "[canAssign]") {
  auto testType = ast::Type();
  auto testGenType = new gen::Type();
  auto constType = ast::Type();
  gen::Class *cl = new gen::Class();
  auto init = ast::Function();

  MOCKGEN

  init.ident.ident = "init";
  constType.typeName = "takes";
  testGenType->Ident = "takes";

  testType.typeName = "testType";
  cl->Ident = "testType";
  cl->nameTable.foo = compareFunc;
  init.argTypes.push_back(constType);
  cl->nameTable.push(init);
  mockGen.addType(testGenType);
  mockGen.addType(cl);

  CHECK_THROWS(!mockGen.canAssign(testType, "takes", "ERROR"));
}