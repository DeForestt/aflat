#ifndef CODEGEN_TYPES_HPP
#define CODEGEN_TYPES_HPP

#include <boost/integer/integer_log2.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"

namespace gen {

struct FieldState {
  enum class Status : uint8_t { Unknown, Uninit, Valid, Moved, Invalid };

  Status status = Status::Unknown;
  uint8_t version = 0;

  uint32_t lastSiteId = 0; // The last site where this field was modified
};

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
  bool returned = false;
  int underscores = 0;
  int refCount = 0;
  int assignCount = 0;
  bool owned = false;

  std::vector<FieldState> fieldStates;
  std::unordered_map<std::string, int> fieldIndex; // field->index mapping
};

enum class EffectKind : uint8_t {
  WriteField,
  MoveField,
  InvalidateField,
  RevalidateField,
};

struct FieldEffect {
  EffectKind kind;
  int field; // -1 = wholeObject
};

struct MethodSummary {
  std::vector<FieldEffect> effects;
  bool returnsOwnership = true;
};

class Type {
public:
  std::string Ident;
  bool hidden = false;
  links::LinkedList<Symbol> publicSymbols;
  links::LinkedList<Symbol> SymbolTable;
  static bool compare(Type *t, std::string ident);
  virtual int poly() { return 0; }
  int size;
  bool uniqueType = false;
};

class Class : public Type {
public:
  bool dynamic = false;
  bool safeType = false;
  bool pedantic = false;
  links::SLinkedList<ast::Function, std::string> publicNameTable;
  links::SLinkedList<ast::Function, ast::Op> overloadTable;
  links::SLinkedList<ast::Function, std::string> nameTable;
  std::unordered_map<std::string, MethodSummary> methodSummaries;
  ast::Statement *contract;
  ast::Statement *body;
  std::vector<ast::DecAssign> defaultValues;
  static bool compare(Type *t, std::string ident);
  gen::Class *parent = nullptr;
};

class Union : public Class {
public:
  struct Alias {
    int byteSize;
    std::string name;
    std::variant<ast::Type *, ast::Expr *> value;
    Alias(std::string name, std::variant<ast::Type *, ast::Expr *> value,
          int byteSize)
        : name(std::move(name)), value(std::move(value)), byteSize(byteSize) {}
  };

  int largestSize = 0;
  std::vector<Alias> aliases;
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
  static bool compare(Type *t, std::string ident);
  static bool compareEnum(EnumValue e, std::string ident);
};

inline void bump(FieldState &fs, uint32_t siteId) {
  ++fs.version;
  fs.lastSiteId = siteId;
}

void apply(FieldState &fs, EffectKind effect, uint32_t siteId);

} // namespace gen

#endif
