#include "Scanner.hpp"

#include <ctype.h>

#include <iostream>

#include "Exceptions.hpp"

LinkedList<lex::Token *> lex::Lexer::Scan(string input, int startLine) {
  LinkedList<lex::Token *> tokens = LinkedList<lex::Token *>();
  int i = 0;
  int lineCount = startLine;
  int lineStart = 0;
  auto advance = [&]() {
    if (input[i] == '\n') {
      i++;
      lineCount++;
      lineStart = i;
    } else {
      i++;
    }
  };
  while (i < input.length()) {
    if (input[i] == '\n') {
      advance();
      continue;
    }
    int startCol = i - lineStart;
    if (std::isalpha(input[i]) || input[i] == '_') {
      auto l_obj = new LObj();
      l_obj->meta = "";
      while (std::isalpha(input[i]) || std::isdigit(input[i]) ||
             input[i] == '_' || input[i] == '-') {
        l_obj->meta += input[i];
        advance();
      }
      l_obj->lineCount = lineCount;
      l_obj->columnStart = startCol;
      l_obj->columnEnd = i - lineStart - 1;
      tokens.push(l_obj);
    } else if (std::isdigit(input[i]) || input[i] == '-') {
      if (input[i] == '-' && !std::isdigit(input[i + 1])) {
        auto sym = new lex::OpSym;
        sym->Sym = '-';
        advance();
        sym->lineCount = lineCount;
        sym->columnStart = startCol;
        sym->columnEnd = i - lineStart - 1;
        tokens << sym;
      } else if (input[i] == '0' && input[i + 1] == 'x') {
        advance();
        advance();
        auto intLit = new lex::INT();
        intLit->value = "0x";
        while (std::isxdigit(input[i])) {
          intLit->value += input[i];
          advance();
        }
        intLit->lineCount = lineCount;
        intLit->columnStart = startCol;
        intLit->columnEnd = i - lineStart - 1;
        tokens << intLit;
      } else {
        auto IntLit = new lex::INT();
        IntLit->value = input[i];
        advance();

        while (std::isdigit(input[i]) || input[i] == '.') {
          IntLit->value += input[i];
          advance();
        }
        IntLit->lineCount = lineCount;
        IntLit->columnStart = startCol;
        IntLit->columnEnd = i - lineStart - 1;

        // Check if IntLit->value contains '.'
        if (IntLit->value.find('.') != string::npos) {
          // Check if IntLit->value contains more than one '.'
          if (IntLit->value.find('.') != IntLit->value.rfind('.')) {
            throw err::Exception("Invalid token: " + IntLit->value +
                                 " on line " + std::to_string(lineCount));
          }

          auto FloatLit = new lex::FloatLit();
          FloatLit->value = IntLit->value;
          FloatLit->lineCount = lineCount;
          FloatLit->columnStart = startCol;
          FloatLit->columnEnd = i - lineStart - 1;
          tokens << FloatLit;
        } else {
          tokens << IntLit;
        }
      }
    } else if (input[i] == '#') {
      auto *IntLit = new lex::Long();
      advance();

      while (std::isdigit(input[i])) {
        IntLit->value += input[i];
        advance();
      }
      IntLit->lineCount = lineCount;
      tokens.push(IntLit);
    } else if (std::isspace(input[i])) {
      advance();
    } else if (input[i] == '\"') {
      auto *stringObj = new StringObj();
      stringObj->value = "";
      advance();
      while (input[i] != '\"') {
        if (input[i] == '\n' || input[i] == '\t' || input[i] == '\r') {
          lineCount++;
          advance();
        }
        if (input[i] == '\\') {
          advance();
          if (input[i] == 'n') {
            stringObj->value += "\\n";
          } else if (input[i] == 't') {
            stringObj->value += '\t';
          } else if (input[i] == '\\') {
            stringObj->value += 0x5C;
          } else if (input[i] == '\"') {
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
                                 " on line " + std::to_string(lineCount));
          }
        } else if (input[i] != '\n' && input[i] != '\t' && input[i] != '\r') {
          stringObj->value += input[i];
        }
        advance();
      }
      advance();
      stringObj->lineCount = lineCount;
      tokens.push(stringObj);
    } else if (input[i] == '`') {
      auto *stringObj = new FStringObj();
      stringObj->value = "";
      advance();
      while (input[i] != '`') {
        if (input[i] == '\n' || input[i] == '\t' || input[i] == '\r') {
          lineCount++;
          advance();
        }
        if (input[i] == '\\') {
          advance();
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
                                 " on line " + std::to_string(lineCount));
          }
        } else if (input[i] != '\n' && input[i] != '\t' && input[i] != '\r') {
          stringObj->value += input[i];
        }
        advance();
      }
      advance();
      stringObj->lineCount = lineCount;
      tokens.push(stringObj);
    } else if (input[i] == '~') {
      auto *stringObj = new lex::StringObj();
      stringObj->value = "";
      advance();
      while (input[i] != '~') {
        // just copy the string exactly... no escaping
        if (input[i] == '\n' || input[i] == '\r') {
          lineCount++;
        }
        stringObj->value += input[i];
        advance();
      }
      advance();
      stringObj->lineCount = lineCount;
      tokens.push(stringObj);
    } else if (input[i] == '\'') {
      auto charobj = new CharObj();
      advance();
      if (input[i] == '\\') {
        advance();
        if (input[i] == 'n') {
          charobj->value = '\n';
        } else if (input[i] == 't') {
          charobj->value = '\t';
        } else if (input[i] == '\\') {
          charobj->value = '\\';
        } else if (input[i] == '\"') {
          charobj->value = '\"';
        } else if (input[i] == '\'') {
          charobj->value = '\'';
        } else if (input[i] == 'r') {
          charobj->value = '\r';
        } else if (input[i] == '0') {
          charobj->value = '\0';
        } else if (input[i] == '\\') {
          charobj->value = '\\';
        } else {
          throw err::Exception("Invalid token: on line " +
                               std::to_string(lineCount));
        }
      } else
        charobj->value = input[i];
      advance();
      if (input[i] != '\'') throw err::Exception("Unterminated Char Value");
      charobj->lineCount = lineCount;
      tokens << charobj;
      advance();
    } else if (input[i] == ';') {
      auto semi = new OpSym;
      semi->Sym = input[i];
      semi->lineCount = lineCount;
      semi->columnStart = startCol;
      advance();
      semi->columnEnd = i - lineStart - 1;
      tokens.push(semi);
    } else if (input[i] == '?') {
      auto Ref = new lex::Ref;
      Ref->lineCount = lineCount;
      Ref->columnStart = startCol;
      advance();
      Ref->columnEnd = i - lineStart - 1;
      tokens << Ref;
    } else if (input[i] == '(') {
      auto cPrenth = new OpSym;
      cPrenth->Sym = input[i];
      cPrenth->lineCount = lineCount;
      cPrenth->columnStart = startCol;
      advance();
      cPrenth->columnEnd = i - lineStart - 1;
      tokens.push(cPrenth);
    } else if (input[i] == ')') {
      auto cPrenth = new OpSym;
      cPrenth->Sym = input[i];
      cPrenth->lineCount = lineCount;
      cPrenth->columnStart = startCol;
      advance();
      cPrenth->columnEnd = i - lineStart - 1;
      tokens.push(cPrenth);
    } else if (input[i] == '{') {
      auto cPrenth = new OpSym;
      cPrenth->Sym = input[i];
      cPrenth->lineCount = lineCount;
      cPrenth->columnStart = startCol;
      advance();
      cPrenth->columnEnd = i - lineStart - 1;
      tokens.push(cPrenth);
    } else if (input[i] == '}') {
      lex::OpSym *cPrenth = new OpSym;
      cPrenth->Sym = input[i];
      cPrenth->lineCount = lineCount;
      cPrenth->columnStart = startCol;
      advance();
      cPrenth->columnEnd = i - lineStart - 1;
      tokens.push(cPrenth);
    } else if (input[i] == '=') {
      if (input[i + 1] == '=') {
        auto equ = new lex::Symbol;
        equ->meta = "==";
        advance();
        equ->lineCount = lineCount;
        equ->columnStart = startCol;
        equ->columnEnd = i - lineStart - 1;
        tokens << equ;
      } else {
        auto equ = new OpSym;
        equ->Sym = input[i];
        equ->lineCount = lineCount;
        equ->columnStart = startCol;
        advance();
        equ->columnEnd = i - lineStart - 1;
        tokens << equ;
      }
      // advance already called
    } else if (input[i] == '<') {
      auto sym = new lex::Symbol;
      sym->meta = "<";
      if (input[i + 1] == '<') {
        auto opSym = new lex::OpSym;
        opSym->Sym = '<';
        opSym->lineCount = lineCount;
        opSym->columnStart = startCol;
        advance();
        opSym->columnEnd = i - lineStart - 1;
        tokens << opSym;
        free(sym);
      } else if (input[i + 1] == '=') {
        sym->meta = "<=";
        sym->lineCount = lineCount;
        sym->columnStart = startCol;
        advance();
        sym->columnEnd = i - lineStart - 1;
        tokens << sym;
      } else {
        sym->lineCount = lineCount;
        sym->columnStart = startCol;
        advance();
        sym->columnEnd = i - lineStart - 1;
        tokens << sym;
      }
    } else if (input[i] == '>') {
      auto *sym = new lex::Symbol;
      sym->meta = ">";
      if (input[i + 1] == '>') {
        auto *opSym = new lex::OpSym;
        opSym->Sym = '>';
        opSym->lineCount = lineCount;
        opSym->columnStart = startCol;
        advance();
        opSym->columnEnd = i - lineStart - 1;
        free(sym);
        tokens << opSym;
      } else if (input[i + 1] == '=') {
        sym->meta = ">=";
        sym->lineCount = lineCount;
        sym->columnStart = startCol;
        advance();
        sym->columnEnd = i - lineStart - 1;
        tokens << sym;
      } else {
        sym->lineCount = lineCount;
        sym->columnStart = startCol;
        advance();
        sym->columnEnd = i - lineStart - 1;
        tokens << sym;
      }
    } else if (input[i] == '|') {
      auto *sym = new lex::OpSym;
      sym->Sym = '|';
      if (input[i + 1] == '|') {
        auto opSym = new lex::Symbol;
        opSym->meta = "||";
        opSym->lineCount = lineCount;
        opSym->columnStart = startCol;
        advance();
        opSym->columnEnd = i - lineStart - 1;
        free(sym);
        tokens << opSym;
      } else {
        sym->lineCount = lineCount;
        sym->columnStart = startCol;
        advance();
        sym->columnEnd = i - lineStart - 1;
        tokens << sym;
      }
    } else if (input[i] == '!') {
      if (input[i + 1] == '=') {
        auto equ = new lex::Symbol;
        equ->meta = "!=";
        advance();
        equ->lineCount = lineCount;
        equ->columnStart = startCol;
        equ->columnEnd = i - lineStart - 1;
        tokens << equ;
      } else {
        auto equ = new OpSym;
        equ->Sym = input[i];
        equ->lineCount = lineCount;
        equ->columnStart = startCol;
        advance();
        equ->columnEnd = i - lineStart - 1;
        tokens << equ;
      }
    } else if (input[i] == ',') {
      auto com = new OpSym;
      com->Sym = input[i];
      com->lineCount = lineCount;
      com->columnStart = startCol;
      advance();
      com->columnEnd = i - lineStart - 1;
      tokens.push(com);
    } else if (input[i] == '+') {
      auto add = new OpSym;
      add->Sym = input[i];
      add->lineCount = lineCount;
      add->columnStart = startCol;
      advance();
      add->columnEnd = i - lineStart - 1;
      tokens.push(add);
    } else if (input[i] == '[') {
      auto add = new OpSym;
      add->Sym = input[i];
      add->lineCount = lineCount;
      add->columnStart = startCol;
      advance();
      add->columnEnd = i - lineStart - 1;
      tokens.push(add);
    } else if (input[i] == ']') {
      auto add = new OpSym;
      add->Sym = input[i];
      add->lineCount = lineCount;
      add->columnStart = startCol;
      advance();
      add->columnEnd = i - lineStart - 1;
      tokens.push(add);
    } else if (input[i] == '*') {
      auto mul = new OpSym;
      mul->Sym = input[i];
      mul->lineCount = lineCount;
      mul->columnStart = startCol;
      advance();
      mul->columnEnd = i - lineStart - 1;
      tokens << mul;
    } else if (input[i] == '^') {
      auto carrot = new OpSym;
      carrot->Sym = input[i];
      carrot->lineCount = lineCount;
      carrot->columnStart = startCol;
      advance();
      carrot->columnEnd = i - lineStart - 1;
      tokens << carrot;
    } else if (input[i] == '%') {
      auto mul = new OpSym;
      mul->Sym = input[i];
      mul->lineCount = lineCount;
      mul->columnStart = startCol;
      advance();
      mul->columnEnd = i - lineStart - 1;
      tokens << mul;
    } else if (input[i] == ':') {
      auto col = new OpSym;
      col->Sym = input[i];
      col->lineCount = lineCount;
      col->columnStart = startCol;
      advance();
      col->columnEnd = i - lineStart - 1;
      tokens << col;
    } else if (input[i] == '@') {
      auto at = new OpSym;
      at->Sym = input[i];
      at->lineCount = lineCount;
      at->columnStart = startCol;
      advance();
      at->columnEnd = i - lineStart - 1;
      tokens << at;
    } else if (input[i] == '.') {
      auto mul = new OpSym;
      mul->Sym = input[i];
      mul->lineCount = lineCount;
      mul->columnStart = startCol;
      advance();
      mul->columnEnd = i - lineStart - 1;
      tokens << mul;
    } else if (input[i] == '/') {
      auto div = new OpSym;
      div->Sym = input[i];
      div->lineCount = lineCount;
      div->columnStart = startCol;
      advance();
      div->columnEnd = i - lineStart - 1;
      tokens << div;
    } else if (input[i] == '&') {
      auto andBit = new OpSym;
      andBit->Sym = input[i];
      andBit->lineCount = lineCount;
      andBit->columnStart = startCol;
      advance();
      andBit->columnEnd = i - lineStart - 1;
      tokens << andBit;
    } else if (input[i] == '$') {
      auto dollar = new OpSym;
      dollar->Sym = input[i];
      dollar->lineCount = lineCount;
      dollar->columnStart = startCol;
      advance();
      dollar->columnEnd = i - lineStart - 1;
      tokens << dollar;
    } else {
      /// Unknown token
      throw err::Exception("Unknown token on line " +
                           std::to_string(lineCount));
    }
  }
  auto last_semi = new lex::OpSym;
  last_semi->Sym = ';';
  last_semi->lineCount = lineCount;
  last_semi->columnStart = i - lineStart;
  last_semi->columnEnd = i - lineStart;
  tokens.push(last_semi);
  return tokens;
}