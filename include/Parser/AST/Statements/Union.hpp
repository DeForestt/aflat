#pragma once
#include <optional>
#include <utility>
#include <variant>

#include "Parser/AST.hpp"
#include "Parser/AST/Statements/Class.hpp"

/*
 * @class Union
 * @brief This class will represent a union it can store One of n types.  It
 *behaves similarly to a class or struct and will support pattern matching with
 *the `match` statement. syntax: union UnionName { Alias1(Type1), Alias2(Type2),
 *  Alias3(expression), // Note: that a constant expression can be used and the
 *compiler will determine the type and always return
 *                      // The value of the expression
 *  Alias4, // Note: This will be an int returning some value
 *  ... <statement> // This statment is used to add implementations to the
 *union, such as methods or properties.
 *
 **/
namespace ast {
class Union : public Class {
 public:
  struct Alias {
    std::string name;
    std::optional<std::variant<ast::Type, ast::Expr *>> value;
    Alias(std::string name,
          std::optional<std::variant<ast::Type, ast::Expr *>> value =
              std::nullopt)
        : name(std::move(name)), value(std::move(value)) {}

    bool isUnit() const { return !value.has_value(); }
    bool isType() const {
      return value && std::holds_alternative<ast::Type>(*value);
    }
    bool isConstExpr() const {
      return value && std::holds_alternative<ast::Expr *>(*value);
    }
    ast::Type getType() const noexcept {
      return isType() ? std::get<ast::Type>(*value)
                      : ast::Type("Invalid", asmc::QWord);
    }
    ast::Expr *getConstExpr() const noexcept {
      return isConstExpr() ? std::get<ast::Expr *>(*value) : nullptr;
    }
    std::string toString() const {
      if (isUnit()) return name;
      if (isType()) return name + "(" + getType().typeName + ")";
      if (isConstExpr()) return name + "(" + getConstExpr()->toString() + ")";
      return name;  // Fallback, should not happen
    }
  };

  // we will create a statment that can be used to add implementations to
  // unions.
  Statement *statement = nullptr;

  // std::vector<std::string> genericTypes = std::vector<std::string>();
  std::vector<Alias *> aliases;

  Union() = default;
  Union(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser,
        bool uniqueType, std::vector<std::string> &genericTypes);

  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  std::vector<std::string> getGenericTypes() const override {
    return genericTypes;
  }

  std::string toString() const {
    std::string result = "union " + ident.ident + " {\n";
    for (const auto &alias : aliases) {
      result += "  " + alias->toString() + ",\n";
    }
    if (statement) {
      result += "  ... " + statement->toString() + "\n";
    }
    result += "}";
    return result;
  }
};
};  // namespace ast
