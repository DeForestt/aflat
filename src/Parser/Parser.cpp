#include "Parser/Parser.hpp"

#include <iostream>

#include "Exceptions.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"

ast::Expr *prioritizeExpr(ast::Expr *expr);

parse::Parser::Parser(int mutability = 0) {
  this->typeList.foo = ast::Type::compare;
  this->mutability = mutability;

  this->addType("int", asmc::Hard, asmc::DWord);
  this->addType("short", asmc::Hard, asmc::Word);
  this->addType("char", asmc::Hard, asmc::Byte);
  this->addType("long", asmc::Hard, asmc::QWord);
  this->addType("adr", asmc::Hard, asmc::QWord);
  this->addType("byte", asmc::Hard, asmc::Byte);
  this->addType("float", asmc::Float, asmc::DWord);
  this->addType("bool", asmc::Hard, asmc::Byte);
  this->addType("generic", asmc::Hard, asmc::QWord, true);
  this->addType("void", asmc::Hard, asmc::QWord);
  this->addType("any", asmc::Hard, asmc::QWord, true);
  // create a dummy type for let
  this->addType("let", asmc::Hard, asmc::QWord, true);
}

void parse::Parser::addType(std::string name, asmc::OpType opType,
                            asmc::Size size) {
  ast::Type type;
  type.typeName = name;
  type.opType = opType;
  type.size = size;

  this->typeList << type;
}

void parse::Parser::addType(std::string name, asmc::OpType opType,
                            asmc::Size size, bool isGeneric) {
  ast::Type type = ast::Type(name, size);
  type.opType = opType;
  type.isGeneric = isGeneric;

  this->typeList << type;
}

int getOpPriority(ast::Op op) {
  switch (op) {
    case ast::AndBit:
    case ast::OrBit:
    case ast::Great:
    case ast::Less:
      return 1;
    case ast::CompEqu:
    case ast::NotEqu:
    case ast::LessCmp:
    case ast::GreatCmp:
    case ast::Leq:
    case ast::Geq:
      return 2;
    case ast::Plus:
    case ast::Minus:
      return 3;
    case ast::Mul:
    case ast::Div:
      return 4;
    case ast::Mod:
    case ast::Carrot:
      return 5;
    default:
      throw err::Exception("Unknown operator");
  }
};

/*
 * function name: Parser::parseStmt
 * description: Recursive function that parses a statement and returns the AST
 * as a Statement parameters: LinkedList<Token> &tokens - the list of tokens to
 * parse return: AST::Statement - the parsed statement
 */
