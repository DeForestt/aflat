#pragma once
#include <optional>
#include <string>

#include "LinkedListS.hpp"
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Call
 * @brief This class is used to represent a function call statement
 */
class Call : public Statement {
public:
  std::string ident;
  std::optional<std::string> imbeddedNamespace = std::nullopt;
  links::LinkedList<Expr *> Args;
  links::LinkedList<std::string> modList;
  std::vector<std::string> genericTypes;
  std::string publify = "";
  bool slick = false;
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  Call() = default;
  Call(const std::string &ident, const links::LinkedList<Expr *> &args,
       links::LinkedList<std::string> modList)
      : ident(ident), Args(args), modList(modList) {}
  Call(const std::string &ident, const links::LinkedList<Expr *> &args,
       links::LinkedList<std::string> modList,
       const std::vector<std::string> &genericTypes)
      : ident(ident), Args(args), modList(modList), genericTypes(genericTypes) {
  }

  // copy constructor
  Call(const Call &other)
      : Statement(other), ident(other.ident), Args(other.Args),
        modList(other.modList), genericTypes(other.genericTypes),
        publify(other.publify), allowDiscardWarning(other.allowDiscardWarning) {
  }
  std::string toString() override;

private:
  bool allowDiscardWarning = false;
  gen::GenerationResult
  generateAttempt(gen::CodeGenerator &generator,
                  std::optional<int> forcedOverloadIndex,
                  links::SLinkedList<ast::Function, std::string> *forcedTable,
                  const std::string &forcedIdent);
  ast::Function *findFunctionByOverload(
      links::SLinkedList<ast::Function, std::string> &table,
      const std::string &ident, std::optional<int> forcedOverloadIndex,
      links::SLinkedList<ast::Function, std::string> *forcedTable,
      const std::string &forcedIdent,
      links::SLinkedList<ast::Function, std::string> *&activeTable,
      std::string &activeIdent, int &activeIndex);
  [[noreturn]] void
  requestOverloadRetry(gen::CodeGenerator &generator,
                       links::SLinkedList<ast::Function, std::string> *table,
                       const std::string &ident, int currentIndex,
                       const std::string &message, bool fatal = true,
                       bool allowFallbackToCurrent = false,
                       bool allowDiscardWarningOnFallback = false);
};

} // namespace ast
