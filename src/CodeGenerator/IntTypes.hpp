#pragma once

#include <cstdint>
#include <string>

namespace gen::utils {

// The compiler's single source of truth for one canonical integer type:
// its width (target-derived for isize/usize), its signedness, and the
// range of values it can represent. Names resolve through
// integerTypeAlias() before lookup, so intTypeInfo("int") returns the
// i32 entry and intTypeInfo("adr") returns the usize entry.
struct IntTypeInfo {
  std::string canonical; // "i32"
  int widthBytes;        // target-derived for isize/usize
  bool isSigned;
  std::int64_t minSigned;    // valid when isSigned
  std::uint64_t maxUnsigned; // valid when !isSigned
};

// The metadata for the integer type a name denotes (aliases included),
// or nullptr when the name is not an integer type.
const IntTypeInfo *intTypeInfo(const std::string &name);

// The smallest value the type can represent (0 for unsigned types).
std::int64_t intTypeMin(const IntTypeInfo &info);
// The largest value the type can represent.
std::uint64_t intTypeMax(const IntTypeInfo &info);

} // namespace gen::utils