ast::Statement *parse::Parser::parseStmt(
    links::LinkedList<lex::Token *> &tokens, bool singleStmt = false) {
  ast::Statement *output = new ast::Statement;
  if (tokens.head == nullptr) {
    return output;
  };
  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto obj = *dynamic_cast<lex::LObj *>(tokens.peek());
    auto safeType = false;
    auto dynamicType = false;
    auto scope = ast::Public;
    tokens.pop();

    bool isMutable = true;
    if (this->mutability == 0)
      isMutable = true;
    else
      isMutable = false;

    if (obj.meta == "safe" || obj.meta == "dynamic") {
      while (obj.meta == "safe" || obj.meta == "dynamic") {
        if (obj.meta == "safe")
          safeType = true;
        else if (obj.meta == "dynamic")
          dynamicType = true;
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
          obj = *dynamic_cast<lex::LObj *>(tokens.peek());
          tokens.pop();
        } else {
          throw err::Exception("Expected type after safe/dynamic on line " +
                               std::to_string(obj.lineCount));
        }
      }

      if (obj.meta != "class") {
        throw err::Exception("Can only specify a class as safe " +
                             std::to_string(obj.lineCount));
      };
    }

    if (obj.meta == "const" || obj.meta == "mutable" || obj.meta == "public" ||
        obj.meta == "private" || obj.meta == "static" || obj.meta == "export") {
      while (obj.meta == "const" || obj.meta == "mutable" ||
             obj.meta == "public" || obj.meta == "private" ||
             obj.meta == "static" || obj.meta == "export") {
        if (obj.meta == "const") {
          isMutable = false;
        } else if (obj.meta == "mutable") {
          isMutable = true;
        } else if (obj.meta == "public") {
          scope = ast::Public;
        } else if (obj.meta == "private") {
          scope = ast::Private;
        } else if (obj.meta == "static") {
          scope = ast::Static;
        } else if (obj.meta == "export") {
          scope = ast::Export;
        }
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
          obj = *dynamic_cast<lex::LObj *>(tokens.peek());
          tokens.pop();
        } else {
          throw err::Exception("Expected type after memory modifier on line " +
                               std::to_string(obj.lineCount));
        }
      };
    }

    if (obj.meta == "const") {
      isMutable = false;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statement after public on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "mutable") {
      if (mutability == 2)
        throw err::Exception(
            "Cannot use a mutable variable in safe "
            "mode, please set config "
            "mutability to promiscuous or strict");
      isMutable = true;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statement after public on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    };
    // check if the next token is a scope modifier
    if (obj.meta == "public") {
      // set the scope modifier to public
      scope = ast::Public;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statement after public on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "private") {
      // set the scope modifier to private
      scope = ast::Private;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statement after private one line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "static") {
      // set the scope modifier to static
      scope = ast::Static;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statement after static on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "export") {
      // set the scope modifier to static
      scope = ast::Export;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statement after static on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    }

    // Declare a variable
    if (typeList[obj.meta] != nullptr) {
      // check if we need to make a function pointer
      const auto sym = dynamic_cast<lex::Symbol *>(tokens.peek());
      const auto type = (sym != nullptr && sym->meta == "<")
                            ? this->parseFPointerType(tokens, obj.meta)
                            : *this->typeList[obj.meta];

      // ensures the the current token is an Ident
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        const auto ident = *dynamic_cast<lex::LObj *>(tokens.pop());

        auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (sym && sym->Sym != ';') {
          auto sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
          auto overload = ast::Op::None;
          std::string scopeName = "global";
          if (sym.Sym == '<') {
            tokens.pop();
            auto *next = dynamic_cast<lex::OpSym *>(tokens.peek());
            if (next != nullptr) {
              if (next->Sym == '>') {
                overload = ast::Op::Less;
              } else if (next->Sym == '<') {
                overload = ast::Op::Great;
              } else if (next->Sym == '+') {
                overload = ast::Op::Plus;
              } else if (next->Sym == '-') {
                overload = ast::Op::Minus;
              } else if (next->Sym == '*') {
                overload = ast::Op::Mul;
              } else if (next->Sym == '^') {
                overload = ast::Op::Carrot;
              } else if (next->Sym == '/') {
                overload = ast::Op::Div;
              } else if (next->Sym == '%') {
                overload = ast::Op::Mod;
              } else if (next->Sym == '!') {
                overload = ast::Op::NotEqu;
              } else if (next->Sym == '&') {
                overload = ast::Op::AndBit;
              } else if (next->Sym == '|') {
                overload = ast::Op::OrBit;
              } else if (next->Sym == '=') {
                overload = ast::Op::Equ;
              } else {
                throw err::Exception("Expected and overload operator on line " +
                                     std::to_string(obj.lineCount));
              }
            } else {
              auto nextSym = dynamic_cast<lex::Symbol *>(tokens.peek());
              if (nextSym == nullptr)
                throw err::Exception("Expected and overload operator on line " +
                                     std::to_string(obj.lineCount));
              if (nextSym->meta == "==") {
                overload = ast::Op::CompEqu;
              } else if (nextSym->meta == "!=") {
                overload = ast::Op::NotEqu;
              } else if (nextSym->meta == ">=") {
                overload = ast::Op::Geq;
              } else if (nextSym->meta == "<=") {
                overload = ast::Op::Leq;
              } else if (nextSym->meta == ">") {
                overload = ast::Op::GreatCmp;
              } else if (nextSym->meta == "<") {
                overload = ast::Op::LessCmp;
              } else {
                throw err::Exception("Expected and overload operator on line " +
                                     std::to_string(obj.lineCount));
              }
            }
            tokens.pop();
            next = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (next == nullptr)
              throw err::Exception("Expected a close operator " +
                                   std::to_string(obj.lineCount));
            if (next->Sym != '>')
              throw err::Exception("Expected a close operator  " +
                                   std::to_string(obj.lineCount) + +" got " +
                                   next->Sym);
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
              throw err::Exception("Expected a symbol on line " +
                                   std::to_string(obj.lineCount));
            sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
          }
          if (sym.Sym == '@') {
            tokens.pop();
            if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
              auto scopeObj = *dynamic_cast<lex::LObj *>(tokens.pop());
              scopeName = scopeObj.meta;
            } else
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  "Expected Scope Name");
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Can Only Scope a Function");
            sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
            if (sym.Sym != '(')
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Can Only Scope a Function");
          }
          // Checking for Parenthesis to see if it is a function
          if (sym.Sym == '(') {
            tokens.pop();
            output = new ast::Function(ident.meta, scope, type, overload,
                                       scopeName, tokens, *this);
          } else if (sym.Sym == '=') {
            tokens.pop();
            output = new ast::DecAssign(
                new ast::Declare(ident.meta, scope, obj.meta, isMutable, type),
                isMutable, tokens, *this);
          }
        } else {
          output =
              new ast::Declare(ident.meta, scope, obj.meta, isMutable, type);
        }
      } else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
        auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        links::LinkedList<ast::Expr *> indices;
        if (sym->Sym == '[') {
          while (sym != nullptr && sym->Sym == '[') {
            tokens.pop();
            indices.push(this->parseExpr(tokens));
            auto closeBracket = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (closeBracket == nullptr || closeBracket->Sym != ']')
              throw err::Exception("Expected a close bracket on line " +
                                   std::to_string(obj.lineCount));
            sym = dynamic_cast<lex::OpSym *>(tokens.peek());
          }
          auto arr = new ast::DecArr;
          auto ident = dynamic_cast<lex::LObj *>(tokens.pop());
          if (ident == nullptr)
            throw err::Exception("Expected a name on line " +
                                 std::to_string(obj.lineCount));
          arr->ident = ident->meta;
          if (this->typeList[obj.meta] == nullptr)
            throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                                 " Unknown type " + obj.meta);
          arr->type = *this->typeList[obj.meta];
          arr->indices = indices;
          arr->mut = isMutable;
          arr->scope = scope;
          arr->logicalLine = obj.lineCount;
          output = arr;

          // Check for equal sign
          auto eq = dynamic_cast<lex::OpSym *>(tokens.peek());
          if (eq != nullptr && eq->Sym == '=') {
            tokens.pop();
            auto assign = new ast::DecAssignArr;
            assign->declare = arr;
            assign->expr = this->parseExpr(tokens);
            assign->mute = isMutable;
            assign->logicalLine = obj.lineCount;
            output = assign;
          }
        } else if (sym->Sym == '{') {
          tokens.pop();
          output = new ast::Destructure(tokens, *this);
        } else
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Expected a symbol or a declaration");
      } else {
        throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                             "Unparsable token found");
      }
    } else if (obj.meta == "return") {
      output = new ast::Return(tokens, *this);
    } else if (obj.meta == "push") {
      auto push = new ast::Push;
      push->expr = this->parseExpr(tokens);
      push->logicalLine = obj.lineCount;
      output = push;
    } else if (obj.meta == "pull") {
      auto pull = new ast::Pull;
      pull->expr = this->parseExpr(tokens);
      pull->logicalLine = obj.lineCount;
      output = pull;
      if (singleStmt) return output;
    } else if (obj.meta == "if") {
      output = new ast::If(tokens, *this);
    } else if (obj.meta == "while") {
      output = new ast::While(tokens, *this);
    } else if (obj.meta == "for") {
      output = new ast::For(tokens, *this);
    } else if (obj.meta == "struct") {
      output = new ast::Struct(tokens, *this);
    } else if (obj.meta == "class") {
      output = new ast::Class(tokens, *this, safeType, dynamicType);
    } else if (obj.meta == "enum") {
      output = new ast::Enum(tokens, *this);
    } else if (obj.meta == "import") {
      output = new ast::Import(tokens, *this);
    } else if (obj.meta == "delete") {
      output = new ast::Delete(tokens, *this);
    } else if (obj.meta == "continue") {
      output = new ast::Continue(tokens);
    } else if (obj.meta == "break") {
      output = new ast::Break(tokens);
    } else if (obj.meta == "else") {
      throw err::Exception(
          "Line: " + std::to_string(tokens.peek()->lineCount) +
          " floating else, may have an extra semicolon before the else");
    } else {
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
        auto sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
        links::LinkedList<std::string> modList;
        while (sym.Sym == '.') {
          if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
            auto mod = *dynamic_cast<lex::LObj *>(tokens.pop());
            modList << mod.meta;
          } else
            throw err::Exception("Expected, Ident after dot. on line " +
                                 std::to_string(sym.lineCount));
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
          } else
            throw err::Exception("expected assignment operator got on line " +
                                 std::to_string(sym.lineCount) + " " + sym.Sym);
        }

        links::LinkedList<ast::Expr *> indices;
        if (sym.Sym == '[') {
          indices << this->parseExpr(tokens);
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
              dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ']') {
            tokens.pop();
          } else
            throw err::Exception("Expected ']'");

          while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                 dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '[') {
            tokens.pop();
            indices << this->parseExpr(tokens);
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ']') {
              tokens.pop();
            } else {
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Expected ]");
            }
          }
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
          } else {
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected some symbol after array");
          }
        }

        if (sym.Sym == '=') {
          output = new ast::Assign(obj.meta, indices, modList, tokens, *this);
        } else if (sym.Sym == '(') {
          output =
              new ast::Call(obj.meta, this->parseCallArgsList(tokens), modList);
          output->logicalLine = obj.lineCount;
        } else if (sym.Sym == '+') {
          output = new ast::Inc(obj.meta, tokens);
        } else if (sym.Sym == '-') {
          output = new ast::Dec(obj.meta, tokens);
        } else
          throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                               " expected assignment operator after " +
                               obj.meta);
      } else
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " expected Assignment operator after " + obj.meta);
    }
  }

  if (singleStmt) {
    return output;
  }
  if (tokens.head == nullptr) {
    this->Output = *output;
    return output;
  } else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
             tokens.head->next != nullptr) {
    auto obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
    tokens.pop();
    if (obj.Sym == ';') {
      auto s = new ast::Sequence;
      s->Statement1 = output;
      s->Statement2 = this->parseStmt(tokens);
      this->Output = *s;
      return s;
    } else if (obj.Sym == '}') {
      this->Output = *output;
      return output;
    } else if (obj.Sym == '!') {
      this->Output = *output;
      return output;
    }
  } else {
    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
        tokens.head->next == nullptr) {
      auto obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
      tokens.pop();
      if (obj.Sym == '}') {
        this->Output = *output;
        return nullptr;
      } else if (obj.Sym == ';') {
        this->Output = *output;
        return output;
      }
    }
  }
  return output;
}

