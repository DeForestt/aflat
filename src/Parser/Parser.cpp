#include "Parser/Parser.hpp"

#include <algorithm>
#include <iostream>
#include <unordered_set>

#include "Exceptions.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements.hpp"
#include "Parser/AST/Statements/Match.hpp"
#include "Scanner.hpp"

ast::Expr *prioritizeExpr(ast::Expr *expr);

parse::Parser::Parser(int mutability) {
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
  this->addType("Self", asmc::Hard, asmc::QWord);
  // create a dummy type for let
  this->addType("let", asmc::Hard, asmc::QWord, true);
  // create a dummy type for typeOf
  this->addType("typeOf", asmc::Hard, asmc::QWord, true);
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
    links::LinkedList<lex::Token *> &tokens, bool singleStmt) {
  ast::Statement *output = new ast::Statement;
  std::vector<parse::Annotation> annotations;

  if (tokens.head == nullptr) {
    return output;
  };

  // annotations
  // @Ident(Argument, Argument)
  auto atSym = dynamic_cast<lex::OpSym *>(tokens.peek());
  while (atSym != nullptr && atSym->Sym == '@') {
    tokens.pop();
    auto annotation = parse::Annotation();
    auto ident = dynamic_cast<lex::LObj *>(tokens.pop());
    if (ident == nullptr)
      throw err::Exception("Expected identifier after @ on line " +
                           std::to_string(atSym->lineCount));
    annotation.name = ident->meta;
    auto openParen = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (!openParen || openParen->Sym != '(') {
      atSym = openParen;
      annotations.push_back(annotation);
      continue;
    }
    tokens.pop();
    auto arg = dynamic_cast<lex::LObj *>(tokens.peek());
    while (arg != nullptr) {
      annotation.args.push_back(arg->meta);
      tokens.pop();
      auto comma = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (comma && comma->Sym == ',') {
        tokens.pop();
        arg = dynamic_cast<lex::LObj *>(tokens.peek());
      } else
        break;
    }
    if (openParen) {
      auto closeParen = dynamic_cast<lex::OpSym *>(tokens.pop());
      if (!closeParen || closeParen->Sym != ')') {
        if (closeParen) {
          throw err::Exception("Expected ')' after annotation on line " +
                               std::to_string(atSym->lineCount) + " got " +
                               closeParen->Sym);
        } else
          throw err::Exception("Expected ')' after annotation on line " +
                               std::to_string(atSym->lineCount));
      }
    }
    annotations.push_back(annotation);
    atSym = dynamic_cast<lex::OpSym *>(tokens.peek());
  }

  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto obj = *dynamic_cast<lex::LObj *>(tokens.peek());
    auto safeType = false;
    auto dynamicType = false;
    auto pedantic = false;
    auto scope = ast::Public;
    std::vector<std::string> typeNames;
    tokens.pop();

    bool isMutable = true;
    bool isImmutable = false;
    if (this->mutability == 0)
      isMutable = true;
    else
      isMutable = false;

    // Use a set for efficient lookup instead of multiple 'or' checks
    static const std::unordered_set<std::string> modifiers = {
        "safe", "dynamic", "pedantic", "types"};

    if (modifiers.count(obj.meta)) {
      while (modifiers.count(obj.meta)) {
        if (obj.meta == "safe")
          safeType = true;
        else if (obj.meta == "dynamic")
          dynamicType = true;
        else if (obj.meta == "pedantic")
          pedantic = true;
        else if (obj.meta == "types") {
          // types(type1, type2, ...)
          auto OpenParen = dynamic_cast<lex::OpSym *>(tokens.pop());
          if (!OpenParen || OpenParen->Sym != '(') {
            throw err::Exception("Expected '(' after types on line " +
                                 std::to_string(obj.lineCount));
          }
          auto typeIdent = dynamic_cast<lex::LObj *>(tokens.pop());
          if (!typeIdent)
            throw err::Exception("Expected identifier after types on line " +
                                 std::to_string(obj.lineCount));
          typeNames.push_back(typeIdent->meta);
          this->addType(typeIdent->meta, asmc::Hard, asmc::QWord, true);
          auto comma = dynamic_cast<lex::OpSym *>(tokens.peek());
          while (comma && comma->Sym == ',') {
            tokens.pop();
            typeIdent = dynamic_cast<lex::LObj *>(tokens.pop());
            if (!typeIdent)
              throw err::Exception("Expected identifier after types on line " +
                                   std::to_string(obj.lineCount));
            typeNames.push_back(typeIdent->meta);
            comma = dynamic_cast<lex::OpSym *>(tokens.peek());
          }
          if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr ||
              dynamic_cast<lex::OpSym *>(tokens.pop())->Sym != ')') {
            throw err::Exception("Expected ')' after types on line " +
                                 std::to_string(obj.lineCount));
          }
        }

        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
          obj = *dynamic_cast<lex::LObj *>(tokens.peek());
          tokens.pop();
        } else {
          throw err::Exception("Expected type after safe/dynamic on line " +
                               std::to_string(obj.lineCount));
        }
      }

      // check if object.meta is class or function
      static const std::unordered_set<std::string> classFunction = {"class",
                                                                    "fn"};

      if (classFunction.count(obj.meta) == 0 &&
          (dynamicType || pedantic || safeType)) {
        throw err::Exception(
            "safe/dynamic/pedantic can only be used with classes or functions "
            "on line " +
            std::to_string(obj.lineCount) + " got " + obj.meta);
      };
    }

    static const std::unordered_set<std::string> accessModifiers = {
        "const",   "mutable", "immutable", "public",
        "private", "static",  "export"};

    if (accessModifiers.count(obj.meta)) {
      while (accessModifiers.count(obj.meta)) {
        if (obj.meta == "const") {
          isMutable = false;
        } else if (obj.meta == "mutable") {
          isMutable = true;
        } else if (obj.meta == "immutable") {
          isMutable = false;
          isImmutable = true;
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
    } else if (obj.meta == "immutable") {
      isMutable = false;
      isImmutable = true;
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
      auto type = (sym != nullptr && sym->meta == "<")
                      ? this->parseFPointerType(tokens, obj.meta)
                      : *this->typeList[obj.meta];

      auto templateTypes = this->parseTemplateTypeList(tokens, obj.lineCount);
      if (!templateTypes.empty()) {
        for (auto &tName : templateTypes) type.typeName += "." + tName;
      }

      const auto ref = dynamic_cast<lex::Ref *>(tokens.peek());
      bool optional = false;
      if (ref) {
        tokens.pop();
        optional = true;
      }

      std::string requestType = "";
      links::LinkedList<std::string> modList;
      // Handle typeOf
      if (obj.meta == "typeOf") {
        auto OpenParen = dynamic_cast<lex::OpSym *>(tokens.pop());
        if (!OpenParen || OpenParen->Sym != '(') {
          throw err::Exception("Expected '(' after typeOf on line " +
                               std::to_string(obj.lineCount));
        }
        auto ident = dynamic_cast<lex::LObj *>(tokens.pop());
        if (!ident)
          throw err::Exception("Expected identifier after typeOf on line " +
                               std::to_string(obj.lineCount));
        requestType = ident->meta;

        auto dot = dynamic_cast<lex::OpSym *>(tokens.peek());
        while (dot && dot->Sym == '.') {
          tokens.pop();
          auto mod = dynamic_cast<lex::LObj *>(tokens.pop());
          if (!mod)
            throw err::Exception("Expected identifier after typeOf on line " +
                                 std::to_string(obj.lineCount));
          modList << mod->meta;
          dot = dynamic_cast<lex::OpSym *>(tokens.peek());
        }

        auto CloseParen = dynamic_cast<lex::OpSym *>(tokens.pop());
        if (!CloseParen || CloseParen->Sym != ')') {
          throw err::Exception("Expected ')' after typeOf on line " +
                               std::to_string(obj.lineCount));
        }
      }

      const auto refSym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (refSym && refSym->Sym == '&') {
        type.isReference = true;
        type.refSize = type.size;
        type.size = asmc::QWord;
        tokens.pop();
      }
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
            output =
                new ast::Function(ident.meta, scope, type, overload, scopeName,
                                  tokens, *this, optional, safeType);
          } else if (sym.Sym == '=') {
            tokens.pop();
            auto decl = new ast::Declare(ident.meta, scope, obj.meta, isMutable,
                                         type, requestType, modList);
            decl->readOnly = isImmutable;
            output =
                new ast::DecAssign(decl, isMutable, tokens, *this, annotations);
          }
        } else {
          auto decl = new ast::Declare(ident.meta, scope, obj.meta, isMutable,
                                       type, requestType, modList);
          decl->readOnly = isImmutable;
          output = decl;
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
          output = new ast::Destructure(isMutable, tokens, *this);
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
    } else if (obj.meta == "resolve") {
      auto ret = new ast::Return(tokens, *this);
      ret->resolver = true;
      output = ret;
    } else if (obj.meta == "fn") {
      output = new ast::Function(scope, tokens, typeNames, *this, safeType);
    } else if (obj.meta == "match") {
      output = new ast::Match(tokens, *this);
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
    } else if (obj.meta == "foreach") {
      output = new ast::ForEach(tokens, *this);
    } else if (obj.meta == "struct") {
      output = new ast::Struct(tokens, *this);
    } else if (obj.meta == "class") {
      output = new ast::Class(tokens, *this, safeType, dynamicType, pedantic,
                              annotations, typeNames);
    } else if (obj.meta == "union") {
      output = new ast::Union(tokens, *this, typeNames);
    } else if (obj.meta == "enum") {
      output = new ast::Enum(tokens, *this);
    } else if (obj.meta == "import") {
      output = new ast::Import(tokens, *this);
    } else if (obj.meta == "transform") {
      output = new ast::Transform(tokens);
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
      auto genericTypes = this->parseTemplateTypeList(tokens, obj.lineCount);

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
          output = new ast::Call(obj.meta, this->parseCallArgsList(tokens),
                                 modList, genericTypes);
          output->logicalLine = obj.lineCount;
        } else if (sym.Sym == '+') {
          output = new ast::Inc(obj.meta, tokens);
        } else if (sym.Sym == '-') {
          output = new ast::Dec(obj.meta, tokens);
        } else {
          // lets write everythig back to the tokens...
          auto s = new lex::OpSym();
          s->Sym = sym.Sym;
          s->lineCount = sym.lineCount;
          tokens.push(s);
          for (int i = 0; i < modList.size(); i++) {
            auto s = new lex::LObj();
            s->meta = modList.pop();
            s->lineCount = sym.lineCount;
            tokens.push(s);
          }

          auto name = new lex::LObj();
          name->meta = obj.meta;
          name->lineCount = obj.lineCount;
          tokens.push(name);

          auto ret = new ast::Return();
          ret->implicit = true;
          ret->logicalLine = obj.lineCount;
          ret->expr = this->parseExpr(tokens);
          output = ret;
        }
      } else {
        // also an implicit return
        auto var = new ast::Var;
        var->Ident = obj.meta;
        var->logicalLine = obj.lineCount;
        auto ret = new ast::Return;
        ret->logicalLine = obj.lineCount;
        ret->expr = var;
        output = ret;
      }
    }
  } else if (tokens.peek() && tokens.count > 0) {
    auto close_curl = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (!close_curl || (close_curl->Sym != '}' && close_curl->Sym != ';')) {
      auto ret = new ast::Return(tokens, *this);
      ret->implicit = true;
      output = ret;
      // if the next token is not a semicolon, add one
      if (tokens.peek()) {
        auto opSym = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (!opSym || opSym->Sym != ';') {
          auto semicolon = new lex::OpSym();
          semicolon->Sym = ';';
          semicolon->lineCount = tokens.peek()->lineCount;
          tokens.push(semicolon);
        }
      }
    }
  }

  if (singleStmt) {
    auto call = dynamic_cast<ast::Call *>(output);

    if (!call) return output;

    if (tokens.peek()) {
      // implicit return exceptions
      // 1. if the next token is a semicolon, it is internal and should not be
      // an implicit return
      // 2. if the next token is an else, it is internal and should not be an
      // implicit return
      auto OpSym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (OpSym && OpSym->Sym == ';') return output;
      auto els = dynamic_cast<lex::LObj *>(tokens.peek());
      if (els && els->meta == "else") return output;
    }
    auto ret = new ast::Return;
    auto callExpr = new ast::CallExpr;
    callExpr->call = call;
    ret->expr = callExpr;
    this->Output = *ret;
    return ret;
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
      s->Statement2 = this->parseStmt(tokens, false);
      this->Output = *s;
      return s;
    } else if (obj.Sym == '}') {
      this->Output = *output;
      // if the output is a call, we need to return it
      // note that this will only work if you omit the semicolon
      if (dynamic_cast<ast::Call *>(output) != nullptr) {
        auto ret = new ast::Return;
        auto callExpr = new ast::CallExpr;
        callExpr->call = dynamic_cast<ast::Call *>(output);
        ret->expr = callExpr;
        return ret;
      }
      // check if the next token is a semicolon if not, treat it like a swquence
      if (tokens.peek()) {
        auto opSym = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (!opSym || (opSym->Sym != ';' && opSym->Sym != ')' &&
                       opSym->Sym != ',' && opSym->Sym != '}')) {
          // check for else
          auto els = dynamic_cast<lex::LObj *>(tokens.peek());
          if (!els || els->meta != "else") {
            auto semicolon = new lex::OpSym();
            semicolon->Sym = ';';
            semicolon->lineCount = tokens.peek()->lineCount;
            tokens.push(semicolon);
          }
        }
      }
      return output;
    } else if (obj.Sym == '!') {
      this->Output = *output;
      return output;
    }
  } else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
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
        throw err::Exception(&"Expected closed parenthesis got "[sym->Sym]);
    }
  }
  return args;
}

