#pragma once
#include <optional>

#include "CodeGenerator/Expr.hpp"

namespace gen {
struct GenerationResult {
  asmc::File file;
  std::optional<gen::Expr> expr;
};
} // namespace gen
