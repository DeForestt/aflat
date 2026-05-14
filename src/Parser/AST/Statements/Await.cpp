#include "Parser/AST/Statements/Await.hpp"

#include "CodeGenerator/CodeGenerator.hpp"

namespace ast {
Await::Await(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  expr = parser.parseExpr(tokens);
  logicalLine = expr != nullptr ? expr->logicalLine : 0;
}

gen::GenerationResult const Await::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  file.text << new asmc::nop();
  return {file, std::nullopt};
}
} // namespace ast
