#ifndef CODEGEN_SCOPE_SCOPE_TYPES_HPP
#define CODEGEN_SCOPE_SCOPE_TYPES_HPP

#include <cstdint>

namespace gen::scope {

using ScopeId = std::uint32_t;

struct ScopeFrame {
  ScopeId id;
  ScopeId parent;
  int symbolCount;
};

} // namespace gen::scope

#endif
