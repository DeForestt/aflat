#include "Utils.hpp"
#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <vector>

std::string utils::remove_char(std::string str, char ch) {
  str.erase(std::remove(str.begin(), str.end(), ch), str.end());
  return str;
}