#include <filesystem>
#include <iostream>
#include <sstream>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

TEST_CASE("unused non-primitive return value warns", "[leak-warning]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  ast::Function make;
  make.ident.ident = "make";
  make.type = ast::Type("Foo", asmc::QWord);
  gen.nameTable.push(make);

  ast::Call call;
  call.ident = "make";
  call.Args = links::LinkedList<ast::Expr *>();

  std::ostringstream buffer;
  auto *old = std::cout.rdbuf(buffer.rdbuf());
  gen.GenSTMT(&call);
  std::cout.rdbuf(old);

  REQUIRE(buffer.str().find("warning") != std::string::npos);
}

TEST_CASE("returning non-primitive value does not warn", "[leak-warning]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  ast::Function make;
  make.ident.ident = "make";
  make.type = ast::Type("Foo", asmc::QWord);
  gen.nameTable.push(make);

  ast::Function func;
  func.ident.ident = "func";
  func.type = ast::Type("Foo", asmc::QWord);
  gen.currentFunction = &func;
  gen.inFunction = true;
  gen.returnType = func.type;

  gen::scope::ScopeManager::getInstance()->reset();
  gen::scope::ScopeManager::getInstance()->pushScope(true);

  auto call = new ast::CallExpr();
  call->call = new ast::Call();
  call->call->ident = "make";
  call->call->Args = links::LinkedList<ast::Expr *>();

  ast::Return ret;
  ret.expr = call;

  std::ostringstream buffer;
  auto *old = std::cout.rdbuf(buffer.rdbuf());
  gen.GenSTMT(&ret);
  std::cout.rdbuf(old);

  REQUIRE(buffer.str().find("warning") == std::string::npos);
}

TEST_CASE("passing temporary to non-owned parameter warns", "[leak-warning]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  ast::Function produce;
  produce.ident.ident = "produce";
  produce.type = ast::Type("Foo", asmc::QWord);
  gen.nameTable.push(produce);

  ast::Function consume;
  consume.ident.ident = "consume";
  consume.type = ast::Type("void", asmc::QWord);
  consume.argTypes.push_back(ast::Type("Foo", asmc::QWord));
  consume.mutability.push_back(false);
  consume.req = 1;
  gen.nameTable.push(consume);

  auto inner = new ast::CallExpr();
  inner->call = new ast::Call();
  inner->call->ident = "produce";
  inner->call->Args = links::LinkedList<ast::Expr *>();

  ast::Call outer;
  outer.ident = "consume";
  outer.Args = links::LinkedList<ast::Expr *>();
  outer.Args.push(inner);

  std::ostringstream buffer;
  auto *old = std::cout.rdbuf(buffer.rdbuf());
  gen.GenSTMT(&outer);
  std::cout.rdbuf(old);

  REQUIRE(buffer.str().find("warning") != std::string::npos);
}

TEST_CASE("passing temporary to owned parameter does not warn",
          "[leak-warning]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  ast::Function produce;
  produce.ident.ident = "produce";
  produce.type = ast::Type("Foo", asmc::QWord);
  gen.nameTable.push(produce);

  ast::Function take;
  take.ident.ident = "take";
  take.type = ast::Type("void", asmc::QWord);
  ast::Type param("Foo", asmc::QWord);
  param.isRvalue = true;
  take.argTypes.push_back(param);
  take.mutability.push_back(false);
  take.req = 1;
  gen.nameTable.push(take);

  auto inner = new ast::CallExpr();
  inner->call = new ast::Call();
  inner->call->ident = "produce";
  inner->call->Args = links::LinkedList<ast::Expr *>();

  ast::Call outer;
  outer.ident = "take";
  outer.Args = links::LinkedList<ast::Expr *>();
  outer.Args.push(inner);

  std::ostringstream buffer;
  auto *old = std::cout.rdbuf(buffer.rdbuf());
  gen.GenSTMT(&outer);
  std::cout.rdbuf(old);

  REQUIRE(buffer.str().find("warning") == std::string::npos);
}
