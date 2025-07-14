#include "CodeGenerator/CodeGenerator.hpp"
#include "Parser/AST.hpp"

namespace ast {
gen::GenerationResult const Expr::generateExpression(
    gen::CodeGenerator &generator, asmc::Size size, std::string typeHint) {
  generator.alert("Do not know how to generate expression of type " +
                  this->toString());
  return {asmc::File(), std::nullopt};
}
}  // namespace ast
