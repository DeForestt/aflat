#include <filesystem>
#include <unordered_map>

#include "CodeGenerator/CodeGenerator.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"
#include "catch.hpp"

TEST_CASE("Parser parses when clauses", "[parser][when]") {
  lex::Lexer l;
  auto tokens =
      l.Scan("when (T is dynamic and T has toString) fn foo() -> int {};", 1);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *func = dynamic_cast<ast::Function *>(seq->Statement1);
  REQUIRE(func != nullptr);
  REQUIRE(func->when.has_value());
  REQUIRE(func->when->predicates.size() == 2);
  CHECK(func->when->predicates[0].typeName == "T");
  CHECK(func->when->predicates[0].op == ast::WhenOperator::IS);
  CHECK(func->when->predicates[0].ident == "dynamic");
  CHECK(func->when->predicates[0].negated == false);
  CHECK(func->when->predicates[1].typeName == "T");
  CHECK(func->when->predicates[1].op == ast::WhenOperator::HAS);
  CHECK(func->when->predicates[1].ident == "toString");
  CHECK(func->when->predicates[1].negated == false);
}

TEST_CASE("when resolution checks primitive", "[when][resolution]") {
  parse::Parser p;
  gen::CodeGenerator gen("mod", p, "",
                         std::filesystem::current_path().string());

  ast::When w;
  ast::WhenPredicat pred;
  pred.op = ast::WhenOperator::IS;
  pred.typeName = "int";
  pred.ident = "primitive";
  pred.negated = false;
  w.predicates.push_back(pred);
  CHECK(gen.whenSatisfied(w));

  w.predicates[0].typeName = "Foo";
  CHECK_FALSE(gen.whenSatisfied(w));
}

TEST_CASE("replaceTypes updates when predicates", "[when][replace]") {
  lex::Lexer l;
  parse::Parser p;
  auto tokens = l.Scan("when (T is primitive) fn foo(x: T) {};", 1);
  tokens.invert();
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *func = dynamic_cast<ast::Function *>(seq->Statement1);
  REQUIRE(func != nullptr);
  REQUIRE(func->when.has_value());
  std::unordered_map<std::string, std::string> map{{"T", "int"}};
  func->replaceTypes(map);
  REQUIRE(func->when->predicates[0].typeName == "int");
  gen::CodeGenerator gen("mod", p, "",
                         std::filesystem::current_path().string());
  CHECK(gen.whenSatisfied(*func->when));
}
