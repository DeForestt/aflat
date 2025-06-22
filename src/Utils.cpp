#include "Utils.hpp"

#include <unistd.h>

#include <algorithm>
#include <cstdarg>
#include <sstream>
#include <vector>

std::string utils::remove_char(std::string str, char ch) {
  str.erase(std::remove(str.begin(), str.end(), ch), str.end());
  return str;
}

std::string utils::trim(const std::string &str) {
  const auto begin = str.find_first_not_of(" \t\n\r");
  if (begin == std::string::npos) return "";
  const auto end = str.find_last_not_of(" \t\n\r");
  return str.substr(begin, end - begin + 1);
}

std::string utils::op_to_string(ast::Op op) {
  switch (op) {
    case ast::Op::Plus:
      return "+";
    case ast::Op::Minus:
      return "-";
    case ast::Op::Carrot:
      return "^";
    case ast::Op::Mod:
      return "%";
    case ast::Op::Mul:
      return "*";
    case ast::Op::Div:
      return "/";
    case ast::Op::Equ:
      return "==";
    case ast::Op::NotEqu:
      return "!=";
    case ast::Op::Less:
      return "<";
    case ast::Op::Great:
      return ">";
    case ast::Op::AndBit:
      return "&";
    case ast::Op::AndBool:
      return "&&";
    case ast::Op::OrBit:
      return "|";
    case ast::Op::LessCmp:
      return "<=";
    case ast::Op::GreatCmp:
      return ">=";
    case ast::Op::CompEqu:
      return "===";
    case ast::Op::Leq:
      return "<=";
    case ast::Op::Geq:
      return ">=";
  }
  return "";
}
