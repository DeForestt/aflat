#pragma once
#include <array>
#include <sstream>
#include <stdexcept>
#include <string>

#include "ASM.hpp"
#include "CodeGenerator/Types.hpp"

namespace gen::utils {
bool searchSymbol(gen::Symbol sym, std::string str);
int sizeToInt(asmc::Size size);

bool compareFunc(ast::Function F, std::string input);
ast::Statement *extract(
    std::string ident, ast::Statement *stmt, std::string id,
    std::unordered_map<std::string, std::string> &namespaceReplacemen);
ast::Statement *extract(std::string ident, ast::Statement *stmt);
ast::Sequence *extractAllFunctions(ast::Statement *stmt);
ast::Sequence *extractAllDeclarations(ast::Statement *stmt);
ast::Sequence *copyAllFunctionShells(ast::Statement *stmt);
void shellStatement(ast::Statement *stmt);
std::string getLibPath(std::string lib);
std::string generateUUID();

template <typename T>
std::string toString(const T &value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

template <typename... Args>
std::string format(std::string fmt, Args... args) {
  std::array<std::string, sizeof...(Args)> argStrings = {toString(args)...};
  std::stringstream result;
  std::size_t argIndex = 0;

  for (std::size_t i = 0; i < fmt.size(); ++i) {
    if (fmt[i] == '{' && i + 1 < fmt.size() && fmt[i + 1] == '}') {
      if (argIndex >= argStrings.size()) {
        throw std::runtime_error(
            "Not enough arguments provided to format string");
      }
      result << argStrings[argIndex++];
      i++;  // Skip the '}' character
    } else {
      result << fmt[i];
    }
  }

  if (argIndex < argStrings.size()) {
    throw std::runtime_error("Too many arguments provided to format string");
  }

  return result.str();
}
};  // namespace gen::utils
