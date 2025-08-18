#pragma once
#include <vector>

#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
#include "Scanner.hpp"

namespace ast {
class List : public Expr {
public:
  std::vector<Expr *> items = std::vector<Expr *>();
  List() = default;
  List(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);

  gen::GenerationResult const
  generateExpression(gen::CodeGenerator &generator, asmc::Size size,
                     std::string typeHint = "") override;

  std::string toString() override {
    std::string result = "[";
    for (size_t i = 0; i < items.size(); ++i) {
      result += items[i]->toString();
      if (i < items.size() - 1) {
        result += ", ";
      }
    }
    result += "]";
    return result;
  }
};
} // namespace ast
