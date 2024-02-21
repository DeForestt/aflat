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