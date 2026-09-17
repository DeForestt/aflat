#pragma once
#include <string>

#include "ASM.hpp"
#include "CodeGenerator/Scope/ScopeTypes.hpp"

namespace gen {

enum class LoanProvenance {
  None,
  Lexical,
  FunctionInput,
  Unknown,
};

// Ownership answers who may transfer or free a value. Storage origin answers
// whether the value's object allocation itself may be freed. Stack-constructed
// values still need their destructor, but their backing storage belongs to the
// current stack frame.
enum class StorageOrigin { Unknown, Heap, Stack };

class Expr {
public:
  asmc::OpType op = asmc::Hard;
  std::string access;
  std::string type = "";
  asmc::Size size;
  bool passable = true;
  bool owned = false;
  bool needsDrop = false;
  StorageOrigin storageOrigin = StorageOrigin::Unknown;
  scope::ScopeId storageScope = 0;
  int stackObjectOffset = 0;
  // True when this expression is an owned value that may be transferred.
  // Owned variables remain lvalues until they are explicitly sold.
  bool transferable = false;
  bool transferExplicit = false;

  LoanProvenance loanProvenance = LoanProvenance::None;
  scope::ScopeId loanScope = 0;

  bool requiresImmutableBinding = false;
  std::string immutableBindingSource;
  void adoptImmutableRequirement(const Expr &other) {
    if (other.requiresImmutableBinding) {
      requiresImmutableBinding = true;
      immutableBindingSource = other.immutableBindingSource;
    }
  }
  Expr() = default;
};
} // namespace gen
