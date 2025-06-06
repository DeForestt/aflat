#include "Scanner.hpp"

#include <ctype.h>

#include <iostream>

#include "Exceptions.hpp"

LinkedList<lex::Token *> lex::Lexer::Scan(string input, int startLine) {
  LinkedList<lex::Token *> tokens = LinkedList<lex::Token *>();
  int i = 0;
  int lineCount = startLine;
  int lastBreak = 0;
  while (i < input.length()) {
    if (input[i] == '\n') {
      lineCount++;
      lastBreak = ++i;
      continue;
    }
    if (std::isspace(input[i])) {
      i++;
      continue;
    }
    int column = i - lastBreak + 1;
    if (std::isalpha(input[i]) || input[i] == '_') {
      std::string meta;
      while (std::isalpha(input[i]) || std::isdigit(input[i]) ||
             input[i] == '_' || input[i] == '-') {
        meta += input[i];
        i++;
      }
      tokens.push(new LObj(meta, lineCount, column));
    } else if (std::isdigit(input[i]) || input[i] == '-') {
      if (input[i] == '-' && !std::isdigit(input[i + 1])) {
        tokens << new lex::OpSym('-', lineCount, column);
        i++;
      } else if (input[i] == '0' && input[i + 1] == 'x') {
        i += 2;
        std::string val = "0x";
        while (std::isxdigit(input[i])) {
          val += input[i];
          i++;
        }
        tokens << new lex::INT(val, lineCount, column);
      } else {
        std::string val;
        val += input[i];
        i++;

        while (std::isdigit(input[i]) || input[i] == '.') {
          val += input[i];
          i++;
        }

        if (val.find('.') != string::npos) {
          if (val.find('.') != val.rfind('.')) {
            throw err::Exception("Invalid token: " + val +
                                 " on line " + std::to_string(lineCount));
          }

          tokens << new lex::FloatLit(val, lineCount, column);
        } else {
          tokens << new lex::INT(val, lineCount, column);
        }
      }
    } else if (input[i] == '#') {
      std::string val;
      i++;

      while (std::isdigit(input[i])) {
        val += input[i];
        i++;
      }
      tokens.push(new lex::Long(val, lineCount, column));
    } else if (input[i] == '\"') {
      std::string value;
      i++;
      while (input[i] != '\"') {
        if (input[i] == '\n' || input[i] == '\t' || input[i] == '\r') {
          lineCount++;
          lastBreak = ++i;
          continue;
        }
        if (input[i] == '\\') {
          i++;
          if (input[i] == 'n') {
            value += "\\n";
          } else if (input[i] == 't') {
            value += '\t';
          } else if (input[i] == '\\') {
            value += 0x5C;
          } else if (input[i] == '\"') {
            value += "\\\"";
          } else if (input[i] == '\'') {
            value += "\\'";
          } else if (input[i] == 'r') {
            value += '\r';
          } else if (input[i] == '0') {
            value += '\0';
          } else if (input[i] == 'e') {
            value += 0x1B;
          } else {
            throw err::Exception("Invalid token: " + value +
                                 " on line " + std::to_string(lineCount));
          }
        } else if (input[i] != '\n' && input[i] != '\t' && input[i] != '\r') {
          value += input[i];
        }
        i++;
      }
      i++;
      tokens.push(new StringObj(value, lineCount, column));
    } else if (input[i] == '`') {
      std::string value;
      i++;
      while (input[i] != '`') {
        if (input[i] == '\n' || input[i] == '\t' || input[i] == '\r') {
          lineCount++;
          lastBreak = ++i;
          continue;
        }
        if (input[i] == '\\') {
          i++;
          if (input[i] == 'n') {
            value += "\\n";
          } else if (input[i] == 't') {
            value += '\t';
          } else if (input[i] == '\\') {
            value += 0x5C;
          } else if (input[i] == '`') {
            value += "`";
          } else if (input[i] == '\'') {
            value += "\\'";
          } else if (input[i] == 'r') {
            value += '\r';
          } else if (input[i] == '0') {
            value += '\0';
          } else if (input[i] == 'e') {
            value += 0x1B;
          } else {
            throw err::Exception("Invalid token: " + value +
                                 " on line " + std::to_string(lineCount));
          }
        } else if (input[i] != '\n' && input[i] != '\t' && input[i] != '\r') {
          value += input[i];
        }
        i++;
      }
      i++;
      tokens.push(new FStringObj(value, lineCount, column));
    } else if (input[i] == '~') {
      std::string value;
      i++;
      while (input[i] != '~') {
        // just copy the string exactly... no escaping
        if (input[i] == '\n' || input[i] == '\r') {
          lineCount++;
        }
        value += input[i];
        i++;
      }
      i++;
      tokens.push(new lex::StringObj(value, lineCount, column));
    } else if (input[i] == '\'') {
      i++;
      char value;
      if (input[i] == '\\') {
        i++;
        if (input[i] == 'n') {
          value = '\n';
        } else if (input[i] == 't') {
          value = '\t';
        } else if (input[i] == '\\') {
          value = '\\';
        } else if (input[i] == '\"') {
          value = '\"';
        } else if (input[i] == '\'') {
          value = '\'';
        } else if (input[i] == 'r') {
          value = '\r';
        } else if (input[i] == '0') {
          value = '\0';
        } else if (input[i] == '\\') {
          value = '\\';
        } else {
          throw err::Exception("Invalid token: on line " +
                               std::to_string(lineCount));
        }
      } else {
        value = input[i];
      }
      i++;
      if (input[i] != '\'') throw err::Exception("Unterminated Char Value");
      tokens << new CharObj(value, lineCount, column);
      i++;
    } else if (input[i] == ';') {
      tokens.push(new OpSym(';', lineCount, column));
      i++;
    } else if (input[i] == '?') {
      tokens << new lex::Ref(lineCount, column);
      i++;
    } else if (input[i] == '(') {
      tokens.push(new OpSym('(', lineCount, column));
      i++;
    } else if (input[i] == ')') {
      tokens.push(new OpSym(')', lineCount, column));
      i++;
    } else if (input[i] == '{') {
      tokens.push(new OpSym('{', lineCount, column));
      i++;
    } else if (input[i] == '}') {
      tokens.push(new OpSym('}', lineCount, column));
      i++;
    } else if (input[i] == '=') {
      if (input[i + 1] == '=') {
        tokens << new lex::Symbol("==", lineCount, column);
        i++;
      } else {
        tokens << new OpSym('=', lineCount, column);
      }
      i++;
    } else if (input[i] == '<') {
      auto symCol = column;
      std::string meta = "<";
      if (input[i + 1] == '<') {
        tokens << new lex::OpSym('<', lineCount, column);
        i++;
      } else if (input[i + 1] == '=') {
        tokens << new lex::Symbol("<=", lineCount, column);
        i++;
      } else {
        tokens << new lex::Symbol(meta, lineCount, symCol);
      }
      i++;
    } else if (input[i] == '>') {
      auto symCol = column;
      std::string meta = ">";
      if (input[i + 1] == '>') {
        tokens << new lex::OpSym('>', lineCount, column);
        i++;
      } else if (input[i + 1] == '=') {
        tokens << new lex::Symbol(">=", lineCount, column);
        i++;
      } else {
        tokens << new lex::Symbol(meta, lineCount, symCol);
      }
      i++;
    } else if (input[i] == '|') {
      char op = '|';
      if (input[i + 1] == '|') {
        tokens << new lex::Symbol("||", lineCount, column);
        i++;
      } else {
        tokens << new lex::OpSym(op, lineCount, column);
      }
      i++;
    } else if (input[i] == '!') {
      if (input[i + 1] == '=') {
        tokens << new lex::Symbol("!=", lineCount, column);
        i++;
      } else {
        tokens << new OpSym('!', lineCount, column);
      }
      i++;
    } else if (input[i] == ',') {
      tokens.push(new OpSym(',', lineCount, column));
      i++;
    } else if (input[i] == '+') {
      tokens.push(new OpSym('+', lineCount, column));
      i++;
    } else if (input[i] == '[') {
      tokens.push(new OpSym('[', lineCount, column));
      i++;
    } else if (input[i] == ']') {
      tokens.push(new OpSym(']', lineCount, column));
      i++;
    } else if (input[i] == '*') {
      tokens << new OpSym('*', lineCount, column);
      i++;
    } else if (input[i] == '^') {
      tokens << new OpSym('^', lineCount, column);
      i++;
    } else if (input[i] == '%') {
      tokens << new OpSym('%', lineCount, column);
      i++;
    } else if (input[i] == ':') {
      tokens << new OpSym(':', lineCount, column);
      i++;
    } else if (input[i] == '@') {
      tokens << new OpSym('@', lineCount, column);
      i++;
    } else if (input[i] == '.') {
      tokens << new OpSym('.', lineCount, column);
      i++;
    } else if (input[i] == '/') {
      tokens << new OpSym('/', lineCount, column);
      i++;
    } else if (input[i] == '&') {
      tokens << new OpSym('&', lineCount, column);
      i++;
    } else if (input[i] == '$') {
      tokens << new OpSym('$', lineCount, column);
      i++;
    } else {
      /// Unknown token
      throw err::Exception("Unknown token on line " +
                           std::to_string(lineCount));
    }
  }
  tokens.push(new lex::OpSym(';', lineCount, 1));
  return tokens;
}