links::LinkedList<ast::Expr *> parse::Parser::parseCallArgsList(
    links::LinkedList<lex::Token *> &tokens) {
  links::LinkedList<ast::Expr *> args;
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
      dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ')') {
    tokens.pop();
  } else {
    bool pop = false;
    do {
      if (pop) tokens.pop();
      args.push(this->parseExpr(tokens));
      pop = true;
    } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
             dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
      auto sym = dynamic_cast<lex::OpSym *>(tokens.pop());
      if (sym->Sym != ')')
        throw err::Exception("Expected closed parenthesis got " + sym->Sym);
    }
  }
  return args;
}

ast::Statement *parse::Parser::parseArgs(
    links::LinkedList<lex::Token *> &tokens, char deliminator, char close,
    std::vector<ast::Type> &types, int &required) {
  auto output = new ast::Statement();
  auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym == nullptr) {
    // a question mark is also acceptable as a marjer for optional
    if (dynamic_cast<lex::Ref *>(tokens.peek()) == nullptr) {
      required++;
    } else {
      tokens.pop();
    }
  } else if (sym->Sym == '*')
    tokens.pop();

  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto obj = *dynamic_cast<lex::LObj *>(tokens.peek());
    bool isMutable;
    if (this->mutability == 0)
      isMutable = true;
    else
      isMutable = false;
    tokens.pop();
    if (obj.meta == "const") {
      isMutable = false;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statement after public on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "mutable") {
      if (mutability == 2)
        throw err::Exception(
            "Cannot use a mutable variable in safe "
            "mode, please set config "
            "mutability to promiscuous or strict");
      isMutable = true;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statement after public on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    };

    if (typeList[obj.meta] != nullptr) {
      auto dec = new ast::Declare();
      const auto sym = dynamic_cast<lex::Symbol *>(tokens.peek());
      if (sym != nullptr && sym->meta == "<") {
        dec->type = this->parseFPointerType(tokens, obj.meta);
      } else {
        dec->type = *typeList[obj.meta];
      }
      // ensures the the current token is an Ident
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        auto ident = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        dec->ident = ident.meta;
        dec->mut = isMutable;
        output = dec;
        types.push_back(dec->type);
      }
    } else
      throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                           " expected type got " + obj.meta);
  }

  if (tokens.head == nullptr) {
    throw err::Exception("unterminated function call");
  } else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
             tokens.head->next != nullptr) {
    auto obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
    tokens.pop();

    if (obj.Sym == deliminator) {
      auto s = new ast::Sequence;
      s->Statement1 = output;
      s->Statement2 =
          this->parseArgs(tokens, deliminator, close, types, required);
      return s;
    } else if (obj.Sym == close) {
      return output;
    }
  }
  return output;
}

