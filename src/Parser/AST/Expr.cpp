#include "CodeGenerator/CodeGenerator.hpp"
#include "Parser/AST.hpp"

namespace ast {

gen::GenerationResult const Expr::generateExpr(gen::CodeGenerator &generator,
                                               asmc::Size size,
                                               std::string typeHint) {
  asmc::File file;
  gen::Expr expr = generator.GenExprRest(this, file, size, std::move(typeHint));
  return {file, expr};
}

}  // namespace ast
