#include "CompilerUtils.hpp"

namespace compilerutils {

namespace {
std::string toolchainDriver() {
#if defined(__APPLE__)
  return "clang";
#else
  return "gcc";
#endif
}

std::string archFlags() {
#if defined(__APPLE__) && defined(__aarch64__)
  return "-arch x86_64 ";
#else
  return "";
#endif
}

std::string hardeningFlags() {
#if defined(__APPLE__)
  return "";
#else
  return "-no-pie -z noexecstack ";
#endif
}
} // namespace

std::string buildCompileCmd(const std::string &srcPath,
                            const std::string &destPath, bool debug) {
  std::string flags;
  if (debug)
    flags = archFlags() + "-g " + hardeningFlags() + "-S -lefence ";
  else
    flags = archFlags() + "-O3 -march=native " + hardeningFlags() + "-S ";
  return toolchainDriver() + " " + flags + srcPath + " -o " + destPath;
}

std::string buildCObjectCmd(const std::string &srcPath,
                            const std::string &destPath, bool debug) {
  std::string flags;
  if (debug)
    flags = archFlags() + "-g -c " + hardeningFlags();
  else
    flags = archFlags() + "-O3 -c " + hardeningFlags();
  return toolchainDriver() + " " + flags + srcPath + " -o " + destPath;
}

std::string buildAssembleCmd(const std::string &asmPath,
                             const std::string &destPath, bool debug) {
  std::string flags = archFlags() + "-c ";
  if (debug)
    flags += "-g ";
  return toolchainDriver() + " " + flags + asmPath + " -o " + destPath;
}

std::string buildLinkCmd(const std::string &output,
                         const std::string &linkerList, bool debug) {
  std::string flags;
#if defined(__APPLE__)
  if (debug)
    flags = archFlags() + "-O0 -g -o ";
  else
    flags = archFlags() + "-O3 -o ";
#else
  if (debug)
    flags = "-O0 -g -no-pie -z noexecstack -o ";
  else
    flags = "-O3 -march=native -no-pie -z noexecstack -o ";
#endif
  return toolchainDriver() + " " + flags + output + " " + linkerList;
}

} // namespace compilerutils