ast::Type parse::Parser::parseFPointerType(
    links::LinkedList<lex::Token *> &tokens, const std::string typeName) {
  tokens.pop();
  auto callTypeList = std::vector<ast::Type>();
  std::string newTypeName = typeName + "~";
  int requiredCount = 0;
  while (true) {
    const auto closeSym = dynamic_cast<lex::Symbol *>(tokens.peek());
    if (closeSym != nullptr && closeSym->meta == ">") {
      tokens.pop();
      break;
    }
    if (dynamic_cast<lex::Ref *>(tokens.peek()) == nullptr)
      requiredCount++;
    else
      tokens.pop();
    const auto type = dynamic_cast<lex::LObj *>(tokens.pop());
    if (type == nullptr)
      throw err::Exception("Type expected on line " +
                           std::to_string(tokens.peek()->lineCount));
    newTypeName += type->meta;
    callTypeList.push_back(*typeList[type->meta]);

    const auto comma = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (comma != nullptr && comma->Sym == ',') {
      tokens.pop();
      newTypeName += ",";
    } else {
      const auto closeSym = dynamic_cast<lex::Symbol *>(tokens.peek());
      if (closeSym != nullptr && closeSym->meta == ">") {
        tokens.pop();
        break;
      }
      throw err::Exception("Expected , or > on line " +
                           std::to_string(tokens.peek()->lineCount));
    }
  }

  auto type = ast::Type(newTypeName + "~", asmc::QWord);
  type.fPointerArgs.returnType = typeList[typeName];
  type.fPointerArgs.argTypes = callTypeList;
  type.fPointerArgs.isFPointer = true;
  type.fPointerArgs.requiredArgs = requiredCount;

  return type;
}

