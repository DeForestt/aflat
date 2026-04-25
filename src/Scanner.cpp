#include "Scanner.hpp"

#include <cctype>

#include <iostream>
#include <memory>
#include <utility>

#include "Exceptions.hpp"

namespace lex {

namespace {

template <typename T>
T *stamp(T *token, int line, int column, size_t start, size_t end) {
  token->lineCount = line;
  token->column = column;
  token->length = static_cast<int>(end > start ? end - start : 1);
  return token;
}

} // namespace

struct Lexer::Impl {
  LinkedList<Token *> Scan(string input, int startLine);
};

LinkedList<Token *> Lexer::Impl::Scan(string input, int startLine) {
  LinkedList<lex::Token *> tokens = LinkedList<lex::Token *>();
  size_t i = 0;
  int lineCount = startLine;
  int columnCount = 1;

  while (i < input.length()) {
    if (input[i] == '\n') {
      ++lineCount;
      columnCount = 1;
      ++i;
      continue;
    }

    if (std::isspace(static_cast<unsigned char>(input[i]))) {
      ++i;
      ++columnCount;
      continue;
    }

    const int tokenLine = lineCount;
    const int tokenColumn = columnCount;
    const size_t tokenStart = i;

    if (std::isalpha(static_cast<unsigned char>(input[i])) || input[i] == '_') {
      auto *l_obj = new LObj();
      while (i < input.length() &&
             (std::isalpha(static_cast<unsigned char>(input[i])) ||
              std::isdigit(static_cast<unsigned char>(input[i])) ||
              input[i] == '_')) {
        l_obj->meta += input[i];
        ++i;
        ++columnCount;
      }
      tokens.push(stamp(l_obj, tokenLine, tokenColumn, tokenStart, i));
      continue;
    }

    if (std::isdigit(static_cast<unsigned char>(input[i])) || input[i] == '-') {
      if (input[i] == '-' &&
          (i + 1 >= input.length() ||
           !std::isdigit(static_cast<unsigned char>(input[i + 1])))) {
        if (i + 1 < input.length() && input[i + 1] == '>') {
          auto *sym = new lex::Symbol;
          sym->meta = "->";
          i += 2;
          columnCount += 2;
          tokens.push(stamp(sym, tokenLine, tokenColumn, tokenStart, i));
        } else {
          auto *sym = new lex::OpSym;
          sym->Sym = '-';
          ++i;
          ++columnCount;
          tokens << stamp(sym, tokenLine, tokenColumn, tokenStart, i);
        }
        continue;
      }

      if (input[i] == '0' && i + 1 < input.length() && input[i + 1] == 'x') {
        auto *intLit = new lex::INT();
        intLit->value = "0x";
        i += 2;
        columnCount += 2;
        while (i < input.length() &&
               std::isxdigit(static_cast<unsigned char>(input[i]))) {
          intLit->value += input[i];
          ++i;
          ++columnCount;
        }
        tokens << stamp(intLit, tokenLine, tokenColumn, tokenStart, i);
        continue;
      }

      if (input[i] == '0' && i + 1 < input.length() && input[i + 1] == 'o') {
        auto *intLit = new lex::INT();
        intLit->value = "0o";
        i += 2;
        columnCount += 2;
        while (i < input.length() && input[i] >= '0' && input[i] <= '7') {
          intLit->value += input[i];
          ++i;
          ++columnCount;
        }
        tokens << stamp(intLit, tokenLine, tokenColumn, tokenStart, i);
        continue;
      }

      auto *intLit = new lex::INT();
      intLit->value = input[i];
      ++i;
      ++columnCount;
      while (i < input.length() &&
             (std::isdigit(static_cast<unsigned char>(input[i])) ||
              input[i] == '.')) {
        intLit->value += input[i];
        ++i;
        ++columnCount;
      }

      if (intLit->value.find('.') != string::npos) {
        if (intLit->value.find('.') != intLit->value.rfind('.')) {
          throw err::Exception("Invalid token: " + intLit->value + " on line " +
                               std::to_string(tokenLine));
        }
        auto *floatLit = new lex::FloatLit();
        floatLit->value = intLit->value;
        delete intLit;
        tokens << stamp(floatLit, tokenLine, tokenColumn, tokenStart, i);
      } else {
        tokens << stamp(intLit, tokenLine, tokenColumn, tokenStart, i);
      }
      continue;
    }

    if (input[i] == '#') {
      auto *longLit = new lex::Long();
      ++i;
      ++columnCount;
      while (i < input.length() &&
             std::isdigit(static_cast<unsigned char>(input[i]))) {
        longLit->value += input[i];
        ++i;
        ++columnCount;
      }
      tokens.push(stamp(longLit, tokenLine, tokenColumn, tokenStart, i));
      continue;
    }

    if (input[i] == '"') {
      auto *stringObj = new StringObj();
      ++i;
      ++columnCount;
      while (i < input.length() && input[i] != '"') {
        if (input[i] == '\n') {
          ++lineCount;
          columnCount = 1;
          ++i;
          continue;
        }
        if (input[i] == '\\') {
          ++i;
          ++columnCount;
          if (i >= input.length())
            break;
          if (input[i] == 'n') {
            stringObj->value += "\\n";
          } else if (input[i] == 't') {
            stringObj->value += '\t';
          } else if (input[i] == '\\') {
            stringObj->value += 0x5C;
          } else if (input[i] == '"') {
            stringObj->value += "\\\"";
          } else if (input[i] == '\'') {
            stringObj->value += "\\'";
          } else if (input[i] == 'r') {
            stringObj->value += '\r';
          } else if (input[i] == '0') {
            stringObj->value += '\0';
          } else if (input[i] == 'e') {
            stringObj->value += 0x1B;
          } else {
            throw err::Exception("Invalid token: " + stringObj->value +
                                 " on line " + std::to_string(tokenLine));
          }
        } else if (input[i] != '\t' && input[i] != '\r') {
          stringObj->value += input[i];
        }
        ++i;
        ++columnCount;
      }
      if (i < input.length() && input[i] == '"') {
        ++i;
        ++columnCount;
      }
      tokens.push(stamp(stringObj, tokenLine, tokenColumn, tokenStart, i));
      continue;
    }

    if (input[i] == '`') {
      auto *stringObj = new FStringObj();
      ++i;
      ++columnCount;
      while (i < input.length() && input[i] != '`') {
        if (input[i] == '\n') {
          ++lineCount;
          columnCount = 1;
          ++i;
          continue;
        }
        if (input[i] == '\\') {
          ++i;
          ++columnCount;
          if (i >= input.length())
            break;
          if (input[i] == 'n') {
            stringObj->value += "\\n";
          } else if (input[i] == 't') {
            stringObj->value += '\t';
          } else if (input[i] == '\\') {
            stringObj->value += 0x5C;
          } else if (input[i] == '`') {
            stringObj->value += "`";
          } else if (input[i] == '\'') {
            stringObj->value += "\\'";
          } else if (input[i] == 'r') {
            stringObj->value += '\r';
          } else if (input[i] == '0') {
            stringObj->value += '\0';
          } else if (input[i] == 'e') {
            stringObj->value += 0x1B;
          } else {
            throw err::Exception("Invalid token: " + stringObj->value +
                                 " on line " + std::to_string(tokenLine));
          }
        } else if (input[i] != '\t' && input[i] != '\r') {
          stringObj->value += input[i];
        }
        ++i;
        ++columnCount;
      }
      if (i < input.length() && input[i] == '`') {
        ++i;
        ++columnCount;
      }
      tokens.push(stamp(stringObj, tokenLine, tokenColumn, tokenStart, i));
      continue;
    }

    if (input[i] == '~') {
      auto *stringObj = new lex::StringObj();
      ++i;
      ++columnCount;
      while (i < input.length() && input[i] != '~') {
        if (input[i] == '\n') {
          ++lineCount;
          columnCount = 1;
        } else {
          ++columnCount;
        }
        stringObj->value += input[i];
        ++i;
      }
      if (i < input.length() && input[i] == '~') {
        ++i;
        ++columnCount;
      }
      tokens.push(stamp(stringObj, tokenLine, tokenColumn, tokenStart, i));
      continue;
    }

    if (input[i] == '\'') {
      auto *charobj = new CharObj();
      ++i;
      ++columnCount;
      if (i < input.length() && input[i] == '\\') {
        ++i;
        ++columnCount;
        if (i >= input.length()) {
          throw err::Exception("Unterminated Char Value");
        }
        if (input[i] == 'n') {
          charobj->value = '\n';
        } else if (input[i] == 't') {
          charobj->value = '\t';
        } else if (input[i] == '\\') {
          charobj->value = '\\';
        } else if (input[i] == '"') {
          charobj->value = '"';
        } else if (input[i] == '\'') {
          charobj->value = '\'';
        } else if (input[i] == 'r') {
          charobj->value = '\r';
        } else if (input[i] == '0') {
          charobj->value = '\0';
        } else {
          throw err::Exception("Invalid token: on line " +
                               std::to_string(tokenLine));
        }
      } else if (i < input.length()) {
        charobj->value = input[i];
      }
      if (i < input.length()) {
        ++i;
        ++columnCount;
      }
      if (i >= input.length() || input[i] != '\'') {
        throw err::Exception("Unterminated Char Value");
      }
      ++i;
      ++columnCount;
      tokens << stamp(charobj, tokenLine, tokenColumn, tokenStart, i);
      continue;
    }

    if (input[i] == '?') {
      auto *ref = new lex::Ref;
      ++i;
      ++columnCount;
      tokens << stamp(ref, tokenLine, tokenColumn, tokenStart, i);
      continue;
    }

    if (input[i] == '=' && i + 1 < input.length() && input[i + 1] == '=') {
      auto *equ = new lex::Symbol;
      equ->meta = "==";
      i += 2;
      columnCount += 2;
      tokens << stamp(equ, tokenLine, tokenColumn, tokenStart, i);
      continue;
    }

    if (input[i] == '<') {
      if (i + 1 < input.length() && input[i + 1] == '<') {
        auto *op = new lex::OpSym;
        op->Sym = '<';
        i += 2;
        columnCount += 2;
        tokens << stamp(op, tokenLine, tokenColumn, tokenStart, i);
      } else if (i + 1 < input.length() && input[i + 1] == '=') {
        auto *sym = new lex::Symbol;
        sym->meta = "<=";
        i += 2;
        columnCount += 2;
        tokens << stamp(sym, tokenLine, tokenColumn, tokenStart, i);
      } else {
        auto *sym = new lex::Symbol;
        sym->meta = "<";
        ++i;
        ++columnCount;
        tokens << stamp(sym, tokenLine, tokenColumn, tokenStart, i);
      }
      continue;
    }

    if (input[i] == '>') {
      if (i + 1 < input.length() && input[i + 1] == '>') {
        auto *op = new lex::OpSym;
        op->Sym = '>';
        i += 2;
        columnCount += 2;
        tokens << stamp(op, tokenLine, tokenColumn, tokenStart, i);
      } else if (i + 1 < input.length() && input[i + 1] == '=') {
        auto *sym = new lex::Symbol;
        sym->meta = ">=";
        i += 2;
        columnCount += 2;
        tokens << stamp(sym, tokenLine, tokenColumn, tokenStart, i);
      } else {
        auto *sym = new lex::Symbol;
        sym->meta = ">";
        ++i;
        ++columnCount;
        tokens << stamp(sym, tokenLine, tokenColumn, tokenStart, i);
      }
      continue;
    }

    if (input[i] == '|') {
      if (i + 1 < input.length() && input[i + 1] == '|') {
        auto *sym = new lex::Symbol;
        sym->meta = "||";
        i += 2;
        columnCount += 2;
        tokens << stamp(sym, tokenLine, tokenColumn, tokenStart, i);
      } else {
        auto *op = new lex::OpSym;
        op->Sym = '|';
        ++i;
        ++columnCount;
        tokens << stamp(op, tokenLine, tokenColumn, tokenStart, i);
      }
      continue;
    }

    if (input[i] == '!') {
      if (i + 1 < input.length() && input[i + 1] == '=') {
        auto *sym = new lex::Symbol;
        sym->meta = "!=";
        i += 2;
        columnCount += 2;
        tokens << stamp(sym, tokenLine, tokenColumn, tokenStart, i);
      } else {
        auto *op = new lex::OpSym;
        op->Sym = '!';
        ++i;
        ++columnCount;
        tokens << stamp(op, tokenLine, tokenColumn, tokenStart, i);
      }
      continue;
    }

    if (input[i] == ':') {
      if (i + 1 < input.length() && input[i + 1] == ':') {
        auto *sym = new lex::Symbol;
        sym->meta = "::";
        i += 2;
        columnCount += 2;
        tokens << stamp(sym, tokenLine, tokenColumn, tokenStart, i);
      } else {
        auto *op = new OpSym;
        op->Sym = ':';
        ++i;
        ++columnCount;
        tokens << stamp(op, tokenLine, tokenColumn, tokenStart, i);
      }
      continue;
    }

    auto pushSingle = [&](char c) {
      auto *op = new OpSym;
      op->Sym = c;
      ++i;
      ++columnCount;
      tokens.push(stamp(op, tokenLine, tokenColumn, tokenStart, i));
    };

    switch (input[i]) {
    case ';':
    case '(':
    case ')':
    case '{':
    case '}':
    case ',':
    case '+':
    case '[':
    case ']':
    case '*':
    case '^':
    case '%':
    case '@':
    case '.':
    case '/':
    case '&':
    case '$':
      pushSingle(input[i]);
      continue;
    case '=': {
      auto *op = new OpSym;
      op->Sym = '=';
      ++i;
      ++columnCount;
      tokens << stamp(op, tokenLine, tokenColumn, tokenStart, i);
      continue;
    }
    default:
      throw err::Exception("Unknown token on line " +
                           std::to_string(tokenLine));
    }
  }

  auto *lastSemi = new lex::OpSym;
  lastSemi->Sym = ';';
  tokens.push(stamp(lastSemi, lineCount, columnCount, i, i + 1));
  return tokens;
}

Lexer::Lexer() : impl(std::make_unique<Impl>()) {}

Lexer::~Lexer() = default;

Lexer::Lexer(Lexer &&) noexcept = default;

Lexer &Lexer::operator=(Lexer &&) noexcept = default;

LinkedList<Token *> Lexer::Scan(string input, int startLine) {
  return impl->Scan(std::move(input), startLine);
}

} // namespace lex
