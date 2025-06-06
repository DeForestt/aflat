#include "Parser/AST/Statements/Sequence.hpp"

#include "CodeGenerator/CodeGenerator.hpp"

namespace ast {
gen::GenerationResult const Sequence::generate(gen::CodeGenerator &generator) {
  generator.logicalLine = this->logicalLine;
  generator.column = this->column;
  asmc::File file;
  if (Statement1) file << generator.GenSTMT(Statement1);
  if (Statement2) file << generator.GenSTMT(Statement2);
  return {file, std::nullopt};
}
}  // namespace ast