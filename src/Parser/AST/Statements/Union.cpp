#include "Parser/AST/Statements/Union.hpp"

#include <algorithm>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <unordered_map>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"

namespace ast {

std::vector<ast::Union::Alias *>
parseAliases(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  std::vector<ast::Union::Alias *> aliases;
  lex::OpSym *comma = nullptr;
  bool first = true;
  do {
    if (!first) {
      tokens.pop(); // pop the comma
    } else {
      first = false;
    }

    auto aliasIdent = dynamic_cast<lex::LObj *>(tokens.pop());

    if (aliasIdent == nullptr) {
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Union alias needs Ident");
    }

    std::string name = aliasIdent->meta;

    std::optional<std::variant<ast::Type, ast::Expr *>> value = std::nullopt;

    comma = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (comma == nullptr || comma->Sym != '(') {
      // If there is no Open Peren, it means this alias is a unit type
      aliases.push_back(new Union::Alias(name));
      continue;
    }

    if (comma->Sym == '(') {
      tokens.pop();
      auto maybeTypeName = dynamic_cast<lex::LObj *>(tokens.peek());
      auto type =
          maybeTypeName ? parser.getTypeList()[maybeTypeName->meta] : nullptr;
      if (type) {
        tokens.pop(); // pop the type name
        auto templateArgs =
            parser.parseTemplateTypeList(tokens, maybeTypeName->lineCount);

        if (!templateArgs.empty()) {
          type->typeName += "<" + templateArgs[0];
          for (size_t i = 1; i < templateArgs.size(); ++i) {
            type->typeName += ", " + templateArgs[i];
          }
          type->typeName += ">";
        }
        // std::cout << "Parsed type: " << type->typeName << std::endl;
        ast::Type aliasType(type->typeName, type->size);

        const auto sym = dynamic_cast<lex::Symbol *>(tokens.peek());
        if (sym && sym->meta == "<")
          aliasType = parser.parseFPointerType(tokens, aliasType.typeName);

        auto templateTypes =
            parser.parseTemplateTypeList(tokens, maybeTypeName->lineCount);
        if (!templateTypes.empty()) {
          for (auto &tName : templateTypes)
            aliasType.typeName += "." + tName;
        }

        std::variant<ast::Type, ast::Expr *> typeOrExpr = aliasType;
        value = typeOrExpr;
      } else {
        auto expr = parser.parseExpr(tokens);
        if (expr == nullptr) {
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Union alias needs Type or Expression");
        }
        value = expr;
      }
      auto closeParen = dynamic_cast<lex::OpSym *>(tokens.pop());
      if (closeParen == nullptr || closeParen->Sym != ')') {
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Union alias needs closing parenthesis");
      }
      comma = dynamic_cast<lex::OpSym *>(tokens.peek());
    }
    aliases.push_back(new Union::Alias(name, value));

  } while (comma != nullptr && comma->Sym == ',');

  return aliases;
}

Union::Union(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser,
             bool uniqueType, std::vector<std::string> &genericTypes) {
  this->genericTypes = genericTypes;
  this->logicalLine = tokens.peek()->lineCount;

  if (auto ident = dynamic_cast<lex::LObj *>(tokens.pop())) {
    this->ident.ident = ident->meta;
  } else {
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " union needs Ident");
  }

  auto type = ast::Type(this->ident.ident, asmc::QWord);
  type.uniqueType = uniqueType;
  parser.getTypeList() << type; // add the type to the typeList
  this->uniqueType = uniqueType;
  auto op = dynamic_cast<lex::OpSym *>(tokens.pop());

  if (op == nullptr || op->Sym != '{') {
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Unopened Union");
  }

  this->aliases = parseAliases(tokens, parser);

  auto closeBrace = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (closeBrace == nullptr || closeBrace->Sym != '}') {
    this->statement = parser.parseStmt(tokens);
  } else {
    this->statement = nullptr;
    tokens.pop(); // pop the closing brace
  }
};

