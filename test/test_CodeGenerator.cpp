#include "catch.hpp"
#include "CodeGenerator/MockCodeGenerator.hpp"

bool compareFunc(ast::Function F, std::string input) {
  if (input == F.ident.ident) {
    return true;
  }
  return false;
}

TEST_CASE("canAssign will not assign to types with different names", "[canAssign]") {
    auto testType = ast::Type();
    auto mockGen = test::mockGen::CodeGenerator("mod");

    testType.typeName = "testType";

    CHECK_THROWS(mockGen.canAssign(testType, "int", "ERROR"));
}

TEST_CASE("canAssign assigns types with the same name", "[canAssing]") {
    auto testType = ast::Type();
    auto mockGen = test::mockGen::CodeGenerator("mod");

    testType.typeName = "testType";
    
    CHECK(mockGen.canAssign(testType, "testType", "ERROR"));
}

TEST_CASE("cannAssign returns false if it can implicit cast", "[canAssign]") {
    auto testType = ast::Type();
    auto testGenType = new gen::Type();
    auto constType = ast::Type();
    gen::Class * cl = new gen::Class();
    auto init = ast::Function();
    auto mockGen = test::mockGen::CodeGenerator("mod");

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