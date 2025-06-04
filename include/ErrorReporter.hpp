#ifndef ERROR_REPORTER_HPP
#define ERROR_REPORTER_HPP

#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>

namespace error {

inline int extractLine(const std::string &msg) {
  std::regex re("(?:[Ll]ine:?\\s*)([0-9]+)");
  std::smatch m;
  if (std::regex_search(msg, m, re)) {
    return std::stoi(m[1].str());
  }
  return -1;
}

inline std::string getLine(const std::string &source, int line) {
  if (line < 0) return "";
  std::istringstream iss(source);
  std::string l;
  for (int i = 1; i <= line && std::getline(iss, l); ++i) {
  }
  return l;
}

inline void report(const std::string &file, int line, const std::string &msg,
                   const std::string &source = "") {
  std::cout << "\033[1;31merror:\033[0m ";
  if (!file.empty()) {
    std::cout << file;
    if (line > 0) std::cout << ":" << line;
    std::cout << ": ";
  }
  std::cout << msg << "\n";
  if (!file.empty() && line > 0) {
    std::cout << " --> " << file << ":" << line << "\n";
    std::cout << "  |" << std::endl;
  }
  if (line > 0 && !source.empty()) {
    std::string prev = getLine(source, line - 1);
    std::string codeLine = getLine(source, line);
    std::string next = getLine(source, line + 1);
    constexpr int width = 4;  // digit width for line numbers
    auto printLine = [&](char mark, int ln, const std::string &txt) {
      std::cout << mark << std::setw(width) << ln << " | " << txt << "\n";
    };
    if (!prev.empty()) printLine(' ', line - 1, prev);
    printLine('>', line, codeLine);
    if (!next.empty()) printLine(' ', line + 1, next);
    std::cout << "  |" << std::endl << std::endl;
  }
}

inline void warn(const std::string &file, int line, const std::string &msg,
                 const std::string &source = "") {
  std::cout << "\033[1;33mwarning:\033[0m ";
  if (!file.empty()) {
    std::cout << file;
    if (line > 0) std::cout << ":" << line;
    std::cout << ": ";
  }
  std::cout << msg << "\n";
  if (!file.empty() && line > 0) {
    std::cout << " --> " << file << ":" << line << "\n";
    std::cout << "  |" << std::endl;
  }
  if (line > 0 && !source.empty()) {
    std::string prev = getLine(source, line - 1);
    std::string codeLine = getLine(source, line);
    std::string next = getLine(source, line + 1);
    constexpr int width = 4;  // digit width for line numbers
    auto printLine = [&](char mark, int ln, const std::string &txt) {
      std::cout << mark << std::setw(width) << ln << " | " << txt << "\n";
    };
    if (!prev.empty()) printLine(' ', line - 1, prev);
    printLine('>', line, codeLine);
    if (!next.empty()) printLine(' ', line + 1, next);
    std::cout << "  |" << std::endl << std::endl;
  }
}

}  // namespace error

#endif  // ERROR_REPORTER_HPP
