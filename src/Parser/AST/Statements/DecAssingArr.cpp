#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/AST/Statements/DecAssignArr.hpp"

namespace ast {
asmc::File const DecAssignArr::generate(gen::CodeGenerator &generator) {
  asmc::File OutputFile;
  ast::DecArr *dec = this->declare;
  ast::Type adr;
  adr.arraySize = 1;
  dec->indices.reset();
  while (dec->indices.pos != nullptr) {
    ast::IntLiteral *lit =
        dynamic_cast<ast::IntLiteral *>(dec->indices.shift());
    if (lit == nullptr) generator.alert("array index must be an integer");
    adr.indices.push(lit->val);
  }
  adr.indices.invert();
  adr.opType = asmc::Hard;
  adr.size = asmc::QWord;
  adr.typeName = "adr";
  adr.typeHint = &dec->type;

  const auto declare =
      new ast::Declare(dec->ident, dec->scope, "adr", this->mute, adr);

  auto assign = new ast::DecAssign();
  assign->declare = declare;
  assign->expr = this->expr;
  assign->mute = this->mute;
  assign->declare->scope = dec->scope;
  OutputFile << assign->generate(generator);
  return OutputFile;
}
}  // namespace ast