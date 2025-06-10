#include <cstdio>
#include <fstream>

#include "CodeGenerator/MockCodeGenerator.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

TEST_CASE("Parser handles mixed import of classes and functions", "[parser]") {
  lex::Lexer l;
  PreProcessor pp;
  auto code = pp.PreProcess("import Foo, {bar} from \"Mod\" under m;", "");
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
  auto code = pp.PreProcess("import Foo, {bar} from \"./Temp\" under m;", "");
  auto tokens = l.Scan(code);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *imp = dynamic_cast<ast::Import *>(seq->Statement1);
  REQUIRE(imp != nullptr);

  test::mockGen::CodeGenerator gen("mod", p, "");
  gen.ImportsOnly(imp);

  REQUIRE(gen.includedClasses.contains("Temp::Foo"));
  REQUIRE_FALSE(gen.includedClasses.contains("Temp::bar"));
  REQUIRE_FALSE(gen.nameSpaceTable.contains("m"));

  std::remove("Temp.af");
}
