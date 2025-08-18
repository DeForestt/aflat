#pragma once
#include <string>

#include "Parser/AST.hpp"

namespace utils {

std::string remove_char(std::string str, char ch);
std::string trim(const std::string &str);
std::string op_to_string(ast::Op op);

} // namespace utils