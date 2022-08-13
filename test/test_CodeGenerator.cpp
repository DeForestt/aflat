#include "catch.hpp"
#include "CodeGenerator/MockCodeGenerator.hpp"


TEST_CASE("canAssign will not assign to types with different names", "[canAssign]") {
    auto testType = ast::Type();
    auto mockGen = test::mockGen::CodeGenerator("mod");

    testType.typeName = "testType";

    CHECK_THROWS(mockGen.canAssign(testType, "int"));
}

TEST_CASE("canAssign assigns types with the same name", "[canAssing]") {
    auto testType = ast::Type();
    auto mockGen = test::mockGen::CodeGenerator("mod");

    testType.typeName = "testType";
    
    CHECK(mockGen.canAssign(testType, "testType"));
}