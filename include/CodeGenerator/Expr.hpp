#pragma once
#include <string>

#include "ASM.hpp"

namespace gen {
class Expr {
public:
  asmc::OpType op = asmc::Hard;
  std::string access;
  std::string type = "";
  asmc::Size size;
  bool passable = true;
  bool owned = false;
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
