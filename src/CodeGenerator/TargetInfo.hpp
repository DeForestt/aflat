#pragma once

#include <string>

namespace gen {

// The compilation target the compiler emits code for. Today there is a
// single target (x86-64); this descriptor is the seam a future target
// option plugs into. Integer widths that depend on the target (isize,
// usize, adr) are derived from it, never from the compiler host.
struct TargetInfo {
  int pointerWidthBytes; // 8 on the current target
  std::string name;      // "x86-64"
};

const TargetInfo &currentTarget();

} // namespace gen