ast::Statement *parse::Parser::parseArgs(
    links::LinkedList<lex::Token *> &tokens, char deliminator, char close,
    std::vector<ast::Type> &types, int &required, std::vector<bool> &mutability,
    std::vector<int> &optConvertionIndices, std::vector<bool> &readOnly) {
  return this->parseArgs(tokens, deliminator, close, types, required,
                         mutability, optConvertionIndices, readOnly, false);
}

ast::Statement *parse::Parser::parseArgs(
    links::LinkedList<lex::Token *> &tokens, char deliminator, char close,
    std::vector<ast::Type> &types, int &required, std::vector<bool> &mutability,
    std::vector<int> &optConvertionIndices, std::vector<bool> &readOnly,
    bool foreach) {
  auto output = new ast::Statement();
  auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym == nullptr) {
    // a question mark denotes an optional argument that is implicitly converted
    // from `NULL` or `VALUE` to `Option<VALUE>`
    if (dynamic_cast<lex::Ref *>(tokens.peek()) == nullptr) {
      required++;
    } else {
      tokens.pop();
      optConvertionIndices.push_back(types.size());
    }
  } else if (sym->Sym == '*')
    tokens.pop();

  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto obj = *dynamic_cast<lex::LObj *>(tokens.peek());
    bool isMutable;
    bool isImmutable = false;
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
    } else if (obj.meta == "immutable") {
      isMutable = false;
      isImmutable = true;
      if (dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr)
        throw err::Exception("Expected statement after public on line " +
                             std::to_string(obj.lineCount));
      obj = *dynamic_cast<lex::LObj *>(tokens.peek());
      tokens.pop();
    } else if (obj.meta == "mutable") {
      if (this->mutability == 2)
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

      auto templateTypes = this->parseTemplateTypeList(tokens, obj.lineCount);
      if (!templateTypes.empty()) {
        for (auto &tName : templateTypes) dec->type.typeName += "." + tName;
      }

      std::string requestType = "";
      links::LinkedList<std::string> modList;
      // Handle typeOf
      if (obj.meta == "typeOf") {
        auto OpenParen = dynamic_cast<lex::OpSym *>(tokens.pop());
        if (!OpenParen || OpenParen->Sym != '(') {
          throw err::Exception("Expected '(' after typeOf on line " +
                               std::to_string(obj.lineCount));
        }
        auto ident = dynamic_cast<lex::LObj *>(tokens.pop());
        if (!ident)
          throw err::Exception("Expected identifier after typeOf on line " +
                               std::to_string(obj.lineCount));
        requestType = ident->meta;

        auto dot = dynamic_cast<lex::OpSym *>(tokens.peek());
        while (dot && dot->Sym == '.') {
          tokens.pop();
          auto mod = dynamic_cast<lex::LObj *>(tokens.pop());
          if (!mod)
            throw err::Exception("Expected identifier after typeOf on line " +
                                 std::to_string(obj.lineCount));
          modList << mod->meta;
          dot = dynamic_cast<lex::OpSym *>(tokens.peek());
        }

        auto CloseParen = dynamic_cast<lex::OpSym *>(tokens.pop());
        if (!CloseParen || CloseParen->Sym != ')') {
          throw err::Exception("Expected ')' after typeOf on line " +
                               std::to_string(obj.lineCount));
        }
      }

      // check if the type is a reference
      const auto refSym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (refSym && refSym->Sym == '&') {
        // check if the type is an rval reference
        tokens.pop();
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
            dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '&') {
          tokens.pop();
          dec->type.isRvalue = true;
        } else {
          dec->type.isReference = true;
          dec->type.refSize = dec->type.size;
          dec->type.size = asmc::QWord;
        }
      }

      // ensures the the current token is an Ident
      if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
        auto ident = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        dec->ident = ident.meta;
        dec->mut = isMutable;
        dec->readOnly = isImmutable;
        dec->requestType = requestType;
        dec->modList = modList;
        output = dec;
        types.push_back(dec->type);
        mutability.push_back(isMutable);
        readOnly.push_back(isImmutable);
      }
    } else
      throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                           " expected type got " + obj.meta);
  }

  if (tokens.head == nullptr) {
    std::cout << "no tokens in call too " << sym->Sym << std::endl;
    throw err::Exception("Expected a symbol or a declaration");
  }

  auto obj = dynamic_cast<lex::OpSym *>(tokens.peek());
  auto lobj = dynamic_cast<lex::LObj *>(tokens.peek());

  if ((obj || (lobj && foreach)) && tokens.head->next != nullptr) {
    auto obj = dynamic_cast<lex::OpSym *>(tokens.peek());
    auto lobj = dynamic_cast<lex::LObj *>(tokens.peek());
    tokens.pop();

    if (obj && obj->Sym == deliminator) {
      auto s = new ast::Sequence;
      s->Statement1 = output;
      s->Statement2 =
          this->parseArgs(tokens, deliminator, close, types, required,
                          mutability, optConvertionIndices, readOnly);
      return s;
    } else if (obj && obj->Sym == close) {
      return output;
    } else if (lobj && lobj->meta == "in") {
      return output;
    }
  } else {
    throw err::Exception("Expected a symbol or a declaration");
  }
  return output;
}

