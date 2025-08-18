#include "Parser/AST/Statements/Ellipsis.hpp"

#include "CodeGenerator/CodeGenerator.hpp"

namespace ast {

gen::GenerationResult const Ellipsis::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  file.text << new asmc::nop();
  return {file, std::nullopt};
}

} // namespace ast
