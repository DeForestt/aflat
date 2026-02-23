#include "Parser/AST/Statements/Continue.hpp"

#include "CodeGenerator/CodeGenerator.hpp"

namespace ast {
/**
 * @brief Construct a new Continue object
 */
Continue::Continue(links::LinkedList<lex::Token *> &tokens) {
  this->logicalLine = tokens.peek()->lineCount;
  if (dynamic_cast<lex::INT *>(tokens.peek()) != nullptr)
    this->level = std::stoi(dynamic_cast<lex::INT *>(tokens.pop())->value);
  else
    this->level = 1;
}

gen::GenerationResult const Continue::generate(gen::CodeGenerator &generator) {
  asmc::File OutputFile;
  if (generator.continueContext.size() == 0)
    generator.alert("Attempted to continue outside of a loop");
  if (generator.continueContext.size() < this->level)
    generator.alert("Attempted to continue deeper than the current loop");

  // we need to pop the scope before continuing to avoid memory leaks

  int index = this->level - 1;
  asmc::Jmp *jmp = new asmc::Jmp();
  jmp->logicalLine = this->logicalLine;
  jmp->to = generator.continueContext.get(index);
  OutputFile.text << jmp;
  return {OutputFile, std::nullopt};
}
}; // namespace ast