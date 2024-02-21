#include "Parser/AST/Statements/Continue.hpp"

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
};  // namespace ast