ast::ConditionalExpr *parse::Parser::parseCondition(
    links::LinkedList<lex::Token *> &tokens) {
  auto output = new ast::ConditionalExpr();

  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
    if (sym.Sym != '(')
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           "\'" + sym.Sym + "\"" +
                           "unOpened Condition.  Please open with: (");
  } else
    throw err::Exception("Line: '" +
                         dynamic_cast<lex::LObj *>(tokens.peek())->meta + "'" +
                         " unOpened Condition.  Please open with: '('");

  output->expr1 = this->parseExpr(tokens);

  if (dynamic_cast<lex::Symbol *>(tokens.peek()) != nullptr) {
    auto sym = *dynamic_cast<lex::Symbol *>(tokens.pop());
    if (sym.meta == "==") {
      output->op = ast::Equ;
    } else if (sym.meta == "!=") {
      output->op = ast::NotEqu;
    } else if (sym.meta == "<") {
      output->op = ast::Less;
    } else if (sym.meta == ">") {
      output->op = ast::Great;
    }
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Condition with now conditional Operator");

  output->expr2 = this->parseExpr(tokens);

  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
    if (sym.Sym != ')')
      throw err::Exception(
          "Line: " + std::to_string(tokens.peek()->lineCount) +
          " unTerminated Condition.  Please terminate with: )");
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " unTerminated Condition.  Please terminate with: )");

  return output;
}

