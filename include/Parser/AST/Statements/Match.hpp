#pragma once
#include "LinkedList.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"

namespace ast {

/*
 * @class Match
 * @brief This class will be used to represent a match statement
 */
class Match : public Expr {
 public:
  struct Pattern {
    std::string aliasName;
    std::optional<std::string> veriableName;
    Pattern(links::LinkedList<lex::Token *> &tokens);
    Pattern() = default;
    Pattern(std::string aliasName, std::optional<std::string> veriableName)
        : aliasName(std::move(aliasName)),
          veriableName(std::move(veriableName)) {}
  };

  struct Case {
    Pattern pattern;
    Statement *statement;
    Case() = default;
    Case(Pattern pattern, Statement *statement)
        : pattern(std::move(pattern)), statement(statement) {}
    Case(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
  };

  ast::Expr *expr = nullptr;  // The expression to match against
  std::vector<Case> cases;

  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  gen::GenerationResult const generateExpression(
      gen::CodeGenerator &generator, asmc::Size size,
      std::string typeHint = "") override {
    return generate(generator);
  }
  std::vector<Case> const parseCases(links::LinkedList<lex::Token *> &tokens,
                                     parse::Parser &parser);

  ast::Type returns =
      ast::Type("void", asmc::QWord);  // The return type of the match statement
  // Constructors
  Match() = default;
  Match(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
};
};  // namespace ast
