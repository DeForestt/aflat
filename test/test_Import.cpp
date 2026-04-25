#include <cstdio>
#include <filesystem>
#include <fstream>
#include <functional>
#include <unordered_map>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

TEST_CASE("Parser handles mixed import of classes and functions", "[parser]") {
  lex::Lexer l;
  PreProcessor pp;
  auto code = pp.PreProcess("import Foo, {bar} from \"Mod\" under m;", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);
  REQUIRE(imp->hasClasses);
  REQUIRE(imp->hasFunctions);
  REQUIRE(imp->imports.size() == 2);
  REQUIRE(imp->imports[0] == "Foo");
  REQUIRE(imp->imports[1] == "bar");
}

TEST_CASE("ImportsOnly emits functions in mixed import", "[codegen]") {
  std::ofstream mod("Temp.af");
  mod << "class Foo {}\n";
  mod << "export int bar() { return 0; };\n";
  mod.close();

  lex::Lexer l;
  PreProcessor pp;
  auto code =
      pp.PreProcess("import Foo, {bar} from \"./Temp\" under m;", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);

  test::mockGen::CodeGenerator gen("mod", p, "",
                                   std::filesystem::current_path().string());
  gen.ImportsOnly(imp);

  REQUIRE(gen.includedClasses().contains("Temp::Foo"));
  REQUIRE(gen.includedClasses().contains("Temp::bar"));
  REQUIRE(gen.nameSpaceTable().contains("m"));

  std::remove("Temp.af");
}

TEST_CASE("Mixed imports preload class definitions", "[imports]") {
  std::ofstream base("MixedBase.af");
  base << "class Base {}\n";
  base << "export int helper() { return 0; };\n";
  base.close();

  std::ofstream child("MixedChild.af");
  child << "import Base, {helper} from \"./MixedBase\";\n";
  child << "class Derived signs Base {\n";
  child << "  Derived init() { return my; };\n";
  child << "};\n";
  child.close();

  lex::Lexer l;
  PreProcessor pp;
  auto code = pp.PreProcess("import {Derived} from \"./MixedChild\";", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);

  test::mockGen::CodeGenerator gen("mod", p, "",
                                   std::filesystem::current_path().string());
  imp->generate(gen);

  REQUIRE_FALSE(gen.hasError());
  REQUIRE(gen.includedMemo().contains(
      std::filesystem::absolute("MixedBase.af").lexically_normal().string()));

  std::remove("MixedBase.af");
  std::remove("MixedChild.af");
}

TEST_CASE("Function imports are emitted after class preloading", "[imports]") {
  std::ofstream base("Preload.af");
  base << "class Base {}\n";
  base << "export int accept() { return 0; };\n";
  base.close();

  std::ofstream mod("ResultImport.af");
  mod << "import Base, {accept} from \"./Preload\";\n";
  mod << "export int call() { return 0; };\n";
  mod.close();

  lex::Lexer l;
  PreProcessor pp;
  auto code = pp.PreProcess("import {call} from \"./ResultImport\";", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);

  test::mockGen::CodeGenerator gen("mod", p, "",
                                   std::filesystem::current_path().string());
  REQUIRE_NOTHROW(imp->generate(gen));
  REQUIRE_FALSE(gen.hasError());

  std::remove("Preload.af");
  std::remove("ResultImport.af");
}

TEST_CASE("Imported error-returning functions keep expect available",
          "[imports]") {
  std::ofstream src("ErrSource.af");
  src << "export fn fail() -> int! { return 0; };\n";
  src.close();

  std::ofstream use("ErrUse.af");
  use << "import {fail} from \"./ErrSource\";\n";
  use << "export int call() { const int value = fail().expect(\"boom\"); "
         "return value; };\n";
  use.close();

  lex::Lexer l;
  PreProcessor pp;
  auto code = pp.PreProcess("import {call} from \"./ErrUse\";", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);

  test::mockGen::CodeGenerator gen("mod", p, "",
                                   std::filesystem::current_path().string());
  REQUIRE_NOTHROW(imp->generate(gen));
  REQUIRE_FALSE(gen.hasError());

  std::remove("ErrSource.af");
  std::remove("ErrUse.af");
}

