#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Call
 * @brief This class is used to represent a function call statement
 */
class Call : public Statement {
 public:
  std::string ident;
  links::LinkedList<Expr *> Args;
  links::LinkedList<std::string> modList;
  std::string publify = "";
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  Call() = default;
  Call(const std::string &ident, const links::LinkedList<Expr *> &args,
       links::LinkedList<std::string> modList)
      : ident(ident), Args(args), modList(modList) {}
  std::string toString() override;
};

}  // namespace ast