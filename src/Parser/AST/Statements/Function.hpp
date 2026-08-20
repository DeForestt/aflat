#pragma once
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"

namespace ast {
/**
 * @class Function
 * @brief Represents a function declaration.
 */
class Function : public Member, public Statement {
public:
  ScopeMod scope = Public;
  bool safe = false; // is this a class method that does not mutate
                     // the class instance?
  Type type;
  Type useType = Type("", asmc::QWord);
  std::string scopeName = "global";
  std::string decorator = "";
  std::string decNSP = "";
  std::vector<std::string> decoratorTemplateTypes;
  bool hidden = false;
  Ident ident;
  Statement *args = nullptr;
  Statement *statement = nullptr;
  Op op = None;
  int req = 0;
  std::vector<int> optConvertionIndices; // indices of optional arguments that
                                         // need to be converted
  std::vector<Type> argTypes;
  std::vector<std::string> genericTypes;
  links::LinkedList<Expr *> decoratorArgs;
  bool wasGeneric = false;
  std::vector<bool> mutability;
  std::vector<bool> readOnly; // if the argument is read only
  bool isLambda = false;
  bool isAsync = false;
  bool isAsyncBody = false;
  bool sinksReceiver = false;
  int asyncStateCounter = 0;
  int nextAsyncState() { return ++asyncStateCounter; }
  std::string asyncStateLabel(int state) const {
    return "__af_async_state_" +
           (scopeName == "global" ? "" : scopeName + "_") + ident.ident + "_" +
           std::to_string(state);
  }
  std::string asyncStartLabel() const {
    return "__af_async_start_" +
           (scopeName == "global" ? "" : scopeName + "_") + ident.ident;
  }
  bool flex = false;
  bool mask = false;
  bool has_return = false;
  bool optional = false;
  bool error = false; // if the function can return an error
  bool autoType = false;
  bool globalLocked = false;    // if the function is locked to the global scope
  bool returnImmutable = false; // if callers must bind return to immutable sym
  bool returnLowOwnership = false; // if the return value does not yield owner
  int overloadIndex = 0;

  Function() = default;
  Function(const string &ident, const ScopeMod &scope, const Type &type,
           const Op op, const std::string &scopeName,
           links::LinkedList<lex::Token *> &tokens, parse::Parser &parser,
           bool optional, bool safe = false, bool sinksReceiver = false);
  Function(const ScopeMod &scope, links::LinkedList<lex::Token *> &tokens,
           std::vector<std::string> genericTypes, parse::Parser &parser,
           bool safe = false, bool isAsync = false, bool sinksReceiver = false);
  Function(const Function &Other, bool locked, bool copyBody = true)
      : scope(Other.scope), type(Other.type), op(Other.op),
        scopeName(Other.scopeName), ident(Other.ident), args(nullptr),
        statement(nullptr), decorator(Other.decorator), decNSP(Other.decNSP),
        decoratorTemplateTypes(Other.decoratorTemplateTypes),
        decoratorArgs(Other.decoratorArgs), argTypes(Other.argTypes),
        optConvertionIndices(Other.optConvertionIndices),
        mutability(Other.mutability), isLambda(Other.isLambda),
        mask(Other.mask), has_return(Other.has_return),
        optional(Other.optional), useType(Other.useType),
        genericTypes(Other.genericTypes), autoType(Other.autoType),
        globalLocked(Other.globalLocked), safe(Other.safe),
        isAsync(Other.isAsync), isAsyncBody(Other.isAsyncBody),
        sinksReceiver(Other.sinksReceiver),
        asyncStateCounter(Other.asyncStateCounter), readOnly(Other.readOnly),
        returnImmutable(Other.returnImmutable),
        returnLowOwnership(Other.returnLowOwnership), error(Other.error) {
    this->logicalLine = Other.logicalLine;
    this->locked = locked;
    this->hidden = Other.hidden;
    this->when = Other.when;
    // Specializations clone their AST so deepCopy resets parser-only coverage
    // points. Declaration shells deliberately omit the implementation body.
    this->args = deepCopy(Other.args);
    this->statement = copyBody ? deepCopy(Other.statement) : nullptr;
  }
  gen::GenerationResult const generate(gen::CodeGenerator &generator) override;
  gen::Expr toExpr(gen::CodeGenerator &generator);

private:
  void parseFunctionBody(links::LinkedList<lex::Token *> &tokens,
                         parse::Parser &parser);
};

void resetSharedGenericFunctionEmissions();
}; // namespace ast