TEST_CASE("extractAll does not poison function names", "[imports]") {
  std::ofstream mod("ExtractAll.af");
  mod << "class Thing {}\n";
  mod << "export int accept() { return 0; };\n";
  mod.close();

  std::ifstream file("ExtractAll.af");
  REQUIRE(file.is_open());
  std::string text((std::istreambuf_iterator<char>(file)),
                   std::istreambuf_iterator<char>());

  lex::Lexer l;
  PreProcessor pp;
  auto tokens = l.Scan(pp.PreProcess(text, "", ""));
  tokens.invert();
  parse::Parser p;
  ast::Statement *root = p.parseStmt(tokens);
  REQUIRE(root != nullptr);

  auto matches = gen::utils::extractAll("accept", root, "Result");
  REQUIRE_FALSE(matches.empty());

  auto again = gen::utils::extractAll("accept", root, "Result");
  REQUIRE_FALSE(again.empty());

  std::remove("ExtractAll.af");
}

TEST_CASE("Import applies nested namespaces", "[namespaces]") {
  std::ofstream inner("Inner.af");
  inner << "export int bar() { return 0; };\n";
  inner.close();

  std::ofstream outer("Outer.af");
  outer << "import { bar } from \"./Inner\" under i;\n";
  outer << "export int call() { return i.bar(); };\n";
  outer.close();

  lex::Lexer l;
  PreProcessor pp;
  auto code = pp.PreProcess("import {call} from \"./Outer\";", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);

  test::mockGen::CodeGenerator gen("mod", p, "",
                                   std::filesystem::current_path().string());
  imp->generate(gen);

  std::ifstream outerFile("Outer.af");
  std::string outerCode((std::istreambuf_iterator<char>(outerFile)),
                        std::istreambuf_iterator<char>());
  outerFile.close();
  lex::Lexer l2;
  PreProcessor pp2;
  auto t2 = l2.Scan(pp2.PreProcess(outerCode, "", ""));
  t2.invert();
  parse::Parser p2;
  ast::Statement *root = p2.parseStmt(t2);
  std::unordered_map<std::string, std::string> map;
  ast::collectImportNamespaces(root, map);
  std::function<ast::Statement *(ast::Statement *)> findCall =
      [&](ast::Statement *s) -> ast::Statement * {
    if (!s)
      return nullptr;
    if (auto f = dynamic_cast<ast::Function *>(s)) {
      if (f->ident.ident == "call")
        return s;
    }
    if (auto seq2 = dynamic_cast<ast::Sequence *>(s)) {
      if (auto r = findCall(seq2->Statement1))
        return r;
      return findCall(seq2->Statement2);
    }
    return nullptr;
  };
  ast::Statement *funcStmt = findCall(root);
  REQUIRE(funcStmt != nullptr);
  funcStmt->namespaceSwap(map);

  auto *func = dynamic_cast<ast::Function *>(funcStmt);
  REQUIRE(func != nullptr);
  std::function<ast::Call *(ast::Statement *)> findCallNode =
      [&](ast::Statement *s) -> ast::Call * {
    if (!s)
      return nullptr;
    if (auto cexpr = dynamic_cast<ast::CallExpr *>(s))
      return cexpr->call;
    if (auto call = dynamic_cast<ast::Call *>(s))
      return call;
    if (auto seqn = dynamic_cast<ast::Sequence *>(s)) {
      if (auto r = findCallNode(seqn->Statement1))
        return r;
      return findCallNode(seqn->Statement2);
    }
    if (auto ret = dynamic_cast<ast::Return *>(s))
      return findCallNode(ret->expr);
    if (auto expr = dynamic_cast<ast::Expr *>(s))
      return findCallNode(expr->extention);
    return nullptr;
  };
  auto *call = findCallNode(func->statement);
  REQUIRE(call != nullptr);
  REQUIRE(call->imbeddedNamespace.has_value());
  REQUIRE(call->imbeddedNamespace.value() == "Inner");

  std::remove("Inner.af");
  std::remove("Outer.af");
}

