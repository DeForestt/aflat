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
