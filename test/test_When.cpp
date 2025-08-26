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
  CHECK(func->when->predicates[0].join == ast::WhenJoiner::AND);
  CHECK(func->when->predicates[1].typeName == "T");
  CHECK(func->when->predicates[1].op == ast::WhenOperator::HAS);
  CHECK(func->when->predicates[1].ident == "toString");
  CHECK(func->when->predicates[1].negated == false);
}

TEST_CASE("Parser parses when clauses with or", "[parser][when]") {
  lex::Lexer l;
  auto tokens =
      l.Scan("when (T is dynamic or T has toString) fn foo() -> int {};", 1);
  tokens.invert();
  parse::Parser p;
  ast::Statement *stmt = p.parseStmt(tokens);
  auto *seq = dynamic_cast<ast::Sequence *>(stmt);
  REQUIRE(seq != nullptr);
  auto *func = dynamic_cast<ast::Function *>(seq->Statement1);
  REQUIRE(func != nullptr);
  REQUIRE(func->when.has_value());
  REQUIRE(func->when->predicates.size() == 2);
  CHECK(func->when->predicates[0].join == ast::WhenJoiner::OR);
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

  ast::WhenPredicat pred2;
  pred2.op = ast::WhenOperator::IS;
  pred2.typeName = "int";
  pred2.ident = "primitive";
  pred2.negated = false;
  w.predicates[0].join = ast::WhenJoiner::OR;
  w.predicates.push_back(pred2);
  CHECK(gen.whenSatisfied(w));
}

TEST_CASE("GenTable skips statements with failing when", "[when][codegen]") {
  parse::Parser p;
  gen::CodeGenerator gen("mod", p, "",
                         std::filesystem::current_path().string());

  links::LinkedList<gen::Symbol> table;
  auto *decl = new ast::Declare();
  decl->ident = "x";
  decl->type = ast::Type("int", asmc::DWord);

  ast::When w;
  ast::WhenPredicat pred;
  pred.op = ast::WhenOperator::IS;
  pred.typeName = "Foo";
  pred.ident = "primitive";
  pred.negated = false;
  w.predicates.push_back(pred);
  decl->when = w;

  gen.GenTable(decl, table);
  CHECK(table.head == nullptr);
}

TEST_CASE("GenArgs skips statements with failing when", "[when][codegen]") {
  parse::Parser p;
  gen::CodeGenerator gen("mod", p, "",
                         std::filesystem::current_path().string());

  ast::Function func;
  func.argTypes.push_back(ast::Type("int", asmc::DWord));
  asmc::File outFile;
  int index = 0;

  auto *decl = new ast::Declare();
  decl->ident = "x";
  decl->type = ast::Type("int", asmc::DWord);

  ast::When w;
  ast::WhenPredicat pred;
  pred.op = ast::WhenOperator::IS;
  pred.typeName = "Foo";
  pred.ident = "primitive";
  pred.negated = false;
  w.predicates.push_back(pred);
  decl->when = w;

  auto out = gen.GenArgs(decl, outFile, func, index);
  CHECK(out.text.head == nullptr);
  CHECK(index == 0);
}

TEST_CASE("ImportsOnly skips statements with failing when", "[when][codegen]") {
  parse::Parser p;
  gen::CodeGenerator gen("mod", p, "",
                         std::filesystem::current_path().string());

  auto *decl = new ast::Declare();
  decl->ident = "x";
  decl->type = ast::Type("int", asmc::DWord);

  ast::When w;
  ast::WhenPredicat pred;
  pred.op = ast::WhenOperator::IS;
  pred.typeName = "Foo";
  pred.ident = "primitive";
  pred.negated = false;
  w.predicates.push_back(pred);
  decl->when = w;

  auto out = gen.ImportsOnly(decl);
  CHECK(out.text.head == nullptr);
}