TEST_CASE("Imports handle parent directory paths", "[imports]") {
  std::filesystem::create_directory("sub");
  std::ofstream inner("Inner.af");
  inner << "export int bar() { return 0; };\n";
  inner.close();

  std::ofstream outer("sub/Outer.af");
  outer << "import {bar} from \"../Inner\" under i;\n";
  outer << "export int call() { return i.bar(); };\n";
  outer.close();

  lex::Lexer l;
  PreProcessor pp;
  auto code = pp.PreProcess("import {call} from \"./sub/Outer\";", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);

  test::mockGen::CodeGenerator gen("mod", p, "",
                                   std::filesystem::current_path().string());
  imp->generate(gen);
  REQUIRE_FALSE(gen.hasError());

  std::filesystem::remove_all("sub");
  std::filesystem::remove("Inner.af");
}

TEST_CASE("ImportsOnly uses import working directory", "[imports]") {
  std::filesystem::create_directories("FlatLog/Logger");
  std::ofstream logger("FlatLog/Logger/mod.af");
  logger << "export class Logger {}\n";
  logger.close();

  std::ofstream mod("FlatLog/mod.af");
  mod << "import Logger from \"./Logger\";\n";
  mod << "export int info() { return 0; };\n";
  mod.close();

  std::ofstream main("main.af");
  main << "import {info} from \"./FlatLog\" under log;\n";
  main.close();

  lex::Lexer l;
  PreProcessor pp;
  auto code =
      pp.PreProcess("import {info} from \"./FlatLog\" under log;", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);

  test::mockGen::CodeGenerator gen("mod", p, "",
                                   std::filesystem::current_path().string());
  imp->generate(gen);

  auto path =
      std::filesystem::absolute("FlatLog/mod.af").lexically_normal().string();
  auto *added = gen.includedMemo().get(path);
  REQUIRE(added != nullptr);

  gen.cwd() = std::filesystem::current_path();
  REQUIRE_NOTHROW(gen.ImportsOnly(added));

  std::filesystem::remove_all("FlatLog");
  std::filesystem::remove("main.af");
}

TEST_CASE("Cyclic imports do not recurse forever", "[imports]") {
  std::filesystem::create_directories("cycle");

  std::ofstream a("cycle/A.af");
  a << "import {bar} from \"./B\";\n";
  a << "export int foo() { return bar(); };\n";
  a.close();

  std::ofstream b("cycle/B.af");
  b << "import {foo} from \"./A\";\n";
  b << "export int bar() { return 0; };\n";
  b.close();

  lex::Lexer l;
  PreProcessor pp;
  auto code = pp.PreProcess("import {foo} from \"./cycle/A\";", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);

  test::mockGen::CodeGenerator gen("mod", p, "",
                                   std::filesystem::current_path().string());
  REQUIRE_NOTHROW(imp->generate(gen));
  REQUIRE_FALSE(gen.hasError());
  REQUIRE(gen.includedMemo().contains(
      std::filesystem::absolute("cycle/A.af").lexically_normal().string()));
  REQUIRE(gen.includedMemo().contains(
      std::filesystem::absolute("cycle/B.af").lexically_normal().string()));

  std::filesystem::remove_all("cycle");
}

TEST_CASE("Imported namespaces do not leak into parent imports", "[imports]") {
  std::ofstream inner("LeakInner.af");
  inner << "import {print} from \"String\" under str;\n";
  inner << "export int child() { return 0; };\n";
  inner.close();

  lex::Lexer l;
  PreProcessor pp;
  auto code =
      pp.PreProcess("import {child} from \"./LeakInner\" under inner;", "", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);

  test::mockGen::CodeGenerator gen("mod", p, "",
                                   std::filesystem::current_path().string());
  imp->generate(gen);

  REQUIRE(gen.nameSpaceTable().contains("inner"));
  REQUIRE_FALSE(gen.nameSpaceTable().contains("str"));

  std::remove("LeakInner.af");
}
