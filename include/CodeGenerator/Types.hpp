#ifndef CODEGEN_TYPES_HPP
#define CODEGEN_TYPES_HPP

#include <string>
#include <vector>
#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"

namespace gen {

class Symbol {
 public:
  std::string symbol;
  ast::Type type;
  bool mutable_ = true;
  bool readOnly = false;
  bool mask = false;
  int byteMod;
  bool usable = true;
  int sold = -1;
  int underscores = 0;
  int refCount = 0;
  int assignCount = 0;
};

class Type {
 public:
  std::string Ident;
  links::LinkedList<Symbol> publicSymbols;
  links::LinkedList<Symbol> SymbolTable;
  static bool compare(Type* t, std::string ident);
  virtual int poly() { return 0; }
  int size;
};

class Class : public Type {
 public:
  bool dynamic = false;
  bool safeType = false;
  bool pedantic = false;
  links::SLinkedList<ast::Function, std::string> publicNameTable;
  links::SLinkedList<ast::Function, ast::Op> overloadTable;
  links::SLinkedList<ast::Function, std::string> nameTable;
  ast::Statement* contract;
  ast::Statement* body;
  std::vector<ast::DecAssign> defaultValues;
  static bool compare(Type* t, std::string ident);
  gen::Class* parent = nullptr;
};

class Enum : public Type {
 public:
  struct EnumValue {
   public:
    std::string name;
    int value;
    EnumValue() = default;
    EnumValue(std::string name, int value) : name(name), value(value) {}
  };

  Enum();

  links::SLinkedList<EnumValue, std::string> values;
  static bool compare(Type* t, std::string ident);
  static bool compareEnum(EnumValue e, std::string ident);
};

}  // namespace gen

#endif
