#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Declare
 * @brief Represents a declaration statement.
 */
class Declare : public Arg, public Statement {
 public:
  ScopeMod scope;
  std::string ident;
  std::string TypeName;
  bool mut = true;
  Type type;
  Declare() = default;
  Declare(const std::string &ident, const ScopeMod &scope,
          const std::string &TypeName, const bool mut, const Type &type)
      : ident(ident), scope(scope), TypeName(TypeName), mut(mut), type(type){};
};
};  // namespace ast