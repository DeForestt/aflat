#include "CompilerUtils.hpp"

namespace compilerutils {

std::string buildCompileCmd(const std::string &srcPath,
                            const std::string &destPath, bool debug) {
  std::string flags;
  if (debug)
    flags = "-g -no-pie -z noexecstack -S -lefence ";
  else
    flags = "-O3 -march=native -S -no-pie -z noexecstack ";
  return "gcc " + flags + srcPath + " -o " + destPath;
}

std::string buildLinkCmd(const std::string &output,
                         const std::string &linkerList, bool debug) {
  std::string flags;
  if (debug)
    flags = "-O0 -g -no-pie -z noexecstack -o ";
  else
    flags = "-O3 -march=native -no-pie -z noexecstack -o ";
  return "gcc " + flags + output + " " + linkerList;
}

} // namespace compilerutils
