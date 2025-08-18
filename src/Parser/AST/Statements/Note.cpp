#include "Parser/AST/Statements/Note.hpp"

#include <iostream>

#include "CodeGenerator/CodeGenerator.hpp"

namespace ast {

gen::GenerationResult const Note::generate(gen::CodeGenerator &generator) {
  std::cout << "Generating: " << message << std::endl;
  asmc::File file;
  file.text << new asmc::nop();
  return {file, std::nullopt};
}

} // namespace ast
