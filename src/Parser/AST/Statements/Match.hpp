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
  bool expressionContext = false;
  bool loanBindings = false;

  struct Pattern {
    std::string aliasName;
    std::optional<std::string> veriableName;
    int bindingLogicalLine = 0;
    bool takesOwnership = false;
    Pattern(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
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

  ast::Expr *expr = nullptr; // The expression to match against
  std::vector<Case> cases;

  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  gen::GenerationResult const
  generateExpression(gen::CodeGenerator &generator, asmc::Size size,
                     std::string typeHint = "") override {
    const auto previousContext = expressionContext;
    expressionContext = true;
    auto result = generate(generator);
    expressionContext = previousContext;
    return result;
  }
  std::vector<Case> const parseCases(links::LinkedList<lex::Token *> &tokens,
                                     parse::Parser &parser);

  ast::Type returns =
      ast::Type("void", asmc::QWord); // The return type of the match statement
  // Constructors
  Match() = default;
  Match(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
};
}; // namespace ast
