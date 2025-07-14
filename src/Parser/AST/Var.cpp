#include "CodeGenerator/CodeGenerator.hpp"
#include "Parser/AST.hpp"

namespace ast {

gen::GenerationResult const Var::generateExpr(gen::CodeGenerator &generator,
                                              asmc::Size size,
                                              std::string typeHint) {
  asmc::File file;
  gen::Expr expr = generator.genVarExpr(*this, file, size);
  return {file, expr};
}

}  // namespace ast
