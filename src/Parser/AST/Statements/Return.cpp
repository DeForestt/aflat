#include "Parser/AST/Statements/Return.hpp"

#include "ASM.hpp"
#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements/Call.hpp"
#include "Parser/Parser.hpp"

namespace ast {
/*
 * @brief This will parse a return statement
 * @param tokens The tokens to parse
 * @param pa
 */
Return::Return(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  if (!(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
        dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ';')) {
    this->expr = parser.parseExpr(tokens);
    this->logicalLine = this->expr->logicalLine;
  } else {
    auto nu = new ast::Var();
    nu->Ident = "**void_type**";
    nu->logicalLine = tokens.peek()->lineCount;
    this->empty = true;
    this->expr = nu;
  };
}

gen::GenerationResult const Return::generate(gen::CodeGenerator &generator) {
  asmc::File file;

  if (implicit && generator.matchScope()) {
    resolver = true; // if this is an implicit return, and
  }

  if (resolver) {
    auto from = generator.GenExpr(this->expr, file);
    if (generator.matchScope() == nullptr) {
      generator.alert(
          "cannot use a resolver return outside of a match statement", true,
          __FILE__, __LINE__);
    }
    if (generator.matchScope()->returns.typeName == "void") {
      auto it = parse::PRIMITIVE_TYPES.find(from.type);
      auto size = asmc::QWord;
      if (it != parse::PRIMITIVE_TYPES.end()) {
        size = gen::utils::toSize(it->second);
      }
      generator.matchScope()->returns.typeName = from.type;
      generator.matchScope()->returns.size = size;
    } else if (!generator.canAssign(
                   generator.matchScope()->returns, from.type,
                   "the return type of the match statement is {} but "
                   "the resolver returns {}")) {
      auto imo =
          generator.imply(this->expr, generator.matchScope()->returns.typeName);
      auto prev = from;
      from = generator.GenExpr(imo, file);
      from.adoptImmutableRequirement(prev);
    }
    // if this is a resolver, we just need to put the expression into %rax
    if (from.op != asmc::Float) {
      auto mov = new asmc::Mov();
      mov->logicalLine = this->logicalLine;
      mov->from = from.access;
      mov->to = generator.registers()["%rax"]->get(from.size);
      mov->size = from.size;
      mov->op = from.op;
      file.text << mov;
    } else {
      auto mov = new asmc::Mov();
      mov->logicalLine = this->logicalLine;
      mov->from = from.access;
      mov->to = generator.registers()["%xmm0"]->get(from.size);
      mov->size = from.size;
      mov->op = from.op;
      file.text << mov;
    }
    return {file, std::nullopt};
  }

  auto mov = new asmc::Mov();
  mov->logicalLine = this->logicalLine;

  auto trashFile = asmc::File();

  gen::Expr from = generator.GenExpr(this->expr, file);

  gen::Symbol *retSym = nullptr;
  if (auto var = dynamic_cast<ast::Var *>(this->expr)) {
    if (var->modList.count == 0 && var->Ident != "my") {
      retSym = gen::scope::ScopeManager::getInstance()->get(var->Ident);
      if (retSym)
        retSym->returned = true;
    }
  }

  if (generator.currentFunction()->optional) {
    // if fromtype is not option.typeName, we need to convert it to
    // option.typeName
    if (from.type != "option<" + generator.returnType().typeName + ">") {
      if (!this->empty &&
          !generator.canAssign(generator.returnType(), from.type,
                               "the return type of this function is {} but the "
                               "expression returns {}")) {
        auto imp = generator.imply(this->expr, generator.returnType().typeName);
        this->expr = imp;
      }
      if (this->empty) {
        auto nu = new ast::Var();
        nu->Ident = "NULL";
        nu->logicalLine = this->logicalLine;
        this->expr = nu;
      }
      auto optionConvertion = new ast::Call();
      optionConvertion->ident = "option.optionWrapper";
      optionConvertion->Args.push(this->expr);
      auto call = new ast::CallExpr();
      call->call = optionConvertion;
      call->logicalLine = this->logicalLine;
      auto prev = from;
      from = generator.GenExpr(call, file);
      from.adoptImmutableRequirement(prev);
    }
  } else if (generator.currentFunction()->error) {
    // if fromtype is not result.typeName, we need to convert it to
    // result.typeName
    if (from.type != "result<" + generator.returnType().typeName + ">") {
      bool isError = false;
      if (parse::PRIMITIVE_TYPES.find(from.type) ==
          parse::PRIMITIVE_TYPES.end()) {
        if (auto t = generator.getType(from.type, file)) {
          auto type = *t;
          if (type->Ident == "Error") {
            isError = true;
          } else {
            if (auto classType = dynamic_cast<ast::Class *>(type)) {
              if (classType->ident.ident == "Error") {
                isError = true;
              } else if (classType->base == "Error") {
                isError = true;
              }
            }
          }
        }
      }

      if (isError) {
        auto reject = new ast::Call();
        reject->ident = "result.reject";
        reject->Args.push(this->expr);
        reject->genericTypes.push_back(generator.returnType().typeName);
        auto call = new ast::CallExpr();
        call->call = reject;
        call->logicalLine = this->logicalLine;
        call->templateTypes = generator.currentFunction()->genericTypes;
        auto prev = from;
        from = generator.GenExpr(call, file);
        from.adoptImmutableRequirement(prev);
      } else {
        if (!this->empty &&
            !generator.canAssign(
                generator.returnType(), from.type,
                "the return type of this function is {} but the "
                "expression returns {}")) {
          auto imp =
              generator.imply(this->expr, generator.returnType().typeName);
          this->expr = imp;
        }
        if (this->empty) {
          auto nu = new ast::Var();
          nu->Ident = "NULL";
          nu->logicalLine = this->logicalLine;
          this->expr = nu;
        }
        auto resultConvertion = new ast::Call();
        resultConvertion->ident = "result.resultWrapper";
        resultConvertion->Args.push(this->expr);
        auto call = new ast::CallExpr();
        call->call = resultConvertion;
        call->logicalLine = this->logicalLine;
        auto prev = from;
        from = generator.GenExpr(call, file);
        from.adoptImmutableRequirement(prev);
      }
    }
  }

  if (from.requiresImmutableBinding &&
      !generator.currentFunction()->returnImmutable) {
    auto source = from.immutableBindingSource.empty()
                      ? std::string("this expression")
                      : from.immutableBindingSource;
    generator.alert("return value derived from `" + source +
                        "` must be bound to an immutable symbol; "
                        "mark the function's return type as immutable",
                    true, __FILE__, __LINE__);
  }

  if (generator.currentFunction()->isLambda &&
      generator.lambdaReturns() == "void") {
    // adopt the new return type
    generator.lambdaReturns() = from.type;
    generator.lambdaSize() = from.size;
    generator.returnType() = ast::Type(from.type, from.size);
  }

  if (generator.currentFunction()->autoType) {
    generator.currentFunction()->autoType = false;
    generator.currentFunction()->type = ast::Type(from.type, from.size);

    if (generator.scope() == nullptr ||
        generator.currentFunction()->genericTypes.size() > 0 ||
        generator.currentFunction()->globalLocked) {
      if (generator.nameTable()[generator.currentFunction()->ident.ident] !=
          nullptr) {
        generator.nameTable()[generator.currentFunction()->ident.ident]->type =
            ast::Type(from.type, from.size);
        generator.nameTable()[generator.currentFunction()->ident.ident]
            ->useType = ast::Type(from.type, from.size);
      }
    } else {
      if (generator.scope()
              ->nameTable[generator.currentFunction()->ident.ident] ==
          nullptr) {
        generator.alert("the function " +
                            generator.currentFunction()->ident.ident +
                            " is not defined in the current scope `" +
                            generator.scope()->Ident +
                            "` , but it is being returned "
                            "from",
                        true, __FILE__, __LINE__);
      }
      generator.scope()
          ->nameTable[generator.currentFunction()->ident.ident]
          ->type = ast::Type(from.type, from.size);
      const auto pub =
          generator.scope()
              ->publicNameTable[generator.currentFunction()->ident.ident];
      if (pub != nullptr) {
        pub->type = ast::Type(from.type, from.size);
      }
    }

    generator.returnType() = ast::Type(from.type, from.size);
  }

  if (!from.passable) {
    generator.alert("cannot return an lvalue reference to safe type " +
                    from.type);
  };

  if (from.op != asmc::Float) {
    // move from.access to %rax
    auto mov2 = new asmc::Mov();
    mov2->logicalLine = this->logicalLine;
    mov2->size = from.size;
    mov2->from = from.access;
    mov2->to = generator.registers()["%rax"]->get(from.size);
    mov2->op = from.op;
    file.text << mov2;
    auto push = new asmc::Push();
    push->logicalLine = this->logicalLine;
    push->op = generator.registers()["%rax"]->get(asmc::QWord);
    file.text << push;
  };

  if (!generator.currentFunction()->optional &&
      !generator.currentFunction()->error &&
      !generator.canAssign(generator.returnType(), from.type,
                           "the return type of this function is {} but the "
                           "expression returns {}")) {
    auto imp = generator.imply(this->expr, generator.returnType().typeName);
    auto prev = from;
    from = generator.GenExpr(imp, file);
    from.adoptImmutableRequirement(prev);
  };

  if (parse::PRIMITIVE_TYPES.find(from.type) == parse::PRIMITIVE_TYPES.end()) {
    if (!from.owned && from.type != "void" &&
        from.type != "--std--flex--function" &&
        !generator.currentFunction()->returnLowOwnership) {
      generator.alert("cannot return a non-owned reference to a type " +
                          from.type,
                      true, __FILE__, __LINE__);
    }
  }

  if (from.op != asmc::Float) {
    auto pop = new asmc::Pop();
    pop->logicalLine = this->logicalLine;
    pop->op = generator.registers()["%rax"]->get(asmc::QWord);
    file.text << pop;
  };

  std::string move2 = (from.op == asmc::Float)
                          ? generator.registers()["%xmm0"]->get(from.size)
                          : generator.registers()["%rax"]->get(from.size);

  mov->from = from.access;
  mov->to = move2;
  mov->size = from.size;
  mov->op = from.op;
  file.text << mov;

  gen::scope::ScopeManager::getInstance()->softPop(&generator, file);
  if (retSym)
    retSym->returned = false;

  auto re = new asmc::Return();
  re->logicalLine = this->logicalLine;
  file.text << re;
  return {file, std::nullopt};
}
} // namespace ast
