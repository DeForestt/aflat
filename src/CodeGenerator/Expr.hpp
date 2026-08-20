#pragma once
#include <string>

#include "ASM.hpp"
#include "CodeGenerator/Scope/ScopeTypes.hpp"

namespace gen {

enum class LoanProvenance {
  None,
  Lexical,
  Unknown,
};

class Expr {
public:
  asmc::OpType op = asmc::Hard;
  std::string access;
  std::string type = "";
  asmc::Size size;
  bool passable = true;
  bool owned = false;
  // True when this expression is an owned value that may be transferred.
  // Owned variables remain lvalues until they are explicitly sold.
  bool transferable = false;
  bool transferExplicit = false;

  LoanProvenance loanProvenance = LoanProvenance::None;
  scope::ScopeId loanScope = 0;

  bool requiresImmutableBinding = false;
  std::string immutableBindingSource;
  void adoptImmutableRequirement(const Expr &other) {
    if (other.requiresImmutableBinding) {
      requiresImmutableBinding = true;
      immutableBindingSource = other.immutableBindingSource;
    }
  }
  Expr() = default;
};
} // namespace gen
