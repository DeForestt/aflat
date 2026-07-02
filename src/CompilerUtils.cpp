#include "CompilerUtils.hpp"

#include <cstdlib>

namespace compilerutils {

std::string releaseCpuFlags() {
  const char *portable = std::getenv("AFLAT_PORTABLE_OUTPUT");
  if (portable != nullptr && portable[0] != '\0' && portable[0] != '0')
    return "-march=x86-64 -mtune=generic";
  return "-march=native";
}

std::string buildCompileCmd(const std::string &srcPath,
                            const std::string &destPath, bool debug) {
  std::string flags;
  if (debug)
    flags = "-g -no-pie -z noexecstack -S -lefence ";
  else
    flags = "-O3 " + releaseCpuFlags() + " -S -no-pie -z noexecstack ";
  return "gcc " + flags + srcPath + " -o " + destPath;
}

std::string buildLinkCmd(const std::string &output,
                         const std::string &linkerList, bool debug) {
  std::string flags;
  if (debug)
    flags = "-O0 -g -no-pie -z noexecstack -o ";
  else
    flags = "-O3 " + releaseCpuFlags() + " -no-pie -z noexecstack -o ";
  return "gcc " + flags + output + " " + linkerList;
}

} // namespace compilerutils