ast::Expr *parse::Parser::parseExpr(links::LinkedList<lex::Token *> &tokens) {
  auto output = new ast::Expr();
  if (dynamic_cast<lex::StringObj *>(tokens.peek()) != nullptr) {
    auto stringObj = *dynamic_cast<lex::StringObj *>(tokens.peek());
    tokens.pop();
    auto *slit = new ast::StringLiteral();
    slit->logicalLine = stringObj.lineCount;
    slit->val = stringObj.value;
    output = slit;
  } else if (dynamic_cast<lex::FStringObj *>(tokens.peek())) {
    lex::Lexer lexer = lex::Lexer();
    auto fstringObj = *dynamic_cast<lex::FStringObj *>(tokens.peek());
    tokens.pop();
    auto *fstringLiteral = new ast::FStringLiteral();
    fstringLiteral->logicalLine = fstringObj.lineCount;
    fstringLiteral->val = fstringObj.value;

    // find each { and } and parse the expression in between
    while (true) {
      std::string::size_type pos = fstringObj.value.find('{', 0);
      if (pos == std::string::npos) {
        break;
      }
      std::string::size_type pos2 = fstringObj.value.find('}', 0);
      if (pos2 == std::string::npos)
        throw err::Exception(
            "Line: " + std::to_string(fstringObj.lineCount) +
            " unTerminated Fstring.  Please terminate with: }");
      std::string expr = fstringObj.value.substr(pos + 1, pos2 - pos - 1);

      // replace with %%% to avoid parsing errors
      fstringObj.value.replace(pos, pos2 - pos + 1, "%%%");

      auto tokes = lexer.Scan(expr);
      tokes.invert();
      auto exprAst = this->parseExpr(tokes);
      fstringLiteral->args.push_back(exprAst);
    }

    fstringLiteral->val = fstringObj.value;
    output = fstringLiteral;
  } else if (dynamic_cast<lex::INT *>(tokens.peek()) != nullptr) {
    auto intObj = *dynamic_cast<lex::INT *>(tokens.pop());
    auto intLiteral = new ast::IntLiteral();
    intLiteral->logicalLine = intObj.lineCount;
    // check if the int is a hex
    if (intObj.value[0] == '0' && intObj.value[1] == 'x') {
      intLiteral->val = std::stoi(intObj.value, nullptr, 16);
    } else {
      intLiteral->val = std::stoi(intObj.value);
    }

    output = intLiteral;
  } else if (dynamic_cast<lex::FloatLit *>(tokens.peek()) != nullptr) {
    auto floatObj = *dynamic_cast<lex::FloatLit *>(tokens.pop());
    auto flit = new ast::FloatLiteral();
    flit->logicalLine = floatObj.lineCount;
    flit->val = floatObj.value;
    output = flit;
  } else if (dynamic_cast<lex::Long *>(tokens.peek()) != nullptr) {
    auto intObj = *dynamic_cast<lex::Long *>(tokens.pop());
    auto longLiteral = new ast::LongLiteral();

    if (intObj.value[0] == '0' && intObj.value[1] == 'x') {
      longLiteral->val = std::stoi(intObj.value, nullptr, 16);
    } else {
      longLiteral->val = std::stoi(intObj.value);
    }

    output = longLiteral;
  } else if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto obj = *dynamic_cast<lex::LObj *>(tokens.pop());
    links::LinkedList<std::string> modList;
    links::LinkedList<ast::Expr *> indices;

    if (tokens.count > 0)
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
        lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
        while (sym.Sym == '.') {
          tokens.pop();
          if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
            lex::LObj mod = *dynamic_cast<lex::LObj *>(tokens.pop());
            modList << mod.meta;
          } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected, Ident after dot.");
          if (tokens.count > 0 &&
              dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
          } else
            break;
        }

        if (sym.Sym == '[') {
          tokens.pop();
          indices << this->parseExpr(tokens);
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
              dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ']') {
            tokens.pop();
          } else
            throw err::Exception("Expected ']'");
          while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                 dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '[') {
            tokens.pop();
            indices << this->parseExpr(tokens);
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ']') {
              tokens.pop();
            } else {
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Expected ]");
            }
          }
        }
      }

    auto *var = new ast::Var();
    var->Ident = obj.meta;
    var->modList = modList;
    var->indices = indices;
    var->logicalLine = obj.lineCount;
    output = var;
    if (obj.meta == "new") {
      auto newExpr = new ast::NewExpr();
      newExpr->logicalLine = obj.lineCount;
      auto typeName = dynamic_cast<lex::LObj *>(tokens.pop());
      if (typeName == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Expected, Ident after new.");
      auto nType = this->typeList[typeName->meta];
      if (nType == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) + ": " +
            typeName->meta + " is not a valid type.");
      newExpr->type = *nType;
      newExpr->logicalLine = obj.lineCount;
      auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym != nullptr && sym->Sym == '(') {
        tokens.pop();
        auto testSym = dynamic_cast<lex::OpSym *>(tokens.peek());
        newExpr->args = links::LinkedList<ast::Expr *>();
        if (testSym != nullptr && testSym->Sym != '[') {
          auto symp = dynamic_cast<lex::OpSym *>(tokens.pop());
          if (symp->Sym != ')')
            throw err::Exception("Expected closed parenthesis got " +
                                 symp->Sym);
        } else {
          bool pop = false;
          do {
            if (pop) tokens.pop();
            newExpr->args.push(this->parseExpr(tokens));
            pop = true;
          } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                   dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');

          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            auto symp = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (symp->Sym != ')')
              throw err::Exception("Expected closed parenthesis got " +
                                   symp->Sym);
          }
        }
      }
      output = newExpr;
    } else if (obj.meta == "if") {
      auto ifExpr = new ast::IfExpr();
      ifExpr->logicalLine = obj.lineCount;
      ifExpr->expr = this->parseExpr(tokens);
      ifExpr->trueExpr = this->parseExpr(tokens);
      // check for else
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr &&
          dynamic_cast<lex::LObj *>(tokens.peek())->meta == "else") {
        tokens.pop();
        ifExpr->falseExpr = this->parseExpr(tokens);
      } else {
        ifExpr->falseExpr = nullptr;
      }
      output = ifExpr;
    } else if (tokens.count > 0 &&
               dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
      auto asObject = *dynamic_cast<lex::LObj *>(tokens.peek());
      if (asObject.meta == "as") {
        auto deRef = new ast::DeReference();
        deRef->logicalLine = asObject.lineCount;
        tokens.pop();
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
          auto view = *dynamic_cast<lex::LObj *>(tokens.pop());
          deRef->Ident = obj.meta;
          deRef->modList = modList;
          if (this->typeList[view.meta] == nullptr)
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Unknown Type " + view.meta);
          deRef->type = *this->typeList[view.meta];
          output = deRef;
        } else
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " No dereference type given with as");
      }
    } else if (tokens.count > 0 &&
               dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
      auto sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym.Sym == '(') {
        tokens.pop();
        auto call = new ast::Call();
        call->ident = obj.meta;
        call->modList = modList;
        call->logicalLine = obj.lineCount;

        auto testSym = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (testSym != nullptr && testSym->Sym != '[') {
          auto symp = dynamic_cast<lex::OpSym *>(tokens.pop());
          if (symp->Sym != ')')
            throw err::Exception("Expected closed parenthesis got " +
                                 symp->Sym);
        } else {
          bool pop = false;
          do {
            if (pop) tokens.pop();
            call->Args.push(this->parseExpr(tokens));
            pop = true;
          } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
                   dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');

          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            auto symp = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (symp->Sym != ')')
              throw err::Exception("Expected closed parenthesis got " +
                                   symp->Sym);
          }
        }

        ast::CallExpr *callExpr = new ast::CallExpr;
        callExpr->call = call;
        callExpr->logicalLine = obj.lineCount;

        output = callExpr;
      } else {
        var->Ident = obj.meta;
        var->indices = indices;
        var->modList = modList;
        var->logicalLine = obj.lineCount;
        output = var;
      }
    } else {
      var->Ident = obj.meta;
      var->modList = modList;
      var->logicalLine = obj.lineCount;
      output = var;
    }
  } else if (dynamic_cast<lex::CharObj *>(tokens.peek()) != nullptr) {
    auto obj = *dynamic_cast<lex::CharObj *>(tokens.pop());
    auto charLiteral = new ast::CharLiteral();
    charLiteral->value = obj.value;
    charLiteral->logicalLine = obj.lineCount;
    output = charLiteral;
  } else if (dynamic_cast<lex::Ref *>(tokens.peek()) != nullptr) {
    tokens.pop();
    auto ref = new ast::Reference();
    if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
      auto identObject = *dynamic_cast<lex::LObj *>(tokens.pop());
      ref->Ident = identObject.meta;
      ref->logicalLine = identObject.lineCount;

      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
        auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        while (sym->Sym == '.') {
          tokens.pop();
          if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
            auto modifierObject = *dynamic_cast<lex::LObj *>(tokens.pop());
            ref->modList.push(modifierObject.meta);
          } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected identifier after dot");
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            sym = dynamic_cast<lex::OpSym *>(tokens.peek());
          } else
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Expected identifier after dot");
        }
      }

      output = ref;
    } else
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " No object given to reference");
  } else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto eq = *dynamic_cast<lex::OpSym *>(tokens.peek());
    if (eq.Sym == '[') {
      tokens.pop();
      ast::Lambda *lambda = new ast::Lambda();
      lambda->function = new ast::Function();
      lambda->function->req = 0;
      lambda->function->args = this->parseArgs(
          tokens, ',', ']', lambda->function->argTypes, lambda->function->req);

      lambda->logicalLine = eq.lineCount;
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Need an => to start lambda not a symbol");
      if ((dynamic_cast<lex::OpSym *>(tokens.pop())->Sym != '='))
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Need an => to start lambda");

      if (dynamic_cast<lex::Symbol *>(tokens.peek()) == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Need an => to start lambda not a symbol");
      if ((dynamic_cast<lex::Symbol *>(tokens.pop())->meta != ">"))
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " GOT: " + dynamic_cast<lex::OpSym *>(tokens.pop())->Sym +
            " Need an => to start lambda");

      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
          dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '{') {
        tokens.pop();
        lambda->function->statement = this->parseStmt(tokens);
      } else
        lambda->function->statement = this->parseStmt(tokens, true);

      ast::Type Adr = ast::Type();
      Adr.typeName = "any";
      Adr.opType = asmc::Hard;
      Adr.size = asmc::QWord;
      lambda->function->type = Adr;

      output = lambda;
    } else if (eq.Sym == '(') {
      tokens.pop();
      auto paren = new ast::ParenExpr();
      paren->expr = this->parseExpr(tokens);
      paren->logicalLine = eq.lineCount;
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Need an ) to end parenthesis not a symbol");
      if ((dynamic_cast<lex::OpSym *>(tokens.pop())->Sym != ')'))
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " GOT: " + dynamic_cast<lex::OpSym *>(tokens.pop())->Sym +
            " Need an ) to end parenthesis");
      output = paren;
    } else if (eq.Sym == '!') {
      tokens.pop();
      auto notExpr = new ast::Not();
      notExpr->expr = this->parseExpr(tokens);
      notExpr->logicalLine = eq.lineCount;
      output = notExpr;
    } else if (eq.Sym == '{') {
      auto save_token = tokens.pop();
      auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym != nullptr && sym->Sym == '}') {
        tokens.pop();
        auto newExpr = new ast::NewExpr();
        newExpr->logicalLine = eq.lineCount;
        newExpr->type.typeName = "Map";
        output = newExpr;
      } else {
        // std::cout << "Creating Map or Struct List" << std::endl;
        // tokens.push(save_token);
        auto first_arg = this->parseExpr(tokens);
        sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (sym != nullptr && (sym->Sym == ',' || sym->Sym == '}')) {
          auto structList = new ast::StructList();
          structList->args.push(first_arg);
          lex::OpSym *symp;

          if (sym->Sym == ',') {
            do {
              tokens.pop();
              structList->args.push(this->parseExpr(tokens));
              symp = dynamic_cast<lex::OpSym *>(tokens.peek());
            } while (symp != nullptr && symp->Sym == ',');

            if (symp == nullptr || symp->Sym != '}')
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Need an } to end struct not a symbol");
          }

          tokens.pop();
          structList->logicalLine = eq.lineCount;
          output = structList;
        } else {
          if (sym->Sym != ':')
            throw err::Exception("Line: " + std::to_string(sym->lineCount) +
                                 " Need an : to start map not " + sym->Sym);

          auto newExpr = new ast::NewExpr();
          newExpr->logicalLine = eq.lineCount;
          newExpr->type.typeName = "Map";

          ast::CallExpr *lastSet = new ast::CallExpr();
          lastSet->logicalLine = eq.lineCount;
          lastSet->call = new ast::Call();
          lastSet->call->ident = "set";
          lastSet->call->modList = links::LinkedList<std::string>();
          lastSet->call->Args = links::LinkedList<ast::Expr *>();
          lastSet->call->Args.push(first_arg);
          tokens.pop();
          lastSet->call->Args.push(this->parseExpr(tokens));
          sym = dynamic_cast<lex::OpSym *>(tokens.peek());
          newExpr->extention = lastSet;

          while (sym != nullptr && sym->Sym == ',') {
            tokens.pop();
            auto nextSet = new ast::CallExpr();
            nextSet->logicalLine = eq.lineCount;
            nextSet->call = new ast::Call();
            nextSet->call->ident = "set";
            nextSet->call->modList = links::LinkedList<std::string>();
            nextSet->call->Args = links::LinkedList<ast::Expr *>();
            nextSet->call->Args.push(this->parseExpr(tokens));
            sym = dynamic_cast<lex::OpSym *>(tokens.peek());
            if (sym == nullptr || sym->Sym != ':')
              throw err::Exception(
                  "Line: " + std::to_string(tokens.peek()->lineCount) +
                  " Map key not followed by :");
            tokens.pop();
            nextSet->call->Args.push(this->parseExpr(tokens));
            lastSet->extention = nextSet;
            lastSet = nextSet;
            sym = dynamic_cast<lex::OpSym *>(tokens.peek());
          }

          if (sym == nullptr || sym->Sym != '}')
            throw err::Exception(
                "Line: " + std::to_string(tokens.peek()->lineCount) +
                " Need an } to end map not a symbol");
          tokens.pop();
          output = newExpr;
        }
      }
    }
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Unknown Expr");

  if (tokens.count > 0)
    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
      auto sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
      auto compound = new ast::Compound();
      compound->logicalLine = sym.lineCount;
      if (sym.Sym == '+') {
        tokens.pop();
        compound->op = ast::Plus;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.Sym == '-') {
        tokens.pop();
        compound->op = ast::Minus;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.Sym == '*') {
        tokens.pop();
        compound->op = ast::Mul;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.Sym == '^') {
        tokens.pop();
        compound->op = ast::Carrot;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.Sym == '/') {
        tokens.pop();
        compound->op = ast::Div;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.Sym == '%') {
        tokens.pop();
        compound->op = ast::Mod;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.Sym == '&') {
        tokens.pop();
        compound->op = ast::AndBit;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.Sym == '>') {
        tokens.pop();
        compound->op = ast::Great;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.Sym == '<') {
        tokens.pop();
        compound->op = ast::Less;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.Sym == '|') {
        tokens.pop();
        compound->op = ast::OrBit;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      }
    } else if (dynamic_cast<lex::Symbol *>(tokens.peek()) != nullptr) {
      auto compound = new ast::Compound();
      auto sym = *dynamic_cast<lex::Symbol *>(tokens.peek());
      compound->logicalLine = sym.lineCount;
      if (sym.meta == "==") {
        tokens.pop();
        compound->op = ast::CompEqu;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.meta == "!=") {
        tokens.pop();
        compound->op = ast::NotEqu;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.meta == ">") {
        tokens.pop();
        compound->op = ast::GreatCmp;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.meta == "<") {
        tokens.pop();
        compound->op = ast::LessCmp;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.meta == ">=") {
        tokens.pop();
        compound->op = ast::Geq;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      } else if (sym.meta == "<=") {
        tokens.pop();
        compound->op = ast::Leq;
        compound->expr1 = output;
        compound->expr2 = this->parseExpr(tokens);
        return prioritizeExpr(compound);
      }
    }

  if (tokens.count > 0)
    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
      auto sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym.Sym == '$') {
        tokens.pop();
        auto typeName = dynamic_cast<lex::LObj *>(tokens.pop());
        if (typeName == nullptr)
          throw err::Exception("Line: " + std::to_string(sym.lineCount) +
                               " Expected typename after in cast");
        output->typeCast = typeName->meta;
      }
    }
  // check if the next token is a dot go to extend the expression
  if (tokens.count > 0)
    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
      auto sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym.Sym == '.') {
        tokens.pop();
        auto next = this->parseExpr(tokens);
        output->extention = next;
      }
    }

  return output;
}

ast::Expr *prioritizeExpr(ast::Expr *expr) {
  auto compound = dynamic_cast<ast::Compound *>(expr);

  ast::Compound *root = compound;

  if (compound != nullptr) {
    ast::Compound *right = dynamic_cast<ast::Compound *>(compound->expr2);
    int i = 0;
    while (right != nullptr) {
      if (getOpPriority(compound->op) >= getOpPriority(right->op)) {
        compound->expr2 = right->expr1;
        right->expr1 = compound;
        if (i == 0) root = right;
        compound = right;
        right = dynamic_cast<ast::Compound *>(compound->expr2);
      } else
        break;
      i++;
    }
  }
  return root;
};