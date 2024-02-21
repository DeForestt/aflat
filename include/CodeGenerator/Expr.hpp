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
  Expr() = default;
};
}  // namespace gen
