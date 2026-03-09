#pragma once
#include <string>

namespace compilerutils {

std::string buildCompileCmd(const std::string &srcPath,
                            const std::string &destPath, bool debug);

std::string buildLinkCmd(const std::string &output,
                         const std::string &linkerList, bool debug);

std::string buildCObjectCmd(const std::string &srcPath,
                            const std::string &destPath, bool debug);

std::string buildAssembleCmd(const std::string &asmPath,
                             const std::string &destPath, bool debug);

} // namespace compilerutils
