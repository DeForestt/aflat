#include "CodeGenerator/TargetInfo.hpp"

namespace gen {

const TargetInfo &currentTarget() {
  static const TargetInfo target{8, "x86-64"};
  return target;
}

} // namespace gen
