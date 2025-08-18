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

TEST_CASE("ImportsOnly ignores functions in mixed import", "[codegen]") {
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

  REQUIRE(gen.includedClasses.contains("Temp::Foo"));
  REQUIRE_FALSE(gen.includedClasses.contains("Temp::bar"));
  REQUIRE_FALSE(gen.nameSpaceTable.contains("m"));

  std::remove("Temp.af");
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
  auto *added = gen.includedMemo.get(path);
  REQUIRE(added != nullptr);

  gen.cwd = std::filesystem::current_path();
  REQUIRE_NOTHROW(gen.ImportsOnly(added));

  std::filesystem::remove_all("FlatLog");
  std::filesystem::remove("main.af");
}