gen::GenerationResult const Union::generate(gen::CodeGenerator &generator) {
  // if the union is generic, do not generate code for it. It will be
  // generated when it is instantiated with specific types.
  if (this->genericTypes.size() > 0) {
    generator.genericTypes().insert(
        {this->ident.ident, dynamic_cast<ast::Union *>(ast::deepCopy(
                                this))}); // add the union to the generic types
    return {asmc::File(), std::nullopt};
  }

  asmc::File OutputFile;
  gen::Union *type = new gen::Union();

  type->hidden = this->hidden;
  type->body = this->statement; // save the body in case of composition

  bool saveScope = generator.globalScope();
  generator.globalScope() = false;
  type->Ident = this->ident.ident;
  type->nameTable.foo = gen::utils::compareFunc;
  type->publicNameTable.foo = gen::utils::compareFunc;
  type->safeType = this->safeType;
  type->dynamic = this->dynamic;
  type->pedantic = this->pedantic;
  type->uniqueType = this->uniqueType;
  generator.scope() = type;

  type->overloadTable.foo = [](ast::Function func, ast::Op op) {
    if (func.op == op) {
      return true;
    }
    return false;
  };

  generator.typeList().push(type);

  auto hoist = gen::utils::copyAllFunctionShells(this->statement);
  if (hoist != nullptr) {
    OutputFile << generator.GenSTMT(hoist);
  }

  if (this->includer && this->genericTypes.size() == 0) {
    gen::utils::shellStatement(this->statement);
  }

  asmc::File junkFile =
      asmc::File(); // We can use this to get the types and size of aliases

  for (const auto &alias : this->aliases) {
    gen::Expr expr;
    if (alias->isUnit()) {
      // need to create an int expr with a unique value...
      // It needs to be resonabley different from other aliases so some high
      // number just in case

      auto intLit = new ast::IntLiteral();
      intLit->val = generator.tempCount()++ +
                    1000000; // Start from a high number to avoid conflicts

      type->aliases.emplace_back(alias->name, intLit, 4);
    } else if (alias->isType()) {
      auto typePtr =
          new ast::Type(alias->getType().typeName, alias->getType().size);

      if (parse::PRIMITIVE_TYPES.find(typePtr->typeName) !=
          parse::PRIMITIVE_TYPES.end()) {
        type->aliases.emplace_back(alias->name, typePtr,
                                   parse::PRIMITIVE_TYPES[typePtr->typeName]);
      } else {
        // For other types, we can use the size from the typeList
        auto t = generator.getType(typePtr->typeName, OutputFile);
        if (!t) {
          generator.alert("Type not found: " + typePtr->typeName, true,
                          __FILE__, __LINE__);
        } else {
          auto typ = *t;
          if (dynamic_cast<gen::Enum *>(typ)) {
            // always use 4 bytes for enums
            type->aliases.emplace_back(alias->name, typePtr, 4);
          } else {
            auto size = typ->SymbolTable.head->data.byteMod;
            if (size <= 0) {
              generator.alert("Type " + typePtr->typeName + " has invalid size",
                              true, __FILE__, __LINE__);
            } else {
              type->aliases.emplace_back(alias->name, typePtr, size);
            }
          }
        }
      }
    } else if (alias->isConstExpr()) {
      // We need to evaluate the expression and determine its type
      auto exprPtr = alias->getConstExpr();
      if (exprPtr == nullptr) {
        generator.alert("Constant expression is null for alias: " + alias->name,
                        true, __FILE__, __LINE__);
      } else {
        expr = generator.GenExpr(exprPtr, junkFile);
        if (expr.size == asmc::AUTO) {
          generator.alert(
              "Could not determine size of constant expression for alias: " +
                  alias->name,
              true, __FILE__, __LINE__);
        }
        type->aliases.emplace_back(alias->name, exprPtr, expr.size);
      }
    } else {
      generator.alert("Unknown alias type for: " + alias->name, true, __FILE__,
                      __LINE__);
    }
  }

  // Now we need to find the largest alias size, this will be the size of the
  // union

  int maxSize =
      type->aliases.empty()
          ? 0
          : std::max_element(type->aliases.begin(), type->aliases.end(),
                             [](const auto &a, const auto &b) {
                               return a.byteSize < b.byteSize;
                             })
                ->byteSize;

  type->largestSize = maxSize;
  // now, before anything else, we need to add a symbol for the union portion...
  // all this does is alocate the memory in the object

  gen::Symbol symbol;
  symbol.type = ast::Type("any", asmc::AUTO);
  symbol.symbol = boost::uuids::to_string(boost::uuids::random_generator()());
  symbol.byteMod = maxSize; // This is the size of the union
  type->SymbolTable.push(symbol);

  // now we need a symbol for the type (just the index of the alias)
  gen::Symbol typeSymbol;
  typeSymbol.type = ast::Type("int", asmc::DWord);
  typeSymbol.symbol = "type"; // This is the type of the union
  typeSymbol.byteMod =
      maxSize + 4; // This is the size of the union + 4 bytes for the type
  type->SymbolTable.push(typeSymbol);

  if (this->statement != nullptr) {
    asmc::File file = generator.GenSTMT(this->statement);
    OutputFile << file;
  }

  generator.globalScope() = saveScope;
  generator.scope() = nullptr;

  return {OutputFile, std::nullopt};
}
}; // namespace ast