ast::Type parse::Parser::parseFPointerType(
    links::LinkedList<lex::Token *> &tokens, const std::string typeName) {
  tokens.pop();
  auto callTypeList = std::vector<ast::Type>();
  std::string newTypeName = typeName + "~";
  int requiredCount = 0;
  std::vector<int> optConvertionIndices;
  while (true) {
    const auto closeSym = dynamic_cast<lex::Symbol *>(tokens.peek());
    if (closeSym != nullptr && closeSym->meta == ">") {
      tokens.pop();
      break;
    }
    if (dynamic_cast<lex::Ref *>(tokens.peek()) == nullptr) {
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
          dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '*')
        tokens.pop();
      else
        requiredCount++;
    } else {
      tokens.pop();
      optConvertionIndices.push_back(callTypeList.size());
    }
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

std::vector<std::string> parse::Parser::parseTemplateTypeList(
    links::LinkedList<lex::Token *> &tokens, int lineCount) {
  std::vector<std::string> list;
  auto templateSym = dynamic_cast<lex::Symbol *>(tokens.peek());
  if (templateSym != nullptr && templateSym->meta == "::") {
    tokens.pop();
    if (dynamic_cast<lex::Symbol *>(tokens.peek()) == nullptr ||
        dynamic_cast<lex::Symbol *>(tokens.peek())->meta != "<") {
      throw err::Exception("Expected < after :: on line " +
                           std::to_string(lineCount));
    }
    tokens.pop();
    while (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
      auto typeName = *dynamic_cast<lex::LObj *>(tokens.pop());
      if (this->typeList[typeName.meta] == nullptr)
        throw err::Exception("Unknown type " + typeName.meta);
      list.push_back(typeName.meta);
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
          dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',') {
        tokens.pop();
      } else if (dynamic_cast<lex::Symbol *>(tokens.peek()) != nullptr &&
                 dynamic_cast<lex::Symbol *>(tokens.peek())->meta == ">") {
        tokens.pop();
        break;
      } else {
        throw err::Exception("Expected , or > after template list on line " +
                             std::to_string(lineCount));
      }
    }
  }
  return list;
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
    fstringLiteral->original = fstringObj.value;

    std::size_t offset = 0;
    std::string original = fstringObj.value;

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

      int startLine =
          fstringObj.lineCount +
          std::count(original.begin(), original.begin() + pos + offset, '\n');
      auto tokes = lexer.Scan(expr, startLine);
      tokes.invert();
      auto exprAst = this->parseExpr(tokes);
      fstringLiteral->args.push_back(exprAst);

      offset += (pos2 - pos + 1) - 3;
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
    auto clean = false;
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
        } else if (sym.Sym == '@') {
          clean = true;
          tokens.pop();
        }
      }

    auto *var = new ast::Var();
    var->Ident = obj.meta;
    var->modList = modList;
    var->indices = indices;
    var->logicalLine = obj.lineCount;
    var->clean = clean;
    output = var;
    auto genericTypeList = this->parseTemplateTypeList(tokens, obj.lineCount);

    // check for the `->` operator if it is there that means that we are
    // constructing a union
    auto arrow = dynamic_cast<lex::Symbol *>(tokens.peek());
    if (arrow != nullptr && arrow->meta == "->") {
      tokens.pop();
      auto unionType = this->typeList[obj.meta];
      if (unionType == nullptr)
        throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                             " Unknown type " + obj.meta);
      auto variantName = dynamic_cast<lex::LObj *>(tokens.pop());
      if (variantName == nullptr)
        throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                             " Expected, Ident after ->");
      auto openParen = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (!openParen || openParen->Sym != '(') {
        throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                             " Expected, ( after ->");
      }
      tokens.pop();
      auto expr = this->parseExpr(tokens);
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
          dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ')') {
        tokens.pop();
      } else {
        throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                             " Expected, ) after union variant");
      }
      auto dynamic = false;
      output = new ast::UnionConstructor(*unionType, variantName->meta, expr,
                                         dynamic, genericTypeList);
    } else if (obj.meta == "new") {
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

      auto types = this->parseTemplateTypeList(tokens, obj.lineCount);
      // check for an arrow operator
      auto arrow = dynamic_cast<lex::Symbol *>(tokens.peek());
      if (arrow != nullptr && arrow->meta == "->") {
        tokens.pop();
        auto variantName = dynamic_cast<lex::LObj *>(tokens.pop());
        if (variantName == nullptr)
          throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                               " Expected, Ident after ->");
        auto openParen = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (!openParen || openParen->Sym != '(') {
          throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                               " Expected, ( after ->");
        }
        tokens.pop();
        auto expr = this->parseExpr(tokens);
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
            dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ')') {
          tokens.pop();
        } else {
          throw err::Exception("Line: " + std::to_string(obj.lineCount) +
                               " Expected, ) after union variant");
        }
        auto dynamic = true;
        output = new ast::UnionConstructor(*nType, variantName->meta, expr,
                                           dynamic, types);
        delete newExpr;
      } else {
        newExpr->templateTypes = std::move(types);
        auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (sym != nullptr && sym->Sym == '(') {
          tokens.pop();
          auto testSym = dynamic_cast<lex::OpSym *>(tokens.peek());
          newExpr->args = links::LinkedList<ast::Expr *>();
          if (testSym != nullptr && testSym->Sym != '[') {
            auto symp = dynamic_cast<lex::OpSym *>(tokens.pop());
            if (symp->Sym != ')')
              throw err::Exception(
                  &"Expected closed parenthesis got "[symp->Sym]);
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
                throw err::Exception(
                    &"Expected closed parenthesis got "[symp->Sym]);
            }
          }
        }
        output = newExpr;
      }
    } else if (obj.meta == "match") {
      output = new ast::Match(tokens, *this);
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
    } else if (obj.meta == "fn") {
      auto lambda = new ast::Lambda();
      lambda->logicalLine = obj.lineCount;
      auto openParen = dynamic_cast<lex::OpSym *>(tokens.pop());
      if (openParen == nullptr || openParen->Sym != '(')
        throw err::Exception("Expected '(' after fn on line " +
                             std::to_string(obj.lineCount));
      lambda->function = new ast::Function();
      lambda->function->req = 0;
      lambda->function->args = this->parseArgs(
          tokens, ',', ')', lambda->function->argTypes, lambda->function->req,
          lambda->function->mutability, lambda->function->optConvertionIndices,
          lambda->function->readOnly);
      auto dash = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (dash != nullptr && dash->Sym == '-') {
        tokens.pop();
        auto greater = dynamic_cast<lex::Symbol *>(tokens.pop());
        if (greater == nullptr || greater->meta != ">")
          throw err::Exception("Expected '>' after '-' on line " +
                               std::to_string(obj.lineCount));
        auto typeName = dynamic_cast<lex::LObj *>(tokens.pop());
        if (typeName == nullptr)
          throw err::Exception("Expected type after -> on line " +
                               std::to_string(obj.lineCount));
        auto type = this->typeList[typeName->meta];
        if (type == nullptr)
          throw err::Exception("Unknown type " + typeName->meta);
        lambda->function->type = *type;
      } else {
        lambda->function->type = *typeList["void"];
        lambda->function->autoType = true;
      }

      auto openCurl = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (openCurl != nullptr && openCurl->Sym == '{') {
        tokens.pop();
        lambda->function->statement = this->parseStmt(tokens, false);
      } else
        lambda->function->statement = this->parseStmt(tokens, true);
      output = lambda;
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
        if (testSym != nullptr && testSym->Sym != '[' && testSym->Sym != '{') {
          auto symp = dynamic_cast<lex::OpSym *>(tokens.pop());
          if (symp->Sym != ')')
            throw err::Exception(
                &"Expected closed parenthesis got "[symp->Sym]);
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
              throw err::Exception(
                  &"Expected closed parenthesis got "[symp->Sym]);
          }
        }

        ast::CallExpr *callExpr = new ast::CallExpr;
        callExpr->templateTypes = std::move(genericTypeList);
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
          tokens, ',', ']', lambda->function->argTypes, lambda->function->req,
          lambda->function->mutability, lambda->function->optConvertionIndices,
          lambda->function->readOnly);

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
      lambda->function->autoType = true;

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
        newExpr->castableMap = true;
        output = newExpr;
      } else {
        auto first_arg = this->parseExpr(tokens);
        sym = dynamic_cast<lex::OpSym *>(tokens.peek());
        if (sym != nullptr && (sym->Sym == ',' || sym->Sym == '}')) {
          auto structList = new ast::StructList();
          structList->args.push(first_arg);
          lex::OpSym *symp;

          if (sym->Sym == ',') {
            do {
              tokens.pop();
              auto checkEnd = dynamic_cast<lex::OpSym *>(tokens.peek());
              if (checkEnd != nullptr && checkEnd->Sym == '}') {
                symp = checkEnd;
                break;
              }
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

            auto checkEnd = dynamic_cast<lex::OpSym *>(tokens.peek());
            if (checkEnd != nullptr && checkEnd->Sym == '}') {
              sym = checkEnd;
              break;
            }

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
    } else if (eq.Sym == '$') {
      // This is the buy operator it will cast an lvalue to an rvalue by
      // invalidating the lvalue and calling any possible move constructor
      tokens.pop();
      auto buy = new ast::Buy();
      buy->expr = this->parseExpr(tokens);
      buy->expr->selling = true;
      buy->logicalLine = eq.lineCount;
      output = buy;
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
