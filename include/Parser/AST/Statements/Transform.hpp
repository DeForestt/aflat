#pragma once
#include <unordered_map>

#include "Parser/AST.hpp"

namespace ast {
class Transform : public Statement {
public:
  std::string ident;
  std::string _template;

  Transform(links::LinkedList<lex::Token *> &tokens);
  Transform(const std::string &ident, const std::string &templateStr)
      : ident(ident), _template(templateStr) {}
  Transform() = default;
  ~Transform() = default;
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  ast::Statement *parse(const std::string &ident, std::string &type,
                        std::string &exp, std::string &scope, std::string &mut,
                        std::unordered_map<std::string, std::string> &args,
                        gen::CodeGenerator &generator);
};
}; // namespace ast