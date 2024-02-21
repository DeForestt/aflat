#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Delete
 * @brief This class is used to represent a delete statement
 */
class Delete : public Statement {
 public:
  std::string ident;
  links::LinkedList<std::string> modList;
  Delete() = default;
  Delete(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser);
};
}  // namespace ast