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

TEST_CASE("memMove generates copy loop", "[memmove]") {
  auto parser = parse::Parser();
  gen::CodeGenerator gen("mod", parser, "",
                         std::filesystem::current_path().string());

  auto file = gen.memMove("%rax", "%rbx", 1);
  REQUIRE(file.text.size() >= 3);
  bool foundSrc = false;
  bool foundDst = false;
  bool pushSrc = false;
  bool pushDst = false;
  bool pushCnt = false;
  bool pushA = false;
  bool popSrc = false;
  bool popDst = false;
  bool popCnt = false;
  bool popA = false;
  for (int i = 0; i < file.text.size(); ++i) {
    if (auto *m = dynamic_cast<asmc::Mov *>(file.text.get(i))) {
      if (m->from == "%rax" && m->to == "%rsi") foundSrc = true;
      if (m->from == "%rbx" && m->to == "%rdi") foundDst = true;
    }
    if (auto *p = dynamic_cast<asmc::Push *>(file.text.get(i))) {
      if (p->op == "%rsi") pushSrc = true;
      if (p->op == "%rdi") pushDst = true;
      if (p->op == "%rcx") pushCnt = true;
      if (p->op == "%rax") pushA = true;
    }
    if (auto *p2 = dynamic_cast<asmc::Pop *>(file.text.get(i))) {
      if (p2->op == "%rsi") popSrc = true;
      if (p2->op == "%rdi") popDst = true;
      if (p2->op == "%rcx") popCnt = true;
      if (p2->op == "%rax") popA = true;
    }
  }
  REQUIRE(foundSrc);
  REQUIRE(foundDst);
  REQUIRE(pushSrc);
  REQUIRE(pushDst);
  REQUIRE(pushCnt);
  REQUIRE(pushA);
  REQUIRE(popSrc);
  REQUIRE(popDst);
  REQUIRE(popCnt);
  REQUIRE(popA);
}
