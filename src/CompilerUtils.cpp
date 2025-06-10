#include "CompilerUtils.hpp"
#include <string_view>

namespace compilerutils {

std::string buildCompileCmd(std::string_view srcPath,
                            std::string_view destPath,
                            bool debug) {
  const std::string flags =
      debug ? "-g -no-pie -z noexecstack -S -lefence "
            : "-O3 -march=native -S -no-pie -z noexecstack ";
  return std::string{"gcc "} + flags + std::string{srcPath} + " -o " +
         std::string{destPath};
}

std::string buildLinkCmd(std::string_view output,
                         std::string_view linkerList,
                         bool debug) {
  const std::string flags =
      debug ? "-O0 -g -no-pie -z noexecstack -o "
            : "-O3 -march=native -no-pie -z noexecstack -o ";
  return std::string{"gcc "} + flags + std::string{output} + " " +
         std::string{linkerList};
}

}  // namespace compilerutils
