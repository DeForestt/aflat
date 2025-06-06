#include "Exceptions.hpp"

#include "Scanner.hpp"

err::Exception::Exception(std::string msg) { this->errorMsg = msg; }

std::string err::formatLoc(int line, int columnStart, int columnEnd) {
  std::string loc =
      "Line: " + std::to_string(line) + " Col: " + std::to_string(columnStart);
  if (columnEnd != columnStart) loc += "-" + std::to_string(columnEnd);
  return loc;
}

err::Exception::Exception(std::string msg, int line, int columnStart,
                          int columnEnd) {
  this->errorMsg = formatLoc(line, columnStart, columnEnd) + " " + msg;
}

err::Exception::Exception(std::string msg, lex::Token *tok)
    : Exception(msg, tok->lineCount, tok->columnStart, tok->columnEnd) {}
