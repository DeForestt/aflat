#include "Parser/AST/Statements/Function.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"

namespace ast {
Function::Function(const string &ident, const ScopeMod &scope, const Type &type,
                   const Op op, const std::string &scopeName,
                   links::LinkedList<lex::Token *> &tokens,
                   parse::Parser &parser)
    : scope(scope), type(type), op(op), scopeName(scopeName) {
  this->ident.ident = ident;
  this->args = parser.parseArgs(tokens, ',', ')', this->argTypes, this->req);

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
            throw err::Exception("Expected closed parenthesis got " +
                                 symp->Sym);
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
              throw err::Exception("Expected closed parenthesis got " +
                                   symp->Sym);
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

gen::GenerationResult const Function::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  ast::Function *saveFunc = generator.currentFunction;
  int saveIntArgs = generator.intArgsCounter;
  bool isLambda = this->isLambda;

  if (generator.scope == nullptr) {
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

  if (this->statement != nullptr) {
    gen::scope::ScopeManager::getInstance()->pushScope(true);
    generator.currentFunction = this;
    bool saveIn = generator.inFunction;
    generator.inFunction = true;
    gen::Class *saveScope = generator.scope;
    bool saveGlobal = generator.globalScope;
    generator.globalScope = false;

    auto label = new asmc::Label;
    label->logicalLine = this->logicalLine;
    if (generator.scope == nullptr || this->isLambda)
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
    generator.returnType = this->type;
    auto link = new asmc::LinkTask();
    link->logicalLine = this->logicalLine;
    link->command = "global";
    link->operand = label->label;

    if (generator.scope != nullptr && !this->isLambda) {
      // add the opo to the arguments of the function
      gen::Symbol symbol;
      auto movy = new asmc::Mov();
      movy->logicalLine = this->logicalLine;
      movy->from = generator.intArgs[generator.intArgsCounter].get(asmc::QWord);

      symbol.symbol = "my";
      symbol.mutable_ = false;

      auto ty = ast::Type();
      ty.typeName = generator.scope->Ident;
      ty.size = asmc::QWord;
      symbol.type = ty;

      int byteMod =
          gen::scope::ScopeManager::getInstance()->assign("my", ty, false);

      movy->size = asmc::QWord;
      movy->to = "-" + std::to_string(byteMod) + +"(%rbp)";
      file.text << movy;
      generator.intArgsCounter++;
    };
    generator.GenArgs(this->args, file);
    if (!isLambda && this->scope == ast::Public) file.linker.push(link);

    // if the function is 'init' and scope is a class, add the default value
    if (this->ident.ident == "init" && generator.scope != nullptr) {
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
}  // namespace ast
