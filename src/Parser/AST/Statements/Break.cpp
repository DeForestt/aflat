#include "Parser/AST/Statements/Break.hpp"

#include "CodeGenerator/CodeGenerator.hpp"

namespace ast {
/**
 * @brief Construct a new Break object
 */
Break::Break(links::LinkedList<lex::Token *> &tokens) {
  this->logicalLine = tokens.peek()->lineCount;
  columnStart = tokens.peek()->columnStart;
  columnEnd = tokens.peek()->columnEnd;
  if (dynamic_cast<lex::INT *>(tokens.peek()) != nullptr)
    this->level = std::stoi(dynamic_cast<lex::INT *>(tokens.pop())->value);
  else
    this->level = 1;
}

gen::GenerationResult const Break::generate(gen::CodeGenerator &generator) {
  asmc::File OutputFile;
  if (generator.breakContext.size() == 0)
    generator.alert("Attempted to break outside of a loop");
  if (generator.breakContext.size() < this->level)
    generator.alert("Attempted to break deeper than the current loop");

  int index = this->level - 1;
  asmc::Jmp *jmp = new asmc::Jmp();
  jmp->logicalLine = this->logicalLine;
  jmp->to = generator.breakContext.get(index);
  OutputFile.text << jmp;
  return {OutputFile, std::nullopt};
}

};  // namespace ast