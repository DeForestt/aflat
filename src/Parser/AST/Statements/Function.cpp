#include "Parser/AST/Statements/Function.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"
#include "Scanner.hpp"

namespace ast {
void Function::parseFunctionBody(links::LinkedList<lex::Token *> &tokens,
                                 parse::Parser &parser) {
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto sym = *dynamic_cast<lex::OpSym *>(tokens.peek());

    // Check for a ':'
    if (sym.Sym == ':') {
      // make the next word a decorator.
      tokens.pop();
      auto *decor = dynamic_cast<lex::LObj *>(tokens.pop());
      if (decor == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            "Expected Identifier after ':'");
      this->decorator = decor->meta;

      if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
        throw err::Exception("Line: " +
                             std::to_string(tokens.peek()->lineCount));
      sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
      if (sym.Sym == '.') {
        tokens.pop();
        auto *lob = dynamic_cast<lex::LObj *>(tokens.pop());
        if (lob == nullptr)
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              "Expected Identifier after '.'");
        this->decNSP = this->decorator;
        this->decorator = lob->meta;
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
          throw err::Exception("Line: " +
                               std::to_string(tokens.peek()->lineCount));
        sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
      };
      if (sym.Sym == '(') {
        tokens.pop();
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
            dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ')') {
          auto symp = dynamic_cast<lex::OpSym *>(tokens.pop());
          if (symp->Sym != ')')
            throw err::Exception(
                &"Expected closed parenthesis got "[symp->Sym]);
        } else {
          bool pop = false;
          do {
            if (pop) tokens.pop();
            this->decoratorArgs.push(parser.parseExpr(tokens));
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
      };
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            "Expected a symbol");
      sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
    }

    if (sym.Sym == '{') {
      tokens.pop();
      this->statement = parser.parseStmt(tokens);
      this->logicalLine = tokens.peek()->lineCount;
    } else {
      this->statement = nullptr;
      this->logicalLine = tokens.peek()->lineCount;
    }
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Need terminating symbol or open symbol");
}

Function::Function(const string &ident, const ScopeMod &scope, const Type &type,
                   const Op op, const std::string &scopeName,
                   links::LinkedList<lex::Token *> &tokens,
                   parse::Parser &parser, bool optional, bool safe)
    : scope(scope),
      type(type),
      op(op),
      scopeName(scopeName),
      optional(optional),
      safe(safe) {
  this->ident.ident = ident;
  this->useType = type;
  this->args = parser.parseArgs(tokens, ',', ')', this->argTypes, this->req,
                                this->mutability, this->readOnly,
                                this->optConvertionIndices);

  parseFunctionBody(tokens, parser);
}

Function::Function(const ScopeMod &scope,
                   links::LinkedList<lex::Token *> &tokens,
                   std::vector<std::string> genericTypes, parse::Parser &parser,
                   bool safe)
    : scope(scope), genericTypes(genericTypes), safe(safe) {
  // updated function syntax
  // func <ident>(<args>) -> <type> { <body> }
  const auto ident = dynamic_cast<lex::LObj *>(tokens.pop());
  if (ident == nullptr)
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Expected Identifier in function declaration");

  this->ident.ident = ident->meta;
  this->logicalLine = tokens.peek()->lineCount;

  auto openBracket = dynamic_cast<lex::OpSym *>(tokens.pop());
  if (openBracket == nullptr || openBracket->Sym != '(')
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         "Expected '('");
  this->args = parser.parseArgs(tokens, ',', ')', this->argTypes, this->req,
                                this->mutability, this->readOnly,
                                this->optConvertionIndices);

  auto dash = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (dash && dash->Sym == '-') {
    tokens.pop();
    auto arrow = dynamic_cast<lex::Symbol *>(tokens.peek());
    if (arrow == nullptr || arrow->meta != ">")
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           "Expected '->'");
    tokens.pop();
    auto typeName = dynamic_cast<lex::LObj *>(tokens.pop());
    if (typeName == nullptr)
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Expected Identifier");
    auto type = parser.typeList[typeName->meta];
    if (type == nullptr)
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Type " + typeName->meta + "not found");

    auto templateTypeList =
        parser.parseTemplateTypeList(tokens, tokens.peek()->lineCount);
    auto typenameStr = type->typeName;
    for (auto &genericType : templateTypeList) {
      typenameStr = typenameStr + "." + genericType;
    }

    this->type = Type(typenameStr, type->size);
    this->type.opType = type->opType;
    this->type.isGeneric = type->isGeneric;
    this->type.safeType = type->safeType;
    this->type.pedantic = type->pedantic;
    this->type.typeHint = type->typeHint;
  } else {
    this->type = *parser.typeList["void"];
    this->autoType = true;
  }
  this->useType = this->type;

  auto optional = dynamic_cast<lex::Ref *>(tokens.peek());
  if (optional != nullptr) {
    this->optional = true;
    tokens.pop();
  }

  parseFunctionBody(tokens, parser);
}

