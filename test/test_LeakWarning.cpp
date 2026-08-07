#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Configs.hpp"
#include "Parser/AST.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("unused non-primitive return value warns", "[leak-warning]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  parser.addType("Foo", asmc::Hard, asmc::QWord, false, true);
  auto foo = new gen::Class();
  foo->Ident = "Foo";
  foo->uniqueType = true;
  gen.addType(foo);

  ast::Function make;
  make.ident.ident = "make";
  make.type = ast::Type("Foo", asmc::QWord);
  gen.nameTable().push(make);

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

  parser.addType("Foo", asmc::Hard, asmc::QWord, false, true);
  auto foo = new gen::Class();
  foo->Ident = "Foo";
  foo->uniqueType = true;
  gen.addType(foo);

  ast::Function make;
  make.ident.ident = "make";
  make.type = ast::Type("Foo", asmc::QWord);
  gen.nameTable().push(make);

  ast::Function func;
  func.ident.ident = "func";
  func.type = ast::Type("Foo", asmc::QWord);
  gen.currentFunction() = &func;
  gen.inFunction() = true;
  gen.returnType() = func.type;

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

  parser.addType("Foo", asmc::Hard, asmc::QWord, false, true);
  auto foo = new gen::Class();
  foo->Ident = "Foo";
  foo->uniqueType = true;
  gen.addType(foo);

  ast::Function produce;
  produce.ident.ident = "produce";
  produce.type = ast::Type("Foo", asmc::QWord);
  gen.nameTable().push(produce);

  ast::Function consume;
  consume.ident.ident = "consume";
  consume.type = ast::Type("void", asmc::QWord);
  consume.argTypes.push_back(ast::Type("Foo", asmc::QWord));
  consume.mutability.push_back(false);
  consume.req = 1;
  gen.nameTable().push(consume);

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

TEST_CASE("passing loaned unique result to non-owned parameter does not warn",
          "[leak-warning]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  parser.addType("Foo", asmc::Hard, asmc::QWord, false, true);
  auto foo = new gen::Class();
  foo->Ident = "Foo";
  foo->uniqueType = true;
  gen.addType(foo);

  ast::Function borrow;
  borrow.ident.ident = "borrow";
  borrow.type = ast::Type("Foo", asmc::QWord);
  borrow.returnLowOwnership = true;
  gen.nameTable().push(borrow);

  ast::Function inspect;
  inspect.ident.ident = "inspect";
  inspect.type = ast::Type("void", asmc::QWord);
  inspect.argTypes.push_back(ast::Type("Foo", asmc::QWord));
  inspect.mutability.push_back(false);
  inspect.req = 1;
  gen.nameTable().push(inspect);

  auto inner = new ast::CallExpr();
  inner->call = new ast::Call();
  inner->call->ident = "borrow";
  inner->call->Args = links::LinkedList<ast::Expr *>();

  ast::Call outer;
  outer.ident = "inspect";
  outer.Args = links::LinkedList<ast::Expr *>();
  outer.Args.push(inner);

  std::ostringstream buffer;
  auto *old = std::cout.rdbuf(buffer.rdbuf());
  gen.GenSTMT(&outer);
  std::cout.rdbuf(old);

  REQUIRE(buffer.str().find("warning") == std::string::npos);
}

TEST_CASE("passing temporary to owned parameter does not warn",
          "[leak-warning]") {
  auto parser = parse::Parser();
  test::mockGen::CodeGenerator gen("mod", parser, "",
                                   std::filesystem::current_path().string());

  parser.addType("Foo", asmc::Hard, asmc::QWord, false, true);
  auto foo = new gen::Class();
  foo->Ident = "Foo";
  foo->uniqueType = true;
  gen.addType(foo);

  ast::Function produce;
  produce.ident.ident = "produce";
  produce.type = ast::Type("Foo", asmc::QWord);
  gen.nameTable().push(produce);

  ast::Function take;
  take.ident.ident = "take";
  take.type = ast::Type("void", asmc::QWord);
  ast::Type param("Foo", asmc::QWord);
  param.isRvalue = true;
  take.argTypes.push_back(param);
  take.mutability.push_back(false);
  take.req = 1;
  gen.nameTable().push(take);

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

TEST_CASE("formatted unique toString result does not warn",
          "[leak-warning][generics][vector]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/vector_unique_to_string");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "vector_unique_to_string.af";
  const auto output = dir / "vector_unique_to_string.s";
  std::ofstream ofs(source);
  ofs << ".needs <std>\n";
  ofs << "import vector from \"Collections/Vector\";\n";
  ofs << "import uni_string from \"uni_string\";\n";
  ofs << "class Item {\n";
  ofs << "    fn init() -> Self { return my; };\n";
  ofs << "    fn toString() -> uni_string { return `item`; };\n";
  ofs << "};\n";
  ofs << "fn main() -> int {\n";
  ofs << "    let values = new vector::<Item>();\n";
  ofs << "    let item = new Item();\n";
  ofs << "    values.push_back(item);\n";
  ofs << "    let rendered = values.toString();\n";
  ofs << "    delete rendered;\n";
  ofs << "    delete values;\n";
  ofs << "    return 0;\n";
  ofs << "};\n";
  ofs.close();

  std::ostringstream buffer;
  auto *old = std::cout.rdbuf(buffer.rdbuf());
  const bool result =
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  std::cout.rdbuf(old);
  fs::remove_all(dir);

  REQUIRE(result);
  CHECK(buffer.str().find("without transferring ownership may leak") ==
        std::string::npos);
}
