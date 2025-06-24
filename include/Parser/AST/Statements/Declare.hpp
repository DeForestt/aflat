#pragma once
#include "Parser/AST.hpp"

namespace ast {
/**
 * @class Declare
 * @brief Represents a declaration statement.
 */
class Declare : public Arg, public Statement {
 public:
  ScopeMod scope;
  std::string ident;
  std::string TypeName;
  std::string requestType;
  links::LinkedList<std::string> modList;
  bool mask = false;
  bool mut = true;
  bool readOnly = false;
  bool trust = false;
  Type type;
  Declare() = default;
  Declare(const std::string &ident, const ScopeMod &scope,
          const std::string &TypeName, const bool mut, const Type &type,
          const std::string &requestType,
          links::LinkedList<std::string> modList,
          bool readOnly = false)
      : ident(ident),
        scope(scope),
        TypeName(TypeName),
        mut(mut),
        readOnly(readOnly),
        type(type),
        requestType(requestType),
        modList(modList) {}
  Declare(const Declare &other)
      : ident(other.ident),
        scope(other.scope),
        TypeName(other.TypeName),
        mut(other.mut),
        readOnly(other.readOnly),
        type(other.type),
        requestType(other.requestType),
        modList(other.modList) {
    this->logicalLine = other.logicalLine;
  }
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
};
};  // namespace ast