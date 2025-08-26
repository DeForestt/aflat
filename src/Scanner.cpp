#include "Scanner.hpp"

#include <ctype.h>

#include <iostream>
#include <memory>

#include "Exceptions.hpp"

LinkedList<std::shared_ptr<lex::Token>> lex::Lexer::Scan(string input,
                                                         int startLine) {
  LinkedList<std::shared_ptr<lex::Token>> tokens =
      LinkedList<std::shared_ptr<lex::Token>>();
  int i = 0;
  int lineCount = startLine;
  while (i < input.length()) {
    if (input[i] == '\n')
      lineCount++;
    if (std::isalpha(input[i]) || input[i] == '_') {
      auto l_obj = std::make_shared<LObj>();
      l_obj->meta = "";
      while (std::isalpha(input[i]) || std::isdigit(input[i]) ||
             input[i] == '_') {
        l_obj->meta += input[i];
        i++;
      }
      l_obj->lineCount = lineCount;
      tokens.push(l_obj);
    } else if (std::isdigit(input[i]) || input[i] == '-') {
      if (input[i] == '-' && !std::isdigit(input[i + 1])) {
        if (input[i + 1] == '>') {
          auto sym = std::make_shared<lex::Symbol>();
          sym->meta = "->";
          i += 2;
          sym->lineCount = lineCount;
          tokens << sym;
        } else {
          auto sym = std::make_shared<lex::OpSym>();
          sym->Sym = '-';
          i++;
          sym->lineCount = lineCount;
          tokens << sym;
        }
      } else if (input[i] == '0' && input[i + 1] == 'x') {
        i += 2;
        auto intLit = std::make_shared<lex::INT>();
        intLit->value = "0x";
        while (std::isxdigit(input[i])) {
          intLit->value += input[i];
          i++;
        }
        intLit->lineCount = lineCount;
        tokens << intLit;
      } else if (input[i] == '0' && input[i + 1] == 'o') {
        i += 2;
        auto intLit = std::make_shared<lex::INT>();
        intLit->value = "0o";
        while (input[i] >= '0' && input[i] <= '7') {
          intLit->value += input[i];
          i++;
        }
        intLit->lineCount = lineCount;
        tokens << intLit;
      } else {
        auto IntLit = std::make_shared<lex::INT>();
        IntLit->value = input[i];
        i++;

        while (std::isdigit(input[i]) || input[i] == '.') {
          IntLit->value += input[i];
          i++;
        }
        IntLit->lineCount = lineCount;

        // Check if IntLit->value contains '.'
        if (IntLit->value.find('.') != string::npos) {
          // Check if IntLit->value contains more than one '.'
          if (IntLit->value.find('.') != IntLit->value.rfind('.')) {
            throw err::Exception("Invalid token: " + IntLit->value +
                                 " on line " + std::to_string(lineCount));
          }

          auto FloatLit = std::make_shared<lex::FloatLit>();
          FloatLit->value = IntLit->value;
          FloatLit->lineCount = lineCount;
          tokens << FloatLit;
        } else
          tokens << IntLit;
      }
    } else if (input[i] == '#') {
      auto IntLit = std::make_shared<lex::Long>();
      i++;

      while (std::isdigit(input[i])) {
        IntLit->value += input[i];
        i++;
      }
      IntLit->lineCount = lineCount;
      tokens.push(IntLit);
    } else if (std::isspace(input[i])) {
      i++;
    } else if (input[i] == '\"') {
      auto stringObj = std::make_shared<StringObj>();
      stringObj->value = "";
      i++;
      while (input[i] != '\"') {
        if (input[i] == '\n' || input[i] == '\t' || input[i] == '\r') {
          lineCount++;
          i++;
        }
        if (input[i] == '\\') {
          i++;
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
        i++;
      }
      i++;
      stringObj->lineCount = lineCount;
      tokens.push(stringObj);
    } else if (input[i] == '`') {
      auto stringObj = std::make_shared<FStringObj>();
      stringObj->value = "";
      i++;
      while (input[i] != '`') {
        if (input[i] == '\n' || input[i] == '\t' || input[i] == '\r') {
          lineCount++;
          i++;
        }
        if (input[i] == '\\') {
          i++;
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
        i++;
      }
      i++;
      stringObj->lineCount = lineCount;
      tokens.push(stringObj);
    } else if (input[i] == '~') {
      auto stringObj = std::make_shared<lex::StringObj>();
      stringObj->value = "";
      i++;
      while (input[i] != '~') {
        // just copy the string exactly... no escaping
        if (input[i] == '\n' || input[i] == '\r') {
          lineCount++;
        }
        stringObj->value += input[i];
        i++;
      }
      i++;
      stringObj->lineCount = lineCount;
      tokens.push(stringObj);
    } else if (input[i] == '\'') {
      auto charobj = std::make_shared<CharObj>();
      i++;
      if (input[i] == '\\') {
        i++;
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
      i++;
      if (input[i] != '\'')
        throw err::Exception("Unterminated Char Value");
      charobj->lineCount = lineCount;
      tokens << charobj;
      i++;
    } else if (input[i] == ';') {
      auto semi = std::make_shared<OpSym>();
      semi->Sym = input[i];
      semi->lineCount = lineCount;
      tokens.push(semi);
      i++;
    } else if (input[i] == '?') {
      auto Ref = std::make_shared<lex::Ref>();
      Ref->lineCount = lineCount;
      tokens << Ref;
      i++;
    } else if (input[i] == '(') {
      auto cPrenth = std::make_shared<OpSym>();
      cPrenth->Sym = input[i];
      cPrenth->lineCount = lineCount;
      tokens.push(cPrenth);
      i++;
    } else if (input[i] == ')') {
      auto cPrenth = std::make_shared<OpSym>();
      cPrenth->Sym = input[i];
      cPrenth->lineCount = lineCount;
      tokens.push(cPrenth);
      i++;
    } else if (input[i] == '{') {
      auto cPrenth = std::make_shared<OpSym>();
      cPrenth->Sym = input[i];
      cPrenth->lineCount = lineCount;
      tokens.push(cPrenth);
      i++;
    } else if (input[i] == '}') {
      auto cPrenth = std::make_shared<OpSym>();
      cPrenth->Sym = input[i];
      cPrenth->lineCount = lineCount;
      tokens.push(cPrenth);
      i++;
    } else if (input[i] == '=') {
      if (input[i + 1] == '=') {
        auto equ = std::make_shared<lex::Symbol>();
        equ->meta = "==";
        i++;
        equ->lineCount = lineCount;
        tokens << equ;
      } else {
        auto equ = std::make_shared<OpSym>();
        equ->Sym = input[i];
        equ->lineCount = lineCount;
        tokens << equ;
      }
      i++;
    } else if (input[i] == '<') {
      auto sym = std::make_shared<lex::Symbol>();
      sym->meta = "<";
      if (input[i + 1] == '<') {
        auto opSym = std::make_shared<lex::OpSym>();
        opSym->Sym = '<';
        opSym->lineCount = lineCount;
        tokens << opSym;

        i++;
      } else if (input[i + 1] == '=') {
        sym->meta = "<=";
        sym->lineCount = lineCount;
        tokens << sym;
        i++;
      } else {
        sym->lineCount = lineCount;
        tokens << sym;
      }
      i++;
    } else if (input[i] == '>') {
      auto sym = std::make_shared<lex::Symbol>();
      sym->meta = ">";
      if (input[i + 1] == '>') {
        auto opSym = std::make_shared<lex::OpSym>();
        opSym->Sym = '>';
        opSym->lineCount = lineCount;
        tokens << opSym;
        i++;
      } else if (input[i + 1] == '=') {
        sym->meta = ">=";
        sym->lineCount = lineCount;
        tokens << sym;
        i++;
      } else {
        sym->lineCount = lineCount;
        tokens << sym;
      }
      i++;
    } else if (input[i] == '|') {
      auto sym = std::make_shared<lex::OpSym>();
      sym->Sym = '|';
      if (input[i + 1] == '|') {
        auto opSym = std::make_shared<lex::Symbol>();
        opSym->meta = "||";
        opSym->lineCount = lineCount;
        tokens << opSym;
        i++;
      } else {
        sym->lineCount = lineCount;
        tokens << sym;
      }
      i++;
    } else if (input[i] == '!') {
      if (input[i + 1] == '=') {
        auto equ = std::make_shared<lex::Symbol>();
        equ->meta = "!=";
        i++;
        equ->lineCount = lineCount;
        tokens << equ;
      } else {
        auto equ = std::make_shared<OpSym>();
        equ->Sym = input[i];
        equ->lineCount = lineCount;
        tokens << equ;
      }
      i++;
    } else if (input[i] == ',') {
      auto com = std::make_shared<OpSym>();
      com->Sym = input[i];
      com->lineCount = lineCount;
      tokens.push(com);
      i++;
    } else if (input[i] == '+') {
      auto add = std::make_shared<OpSym>();
      add->Sym = input[i];
      add->lineCount = lineCount;
      tokens.push(add);
      i++;
    } else if (input[i] == '[') {
      auto add = std::make_shared<OpSym>();
      add->Sym = input[i];
      add->lineCount = lineCount;
      tokens.push(add);
      i++;
    } else if (input[i] == ']') {
      auto add = std::make_shared<OpSym>();
      add->Sym = input[i];
      add->lineCount = lineCount;
      tokens.push(add);
      i++;
    } else if (input[i] == '*') {
      auto mul = std::make_shared<OpSym>();
      mul->Sym = input[i];
      mul->lineCount = lineCount;
      tokens << mul;
      i++;
    } else if (input[i] == '^') {
      auto carrot = std::make_shared<OpSym>();
      carrot->Sym = input[i];
      carrot->lineCount = lineCount;
      tokens << carrot;
      i++;
    } else if (input[i] == '%') {
      auto mul = std::make_shared<OpSym>();
      mul->Sym = input[i];
      mul->lineCount = lineCount;
      tokens << mul;
      i++;
    } else if (input[i] == ':') {
      auto col = std::make_shared<OpSym>();
      col->Sym = input[i];
      col->lineCount = lineCount;
      i++;
      if (input[i] == ':') {
        auto sym = std::make_shared<lex::Symbol>();
        sym->meta = "::";
        sym->lineCount = lineCount;
        tokens << sym;
        i++;
      } else {
        tokens << col;
      }
    } else if (input[i] == '@') {
      auto at = std::make_shared<OpSym>();
      at->Sym = input[i];
      at->lineCount = lineCount;
      tokens << at;
      i++;
    } else if (input[i] == '.') {
      auto mul = std::make_shared<OpSym>();
      mul->Sym = input[i];
      mul->lineCount = lineCount;
      tokens << mul;
      i++;
    } else if (input[i] == '/') {
      auto div = std::make_shared<OpSym>();
      div->Sym = input[i];
      div->lineCount = lineCount;
      tokens << div;
      i++;
    } else if (input[i] == '&') {
      auto andBit = std::make_shared<OpSym>();
      andBit->Sym = input[i];
      andBit->lineCount = lineCount;
      tokens << andBit;
      i++;
    } else if (input[i] == '$') {
      auto dollar = std::make_shared<OpSym>();
      dollar->Sym = input[i];
      dollar->lineCount = lineCount;
      tokens << dollar;
      i++;
    } else {
      /// Unknown token
      throw err::Exception("Unknown token on line " +
                           std::to_string(lineCount));
    }
  }
  auto last_semi = std::make_shared<lex::OpSym>();
  last_semi->Sym = ';';
  last_semi->lineCount = lineCount;
  tokens.push(last_semi);
  return tokens;
}
