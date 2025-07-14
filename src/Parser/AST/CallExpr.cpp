#include "CodeGenerator/CodeGenerator.hpp"
#include "Parser/AST.hpp"

namespace ast {

gen::GenerationResult const CallExpr::generateExpr(
    gen::CodeGenerator &generator, asmc::Size size, std::string typeHint) {
  asmc::File file;
  gen::Expr expr = generator.genCallExpr(this, file, size);
  return {file, expr};
}

}  // namespace ast
