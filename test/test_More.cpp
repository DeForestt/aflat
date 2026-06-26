#include "CodeGenerator/MockCodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"
#include "catch.hpp"

TEST_CASE("Definition compares name", "[preprocessor]") {
  Definition d;
  d.name = "FOO";
  d.value = "BAR";
  REQUIRE(Definition::compair(d, "FOO"));
  REQUIRE_FALSE(Definition::compair(d, "BAR"));
}

TEST_CASE("PreProcessor replaces defined values", "[preprocessor]") {
  PreProcessor pp;
  std::string output = pp.PreProcess(".def X = Y\nX\n", "", "");
  REQUIRE(output == "Y\n");
}

TEST_CASE("getLibPath forms correct suffix", "[cgutils]") {
  using gen::utils::getLibPath;
  std::string path = getLibPath("foo");
  REQUIRE(path.size() >= 9);
  REQUIRE(path.rfind("/libraries/std/foo/", path.size() - 1) !=
          std::string::npos);
}

TEST_CASE("sizeToInt returns expected values", "[codegen]") {
  REQUIRE(gen::utils::sizeToInt(asmc::QWord) == 8);
  REQUIRE(gen::utils::sizeToInt(asmc::Word) == 2);
  REQUIRE(gen::utils::sizeToInt(asmc::Byte) == 1);
  REQUIRE(gen::utils::sizeToInt(asmc::DWord) == 4);
}

TEST_CASE("Parser keeps comparison outside chained call extension",
          "[parser]") {
  lex::Lexer lexer;
  parse::Parser parser;
  auto tokens = lexer.Scan("m.pop().unwrap() == 20");
  tokens.invert();

  ast::Expr *expr = parser.parseExpr(tokens);
  auto *comparison = dynamic_cast<ast::Compound *>(expr);
  REQUIRE(comparison != nullptr);
  REQUIRE(comparison->op == ast::CompEqu);

  auto *left = dynamic_cast<ast::CallExpr *>(comparison->expr1);
  REQUIRE(left != nullptr);
  REQUIRE(left->call->ident == "m");
  REQUIRE(left->call->modList.count == 1);
  REQUIRE(left->call->modList.get(0) == "pop");

  auto *extension = dynamic_cast<ast::CallExpr *>(left->extention);
  REQUIRE(extension != nullptr);
  REQUIRE(extension->call->ident == "unwrap");

  auto *right = dynamic_cast<ast::IntLiteral *>(comparison->expr2);
  REQUIRE(right != nullptr);
  REQUIRE(right->val == 20);
}