gen::GenerationResult const Function::generate(gen::CodeGenerator &generator) {
  // if the function is generic, do not generate code for it. It will be
  // generated when it is called with specific types.
  if (this->genericTypes.size() > 0) {
    generator.genericFunctions << *this;
    return {asmc::File(), std::nullopt};
  };

  bool hidden = false;
  asmc::File file;
  ast::Function *saveFunc = generator.currentFunction;
  int saveIntArgs = generator.intArgsCounter;
  bool isLambda = this->isLambda;

  if (generator.scope == nullptr || this->globalLocked) {
    if (!this->isLambda) generator.nameTable << *this;
  } else {
    if (!this->isLambda) this->scopeName = generator.scope->Ident;
    generator.scope->nameTable << *this;
    if (this->op != ast::None)
      if (!this->isLambda) this->scopeName = generator.scope->Ident;
    generator.scope->overloadTable << *this;
    if (this->scope == ast::Public)
      if (!this->isLambda) generator.scope->publicNameTable << *this;
  }

  if (this->statement != nullptr && !this->hidden) {
    gen::scope::ScopeManager::getInstance()->pushScope(true);
    generator.currentFunction = this;
    bool saveIn = generator.inFunction;
    generator.inFunction = true;
    gen::Class *saveScope = generator.scope;
    bool saveGlobal = generator.globalScope;
    generator.globalScope = false;

    auto label = new asmc::Label;
    label->logicalLine = this->logicalLine;
    if (generator.scope == nullptr || this->isLambda || this->globalLocked)
      label->label = this->ident.ident;
    else
      label->label = "pub_" + generator.scope->Ident + "_" + this->ident.ident;
    if (this->scopeName != "global") {
      label->label = "pub_" + this->scopeName + "_" + this->ident.ident;
      gen::Type *tScope = *generator.typeList[this->scopeName];
      if (tScope == nullptr) generator.alert("Failed to locate function Scope");
      if (dynamic_cast<gen::Class *>(tScope) == nullptr)
        generator.alert("Can only scope to  a class");
      generator.scope = dynamic_cast<gen::Class *>(tScope);
      if (generator.scope->hidden) {
        hidden = true;
      }
    }

    asmc::Push *push = new asmc::Push();
    push->logicalLine = this->logicalLine;
    push->op = "%rbp";
    asmc::Mov *mov = new asmc::Mov();
    mov->logicalLine = this->logicalLine;
    mov->size = asmc::QWord;
    mov->from = "%rsp";
    mov->to = "%rbp";

    if (this->scope == ast::Export) {
      auto link = new asmc::LinkTask();
      link->logicalLine = this->logicalLine;
      link->command = "global";
      link->operand = generator.moduleId + '.' + label->label;
      file.linker.push(link);
      auto label2 = new asmc::Label();
      label2->label = generator.moduleId + '.' + label->label;
      file.text << label2;
    }

    file.text.push(label);
    file.text.push(push);
    file.text.push(mov);
    // push the callee preserved registers
    auto push2 = new asmc::Push();
    push2->logicalLine = this->logicalLine;
    push2->op = "%rbx";
    file.text.push(push2);
    auto push3 = new asmc::Push();
    push3->logicalLine = this->logicalLine;

    int AlignmentLoc = file.text.count;
    generator.intArgsCounter = 0;

    this->useType = this->type;

    if (generator.scope != nullptr && this->type.typeName == "Self") {
      this->useType = Type(generator.scope->Ident, asmc::QWord);
      // needs to change useType in the class nameTables
      auto cl = generator.typeList[this->scopeName];
      if (cl != nullptr) {
        auto c = dynamic_cast<gen::Class *>(*cl);
        if (c != nullptr) {
          auto privateFunc = c->nameTable[this->ident.ident];
          if (privateFunc != nullptr) {
            privateFunc->useType = this->useType;
          }
          auto publicFunc = c->publicNameTable[this->ident.ident];
          if (publicFunc != nullptr) {
            publicFunc->useType = this->useType;
          }
        }
      }
    }

    generator.returnType = this->useType;

    auto link = new asmc::LinkTask();
    link->logicalLine = this->logicalLine;
    link->command = "global";
    link->operand = label->label;

    if (generator.scope != nullptr && !this->isLambda && !this->globalLocked) {
      // add the opo to the arguments of the function
      auto movy = new asmc::Mov();
      movy->logicalLine = this->logicalLine;
      movy->from = generator.intArgs[generator.intArgsCounter].get(asmc::QWord);

      auto ty = ast::Type();
      ty.typeName = generator.scope->Ident;
      ty.size = asmc::QWord;

      int byteMod = gen::scope::ScopeManager::getInstance()->assign(
          "my", ty, false, false, this->safe);

      movy->size = asmc::QWord;
      movy->to = "-" + std::to_string(byteMod) + +"(%rbp)";
      file.text << movy;
      generator.intArgsCounter++;
    };
    int counter = 0;
    auto argmute = generator.GenArgs(this->args, file, *this, counter);
    if (!isLambda && this->scope == ast::Public && !hidden)
      file.linker.push(link);

    file << argmute;

    // if the function is 'init' and scope is a class, add the default value
    if (this->ident.ident == "init" && generator.scope != nullptr &&
        !globalLocked) {
      // add all of the default values from the scopes list
      for (ast::DecAssign it : generator.scope->defaultValues) {
        ast::Assign assign = ast::Assign();
        assign.Ident = ("my");
        assign.override = true;
        assign.expr = it.expr;
        assign.modList = LinkedList<std::string>();
        assign.modList.push(it.declare->ident);
        file << generator.GenSTMT(&assign);
      }
    }

    asmc::File statement = generator.GenSTMT(this->statement);
    // check if the last statement is a return statement
    if (statement.text.count > 0) {
      if (!generator.currentFunction->has_return) {
        // if the function name is init then we need to alert to return
        // 'my'
        if (this->ident.ident == "init") {
          auto returnStmt = new ast::Return();
          returnStmt->logicalLine = this->logicalLine;
          auto var = new ast::Var();
          var->Ident = "my";
          returnStmt->expr = var;
          statement << generator.GenSTMT(returnStmt);
        } else {
          asmc::Return *ret = new asmc::Return();
          ret->logicalLine = this->logicalLine;
          statement.text.push(ret);
        };
      }
    } else {
      auto pop = new asmc::Pop();
      pop->logicalLine = this->logicalLine;
      pop->op = "%rbx";
      statement.text.push(pop);
      auto ret = new asmc::Return();
      ret->logicalLine = this->logicalLine;
      statement.text.push(ret);
    }
    file << statement;

    auto sub = new asmc::Subq;
    sub->logicalLine = this->logicalLine;
    sub->op1 =
        "$" + std::to_string(
                  gen::scope::ScopeManager::getInstance()->getStackAlignment());
    sub->op2 = generator.registers["%rsp"]->get(asmc::QWord);
    file.text.insert(sub, AlignmentLoc + 1);

    generator.scope = saveScope;
    generator.globalScope = saveGlobal;
    generator.inFunction = saveIn;
    gen::scope::ScopeManager::getInstance()->popScope(&generator, file, true);
  }

  generator.intArgsCounter = saveIntArgs;
  generator.currentFunction = saveFunc;
  return {file, std::nullopt};
};

gen::Expr Function::toExpr(gen::CodeGenerator &generator) {
  gen::Expr output;
  auto tn = useType.typeName != "" ? useType.typeName : type.typeName;
  if (generator.scope != nullptr && tn == "Self") {
    tn = generator.scope->Ident;
  }
  output.type = this->optional ? "Option" : tn;
  output.size = this->optional ? asmc::QWord : this->type.size;
  output.access = generator.registers["%rax"]->get(output.size);
  if (this->type.typeName == "float") {
    output.access = generator.registers["%xmm0"]->get(output.size);
    output.op = asmc::Float;
  }
  return output;
};
}  // namespace ast
