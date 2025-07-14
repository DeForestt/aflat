#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <iostream>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/GenerationResult.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"

using namespace gen::utils;

static bool startsWith(const std::string &s, const std::string &prefix) {
  return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

namespace gen {

Expr CodeGenerator::GenExpr(ast::Expr *expr, asmc::File &OutputFile,
                            asmc::Size size, std::string typeHint) {
  auto result = expr->generateExpr(*this, size, std::move(typeHint));
  OutputFile << result.file;
  return result.expr.value();
}

}  // namespace gen
