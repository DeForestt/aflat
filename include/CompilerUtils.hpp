#pragma once
#include <string>

namespace compilerutils {

std::string buildCompileCmd(std::string_view srcPath,
                            std::string_view destPath, bool debug);

std::string buildLinkCmd(std::string_view output,
                         std::string_view linkerList, bool debug);

}  // namespace compilerutils
