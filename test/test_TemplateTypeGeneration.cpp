#include "CodeGenerator/MockCodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/Parser.hpp"
#include "Parser/AST/Statements/Sequence.hpp"
#include "catch.hpp"

TEST_CASE("resolveSymbol generates template types", "[templates]") {
  auto sm = gen::scope::ScopeManager::getInstance();
  sm->reset();

  parse::Parser parser;
  test::mockGen::CodeGenerator gen("mod", parser, "");

  auto cls = new ast::Class();
  cls->ident.ident = "Box";
  cls->genericTypes.push_back("T");
  cls->statement = new ast::Sequence();
  gen.genericTypes["Box"] = cls;

  ast::Type t;
  t.typeName = "Box.int";
  t.size = asmc::QWord;
  sm->assign("x", t, false);

  links::LinkedList<std::string> mods;
  links::LinkedList<ast::Expr *> idx;
  asmc::File file;
  gen.resolveSymbol("x", mods, file, idx);

  REQUIRE(gen.typeList["Box.int"] != nullptr);
}
