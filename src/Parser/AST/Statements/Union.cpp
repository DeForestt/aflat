#include "Parser/AST/Statements/Union.hpp"

#include <algorithm>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <unordered_map>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements/Match.hpp"
#include "Parser/Parser.hpp"

namespace ast {

namespace {

ast::Assign *buildUnionInvalidateBody(int logicalLine) {
  auto *assign = new ast::Assign();
  assign->logicalLine = logicalLine;
  assign->Ident = "my";
  assign->modList.push("type");
  assign->override = true;

  auto *invalidTag = new ast::IntLiteral();
  invalidTag->logicalLine = logicalLine;
  invalidTag->val = -1;
  assign->expr = invalidTag;
  return assign;
}

ast::Function *buildAutomaticInvalidate(const gen::Union *type,
                                        int logicalLine) {
  auto *func = new ast::Function();
  func->logicalLine = logicalLine;
  func->ident.ident = "__invalidate__";
  func->scope = ast::Public;
  func->hidden = type->declarationOnly;
  func->args = nullptr;
  func->statement = buildUnionInvalidateBody(logicalLine);
  func->type.typeName = "void";
  func->type.size = asmc::QWord;
  func->useType = func->type;
  return func;
}

ast::Function *buildAutomaticTransfer(const gen::Union *type, int logicalLine) {
  auto *func = new ast::Function();
  func->logicalLine = logicalLine;
  func->ident.ident = "__transfer_to__";
  func->scope = ast::Public;
  func->hidden = type->declarationOnly;

  auto *buffer = new ast::Declare();
  buffer->logicalLine = logicalLine;
  buffer->ident = "buffer";
  buffer->type = ast::Type("adr", asmc::QWord);
  buffer->mut = false;
  buffer->readOnly = true;
  func->args = buffer;
  func->argTypes.push_back(buffer->type);
  func->req = 1;
  func->readOnly.push_back(true);
  func->mutability.push_back(false);

  auto *body = new ast::Sequence();
  auto *copy = new ast::Call();
  copy->logicalLine = logicalLine;
  copy->ident = "af_memcpy";

  auto *dst = new ast::Var();
  dst->logicalLine = logicalLine;
  dst->Ident = "buffer";
  copy->Args.push(dst);

  // The receiver is already an address to the union storage. `?my` would
  // address the local receiver slot and transfer stack bytes.
  auto *src = new ast::Var();
  src->logicalLine = logicalLine;
  src->Ident = "my";
  src->clean = true;
  copy->Args.push(src);

  auto *size = new ast::IntLiteral();
  size->logicalLine = logicalLine;
  size->val = type->instanceSize;
  copy->Args.push(size);
  body->Statement1 = copy;

  auto *invalidate = new ast::Call();
  invalidate->logicalLine = logicalLine;
  invalidate->ident = "my";
  invalidate->modList.push("__invalidate__");
  body->Statement2 = invalidate;

  func->statement = body;
  func->type.typeName = "void";
  func->type.size = asmc::QWord;
  func->useType = func->type;
  return func;
}

ast::Function *buildAutomaticDestructor(gen::CodeGenerator &generator,
                                        const gen::Union *type,
                                        int logicalLine) {
  auto *match = new ast::Match();
  match->logicalLine = logicalLine;
  match->loanBindings = true;
  auto *subject = new ast::Var();
  subject->logicalLine = logicalLine;
  subject->Ident = "my";
  match->expr = subject;

  int payloadIndex = 0;
  for (const auto &alias : type->aliases) {
    if (!std::holds_alternative<ast::Type *>(alias.value))
      continue;

    auto *payloadType = std::get<ast::Type *>(alias.value);
    auto **entry = generator.typeList()[payloadType->typeName];
    auto *payloadClass =
        entry == nullptr ? nullptr : dynamic_cast<gen::Class *>(*entry);
    if (payloadClass == nullptr)
      continue;

    const char *methodName = payloadClass->uniqueType ? "del" : "endScope";
    if (payloadClass->nameTable[methodName] == nullptr)
      continue;

    const std::string payloadName =
        "__union_payload_" + std::to_string(payloadIndex++);
    auto *cleanup = new ast::Call();
    cleanup->logicalLine = logicalLine;
    cleanup->ident = payloadName;
    cleanup->modList.push(methodName);
    match->cases.emplace_back(ast::Match::Pattern(alias.name, payloadName),
                              cleanup);
  }

  match->cases.emplace_back(ast::Match::Pattern("_", std::nullopt),
                            new ast::Sequence());

  auto *body = new ast::Sequence();
  body->Statement1 = match;
  body->Statement2 = buildUnionInvalidateBody(logicalLine);

  auto *func = new ast::Function();
  func->logicalLine = logicalLine;
  func->ident.ident = "del";
  func->scope = ast::Public;
  func->hidden = type->declarationOnly;
  func->args = nullptr;
  func->statement = body;
  func->type.typeName = "void";
  func->type.size = asmc::QWord;
  func->useType = func->type;
  return func;
}

} // namespace

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
      throw err::Exception(
          "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
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
            type->typeName += "," + templateArgs[i];
          }
          type->typeName += ">";
        }
        // std::cout << "Parsed type: " << type->typeName << std::endl;
        ast::Type aliasType(*type);

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
              "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
              " Union alias needs Type or Expression");
        }
        value = expr;
      }
      auto closeParen = dynamic_cast<lex::OpSym *>(tokens.pop());
      if (closeParen == nullptr || closeParen->Sym != ')') {
        throw err::Exception(
            "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
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
  this->logicalLine = lex::tokenLine(tokens.peek());

  if (auto ident = dynamic_cast<lex::LObj *>(tokens.pop())) {
    this->ident.ident = ident->meta;
  } else {
    throw err::Exception(
        "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
        " union needs Ident");
  }

  auto type = ast::Type(this->ident.ident, asmc::QWord);
  type.uniqueType = uniqueType;
  parser.getTypeList() << type; // add the type to the typeList
  this->uniqueType = uniqueType;
  auto op = dynamic_cast<lex::OpSym *>(tokens.pop());

  if (op == nullptr || op->Sym != '{') {
    throw err::Exception(
        "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
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

Union::~Union() {
  for (auto *alias : aliases)
    delete alias;
}

gen::GenerationResult const Union::generate(gen::CodeGenerator &generator) {
  // if the union is generic, do not generate code for it. It will be
  // generated when it is instantiated with specific types.
  if (this->genericTypes.size() > 0) {
    auto &templates = generator.genericTypes();
    auto existing = templates.find(this->ident.ident);
    if (existing == templates.end() || existing->second == nullptr ||
        existing->second->templateModuleRoot == nullptr)
      templates[this->ident.ident] = dynamic_cast<ast::Union *>(
          ast::deepCopy(this)); // add the union to the generic types
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
  type->declarationOnly = this->includer;
  generator.scope() = type;

  type->overloadTable.foo = [](const ast::Function &func, const ast::Op &op) {
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
      auto typePtr = new ast::Type(alias->getType());

      if (typePtr->typeName.find("~") != std::string::npos &&
          typePtr->typeName.find("<") == std::string::npos) {
        type->aliases.emplace_back(alias->name, typePtr, 8);
      } else if (parse::PRIMITIVE_TYPES.find(typePtr->typeName) !=
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
            auto *classType = dynamic_cast<gen::Class *>(typ);
            auto size = 0;
            if (typ->SymbolTable.head != nullptr) {
              size = typ->SymbolTable.head->data.byteMod;
            } else if (classType != nullptr && !classType->declarationOnly) {
              size = 1;
            }
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
  type->instanceSize = maxSize + 4;
  type->layoutFinalized = true;
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

  if (type->uniqueType) {
    // Concrete generic methods are normally emitted lazily on first call.
    // Union lifecycle methods are compiler-required entry points used by
    // scope cleanup, so their bodies must always be available.
    for (const auto *method : {"del", "__invalidate__", "__transfer_to__"}) {
      generator.generatedLazyConcreteMethodNames().insert("pub_" + type->Ident +
                                                          "_" + method);
    }
    if (gen::utils::extract("del", this->statement) == nullptr)
      OutputFile << generator.GenSTMT(
          buildAutomaticDestructor(generator, type, this->logicalLine));
    if (gen::utils::extract("__invalidate__", this->statement) == nullptr)
      OutputFile << generator.GenSTMT(
          buildAutomaticInvalidate(type, this->logicalLine));
    if (gen::utils::extract("__transfer_to__", this->statement) == nullptr)
      OutputFile << generator.GenSTMT(
          buildAutomaticTransfer(type, this->logicalLine));
  }

  if (this->statement != nullptr) {
    asmc::File file = generator.GenSTMT(this->statement);
    OutputFile << file;
  }

  generator.globalScope() = saveScope;
  generator.scope() = nullptr;

  return {OutputFile, std::nullopt};
}
}; // namespace ast
