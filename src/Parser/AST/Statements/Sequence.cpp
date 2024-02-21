#include "Parser/AST/Statements/Sequence.hpp"

#include "CodeGenerator/CodeGenerator.hpp"

namespace ast {
asmc::File const Sequence::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  file << generator.GenSTMT(Statement1);
  file << generator.GenSTMT(Statement2);
  return file;
}
}  // namespace ast