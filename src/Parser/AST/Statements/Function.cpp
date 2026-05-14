#include "Parser/AST/Statements/Function.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements/Assign.hpp"
#include "Parser/AST/Statements/Await.hpp"
#include "Parser/AST/Statements/Dec.hpp"
#include "Parser/AST/Statements/DecArr.hpp"
#include "Parser/AST/Statements/DecAssign.hpp"
#include "Parser/AST/Statements/DecAssignArr.hpp"
#include "Parser/AST/Statements/Declare.hpp"
#include "Parser/AST/Statements/Delete.hpp"
#include "Parser/AST/Statements/For.hpp"
#include "Parser/AST/Statements/ForEach.hpp"
#include "Parser/AST/Statements/If.hpp"
#include "Parser/AST/Statements/Match.hpp"
#include "Parser/AST/Statements/Pause.hpp"
#include "Parser/AST/Statements/Return.hpp"
#include "Parser/AST/Statements/Sequence.hpp"
#include "Parser/AST/Statements/While.hpp"
#include "Scanner.hpp"

namespace {
void collectYieldPointsFromExpr(ast::Expr *expr,
                                std::vector<ast::Statement *> &yieldPoints);

void collectYieldPoints(ast::Statement *statement,
                        std::vector<ast::Statement *> &yieldPoints) {
  if (statement == nullptr) {
    return;
  }
  if (auto *expr = dynamic_cast<ast::Expr *>(statement)) {
    collectYieldPointsFromExpr(expr, yieldPoints);
  }
  if (auto *sequence = dynamic_cast<ast::Sequence *>(statement)) {
    collectYieldPoints(sequence->Statement1, yieldPoints);
    collectYieldPoints(sequence->Statement2, yieldPoints);
    return;
  }
  if (dynamic_cast<ast::Pause *>(statement) != nullptr) {
    yieldPoints.push_back(statement);
  }

  if (auto *ret = dynamic_cast<ast::Return *>(statement)) {
    collectYieldPointsFromExpr(ret->expr, yieldPoints);
  } else if (auto *assign = dynamic_cast<ast::Assign *>(statement)) {
    collectYieldPointsFromExpr(assign->expr, yieldPoints);
  } else if (auto *decAssign = dynamic_cast<ast::DecAssign *>(statement)) {
    collectYieldPointsFromExpr(decAssign->expr, yieldPoints);
  } else if (auto *decAssignArr =
                 dynamic_cast<ast::DecAssignArr *>(statement)) {
    collectYieldPointsFromExpr(decAssignArr->expr, yieldPoints);
  } else if (auto *push = dynamic_cast<ast::Push *>(statement)) {
    collectYieldPointsFromExpr(push->expr, yieldPoints);
  } else if (auto *pull = dynamic_cast<ast::Pull *>(statement)) {
    collectYieldPointsFromExpr(pull->expr, yieldPoints);
  } else if (auto *cwrite = dynamic_cast<ast::CWrite *>(statement)) {
    collectYieldPointsFromExpr(cwrite->expr, yieldPoints);
  } else if (auto *dec = dynamic_cast<ast::Dec *>(statement)) {
    (void)dec;
  } else if (auto *decArr = dynamic_cast<ast::DecArr *>(statement)) {
    (void)decArr;
  } else if (auto *deleter = dynamic_cast<ast::Delete *>(statement)) {
    (void)deleter;
  } else if (auto *ifStmt = dynamic_cast<ast::If *>(statement)) {
    collectYieldPointsFromExpr(ifStmt->expr, yieldPoints);
    collectYieldPoints(ifStmt->statement, yieldPoints);
    collectYieldPoints(ifStmt->elseStatement, yieldPoints);
  } else if (auto *whileStmt = dynamic_cast<ast::While *>(statement)) {
    collectYieldPointsFromExpr(whileStmt->expr, yieldPoints);
    collectYieldPoints(whileStmt->stmt, yieldPoints);
  } else if (auto *forStmt = dynamic_cast<ast::For *>(statement)) {
    collectYieldPoints(forStmt->declare, yieldPoints);
    collectYieldPointsFromExpr(forStmt->expr, yieldPoints);
    collectYieldPoints(forStmt->increment, yieldPoints);
    collectYieldPoints(forStmt->Run, yieldPoints);
  } else if (auto *forEachStmt = dynamic_cast<ast::ForEach *>(statement)) {
    collectYieldPointsFromExpr(forEachStmt->iterator, yieldPoints);
    collectYieldPoints(forEachStmt->implementation, yieldPoints);
  } else if (auto *match = dynamic_cast<ast::Match *>(statement)) {
    collectYieldPointsFromExpr(match->expr, yieldPoints);
    for (const auto &matchCase : match->cases) {
      collectYieldPoints(matchCase.statement, yieldPoints);
    }
  } else if (auto *pause = dynamic_cast<ast::Pause *>(statement)) {
    collectYieldPoints(pause->body, yieldPoints);
  }
}

void collectYieldPointsFromExpr(ast::Expr *expr,
                                std::vector<ast::Statement *> &yieldPoints) {
  if (expr == nullptr) {
    return;
  }
  if (dynamic_cast<ast::Await *>(expr) != nullptr) {
    yieldPoints.push_back(expr);
    return;
  }
  if (auto *match = dynamic_cast<ast::Match *>(expr)) {
    collectYieldPointsFromExpr(match->expr, yieldPoints);
    for (const auto &matchCase : match->cases) {
      collectYieldPoints(matchCase.statement, yieldPoints);
    }
    return;
  }
  if (auto *paren = dynamic_cast<ast::parenExpr *>(expr)) {
    collectYieldPointsFromExpr(paren->expr, yieldPoints);
  } else if (auto *buy = dynamic_cast<ast::Buy *>(expr)) {
    collectYieldPointsFromExpr(buy->expr, yieldPoints);
  } else if (auto *notExpr = dynamic_cast<ast::Not *>(expr)) {
    collectYieldPointsFromExpr(notExpr->expr, yieldPoints);
  } else if (auto *callExpr = dynamic_cast<ast::CallExpr *>(expr)) {
    for (auto arg : callExpr->call->Args) {
      collectYieldPointsFromExpr(arg, yieldPoints);
    }
  } else if (auto *compound = dynamic_cast<ast::Compound *>(expr)) {
    collectYieldPointsFromExpr(compound->expr1, yieldPoints);
    collectYieldPointsFromExpr(compound->expr2, yieldPoints);
  } else if (auto *ifExpr = dynamic_cast<ast::IfExpr *>(expr)) {
    collectYieldPointsFromExpr(ifExpr->expr, yieldPoints);
    collectYieldPointsFromExpr(ifExpr->trueExpr, yieldPoints);
    collectYieldPointsFromExpr(ifExpr->falseExpr, yieldPoints);
  } else if (auto *newExpr = dynamic_cast<ast::NewExpr *>(expr)) {
    for (auto arg : newExpr->args) {
      collectYieldPointsFromExpr(arg, yieldPoints);
    }
  } else if (auto *structList = dynamic_cast<ast::StructList *>(expr)) {
    for (auto arg : structList->args) {
      collectYieldPointsFromExpr(arg, yieldPoints);
    }
  } else if (auto *fstring = dynamic_cast<ast::FStringLiteral *>(expr)) {
    for (auto arg : fstring->args) {
      collectYieldPointsFromExpr(arg, yieldPoints);
    }
  } else if (auto *unionConstructor =
                 dynamic_cast<ast::UnionConstructor *>(expr)) {
    collectYieldPointsFromExpr(unionConstructor->expr, yieldPoints);
  }
}

ast::Declare *firstDeclare(ast::Statement *statement) {
  if (statement == nullptr) {
    return nullptr;
  }
  if (auto *declare = dynamic_cast<ast::Declare *>(statement)) {
    return declare;
  }
  if (auto *sequence = dynamic_cast<ast::Sequence *>(statement)) {
    if (auto *first = firstDeclare(sequence->Statement1)) {
      return first;
    }
    return firstDeclare(sequence->Statement2);
  }
  return nullptr;
}
} // namespace

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
            if (pop)
              tokens.pop();
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
                   parse::Parser &parser, bool optional, bool safe,
                   bool asyncFunction)
    : scope(scope), type(type), op(op), scopeName(scopeName),
      optional(optional), safe(safe), asyncFunction(asyncFunction) {
  this->ident.ident = ident;
  this->useType = type;
  this->args = parser.parseArgs(tokens, ',', ')', this->argTypes, this->req,
                                this->mutability, this->optConvertionIndices,
                                this->readOnly);

  parseFunctionBody(tokens, parser);
}

Function::Function(const ScopeMod &scope,
                   links::LinkedList<lex::Token *> &tokens,
                   std::vector<std::string> genericTypes, parse::Parser &parser,
                   bool safe, bool asyncFunction)
    : scope(scope), genericTypes(genericTypes), safe(safe),
      asyncFunction(asyncFunction) {
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
                                this->mutability, this->optConvertionIndices,
                                this->readOnly);

  auto arrow = dynamic_cast<lex::Symbol *>(tokens.peek());
  if (arrow && arrow->meta == "->") {
    tokens.pop();

    while (true) {
      if (auto modifier = dynamic_cast<lex::LObj *>(tokens.peek())) {
        if (modifier->meta == "immutable") {
          this->returnImmutable = true;
          tokens.pop();
          continue;
        }
        if (modifier->meta == "loan") {
          this->returnLowOwnership = true;
          tokens.pop();
          continue;
        }
      }
      break;
    }

    auto typeName = dynamic_cast<lex::LObj *>(tokens.pop());
    if (typeName == nullptr)
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Expected Identifier");
    auto type = parser.getTypeList()[typeName->meta];
    if (type == nullptr)
      type = new Type(typeName->meta, asmc::QWord);
    auto templateTypeList =
        parser.parseTemplateTypeList(tokens, tokens.peek()->lineCount);

    auto typenameStr = type->typeName;
    if (!templateTypeList.empty()) {
      typenameStr += "<";
      for (size_t i = 0; i < templateTypeList.size(); ++i) {
        typenameStr += templateTypeList[i];
        if (i != templateTypeList.size() - 1) {
          typenameStr += ", ";
        }
      }
      typenameStr += ">";
    };

    this->type = Type(typenameStr, type->size);
    this->type.opType = type->opType;
    this->type.isGeneric = type->isGeneric;
    this->type.safeType = type->safeType;
    this->type.pedantic = type->pedantic;
    this->type.typeHint = type->typeHint;
  } else {
    this->type = *parser.getTypeList()["void"];
    this->autoType = true;
  }
  this->useType = this->type;

  if (this->asyncFunction) {
    if (this->argTypes.empty()) {
      throw err::Exception("Line: " + std::to_string(this->logicalLine) +
                           " async functions must take a task<T> job as the "
                           "first argument");
    }
    const std::string expectedTaskType = "task<" + this->type.typeName + ">";
    if (this->argTypes.front().typeName != expectedTaskType) {
      throw err::Exception("Line: " + std::to_string(this->logicalLine) +
                           " async functions must take " + expectedTaskType +
                           " as the first argument");
    }
  }

  auto optional = dynamic_cast<lex::Ref *>(tokens.peek());
  if (optional != nullptr) {
    this->optional = true;
    tokens.pop();
  } else if (auto opSym = dynamic_cast<lex::OpSym *>(tokens.peek())) {
    if (opSym->Sym == '!') {
      this->error = true;
      tokens.pop();
    }
  }

  parseFunctionBody(tokens, parser);
}

gen::GenerationResult const Function::generate(gen::CodeGenerator &generator) {
  // if the function is generic, do not generate code for it. It will be
  // generated when it is called with specific types.
  if (this->genericTypes.size() > 0) {
    generator.genericFunctions() << *this;
    return {asmc::File(), std::nullopt};
  }

  bool hidden = false;
  asmc::File file;
  ast::Function *saveFunc = generator.currentFunction();
  int saveIntArgs = generator.intArgsCounter();
  bool isLambda = this->isLambda;

  auto assignOverloadSuffix =
      [&](links::SLinkedList<ast::Function, std::string> &table,
          bool requireGlobalScope) {
        if (this->isLambda)
          return;
        if (auto *firstInstance = table[this->ident.ident]) {
          bool forwardDeclaration = (firstInstance->statement == nullptr &&
                                     this->statement != nullptr);
          if (!forwardDeclaration && !firstInstance->wasGeneric &&
              (!requireGlobalScope || this->scopeName == "global")) {
            this->overloadIndex = firstInstance->overloadIndex + 1;
            this->ident.ident += "_ovl" + std::to_string(this->overloadIndex);
          }
        }
      };

  if (generator.scope() == nullptr || this->globalLocked) {
    assignOverloadSuffix(generator.nameTable(), true);
    if (!this->isLambda) {
      generator.nameTable() << *this;
    }
  } else {
    if (!this->isLambda)
      this->scopeName = generator.scope()->Ident;
    assignOverloadSuffix(generator.scope()->nameTable, false);
    generator.scope()->nameTable << *this;
    if (this->op != ast::None)
      if (!this->isLambda)
        this->scopeName = generator.scope()->Ident;
    generator.scope()->overloadTable << *this;
    if (this->scope == ast::Public)
      if (!this->isLambda)
        generator.scope()->publicNameTable << *this;
  }

  if (this->statement != nullptr && !this->hidden) {
    gen::scope::ScopeManager::getInstance()->pushScope(true);
    generator.currentFunction() = this;
    bool saveIn = generator.inFunction();
    generator.inFunction() = true;
    gen::Class *saveScope = generator.scope();
    bool saveGlobal = generator.globalScope();
    generator.globalScope() = false;

    auto label = new asmc::Label;
    label->logicalLine = this->logicalLine;
    if (generator.scope() == nullptr || this->isLambda || this->globalLocked)
      label->label = this->ident.ident;
    else
      label->label =
          "pub_" + generator.scope()->Ident + "_" + this->ident.ident;
    if (this->scopeName != "global") {
      label->label = "pub_" + this->scopeName + "_" + this->ident.ident;
      gen::Type *tScope = *generator.typeList()[this->scopeName];
      if (tScope == nullptr)
        generator.alert("Failed to locate function Scope");
      if (dynamic_cast<gen::Class *>(tScope) == nullptr)
        generator.alert("Can only scope to  a class");
      generator.scope() = dynamic_cast<gen::Class *>(tScope);
      if (generator.scope()->hidden) {
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
      link->operand = generator.moduleId() + '.' + label->label;
      file.linker.push(link);
      auto label2 = new asmc::Label();
      label2->label = generator.moduleId() + '.' + label->label;
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
    generator.intArgsCounter() = 0;

    this->useType = this->type;

    if (generator.scope() != nullptr && this->type.typeName == "Self") {
      this->useType = Type(generator.scope()->Ident, asmc::QWord);
      // needs to change useType in the class nameTables
      auto cl = generator.typeList()[this->scopeName];
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

    generator.returnType() = this->useType;

    auto link = new asmc::LinkTask();
    link->logicalLine = this->logicalLine;
    link->command = "global";
    link->operand = label->label;

    if (this->asyncFunction) {
      auto *firstArg = firstDeclare(this->args);
      generator.coroutineTaskIdent() =
          firstArg != nullptr ? firstArg->ident : "job";
    }

    if (this->asyncFunction) {
      auto moveTask = new asmc::Mov();
      moveTask->logicalLine = this->logicalLine;
      moveTask->size = asmc::QWord;
      moveTask->from = "%rdi";
      moveTask->to = "%rbx";
      file.text.push(moveTask);

      auto loadFrame = new asmc::Mov();
      loadFrame->logicalLine = this->logicalLine;
      loadFrame->size = asmc::QWord;
      loadFrame->from = "0(%rbx)";
      loadFrame->to = "%r10";
      file.text.push(loadFrame);

      auto checkFrame = new asmc::Cmp();
      checkFrame->logicalLine = this->logicalLine;
      checkFrame->size = asmc::QWord;
      checkFrame->from = "$0";
      checkFrame->to = "%r10";
      file.text.push(checkFrame);

      auto haveFrame = new asmc::Je();
      haveFrame->logicalLine = this->logicalLine;
      haveFrame->to = ".L" + generator.moduleId() + "_" + this->ident.ident +
                      "_allocate_frame";
      file.text.push(haveFrame);

      auto existingFrame = new asmc::Lea();
      existingFrame->logicalLine = this->logicalLine;
      existingFrame->from = "65536(%r10)";
      existingFrame->to = "%r10";
      file.text.push(existingFrame);

      auto readyJump = new asmc::Jmp();
      readyJump->logicalLine = this->logicalLine;
      readyJump->to = ".L" + generator.moduleId() + "_" + this->ident.ident +
                      "_frame_ready";
      file.text.push(readyJump);

      auto allocateLabel = new asmc::Label();
      allocateLabel->logicalLine = this->logicalLine;
      allocateLabel->label = ".L" + generator.moduleId() + "_" +
                             this->ident.ident + "_allocate_frame";
      file.text.push(allocateLabel);

      auto allocSize = new asmc::Mov();
      allocSize->logicalLine = this->logicalLine;
      allocSize->size = asmc::QWord;
      allocSize->from = "$65536";
      allocSize->to = "%rdi";
      file.text.push(allocSize);

      auto callMalloc = new asmc::Call();
      callMalloc->logicalLine = this->logicalLine;
      callMalloc->function = "af_malloc";
      file.text.push(callMalloc);

      auto storeFrame = new asmc::Mov();
      storeFrame->logicalLine = this->logicalLine;
      storeFrame->size = asmc::QWord;
      storeFrame->from = "%rax";
      storeFrame->to = "0(%rbx)";
      file.text.push(storeFrame);

      auto storeFrameSize = new asmc::Mov();
      storeFrameSize->logicalLine = this->logicalLine;
      storeFrameSize->size = asmc::QWord;
      storeFrameSize->from = "$65536";
      storeFrameSize->to = "8(%rbx)";
      file.text.push(storeFrameSize);

      auto moveFrame = new asmc::Lea();
      moveFrame->logicalLine = this->logicalLine;
      moveFrame->from = "65536(%rax)";
      moveFrame->to = "%r10";
      file.text.push(moveFrame);

      auto frameReady = new asmc::Label();
      frameReady->logicalLine = this->logicalLine;
      frameReady->label = ".L" + generator.moduleId() + "_" +
                          this->ident.ident + "_frame_ready";
      file.text.push(frameReady);

      auto restoreTaskArg = new asmc::Mov();
      restoreTaskArg->logicalLine = this->logicalLine;
      restoreTaskArg->size = asmc::QWord;
      restoreTaskArg->from = "%rbx";
      restoreTaskArg->to = "%rdi";
      file.text.push(restoreTaskArg);
    }

    if (generator.scope() != nullptr && !this->isLambda &&
        !this->globalLocked) {
      // add the opo to the arguments of the function
      auto movy = new asmc::Mov();
      movy->logicalLine = this->logicalLine;
      movy->from =
          generator.intArgs()[generator.intArgsCounter()].get(asmc::QWord);

      auto ty = ast::Type();
      ty.typeName = generator.scope()->Ident;
      ty.size = asmc::QWord;

      int byteMod = gen::scope::ScopeManager::getInstance()->assign(
          "my", ty, false, false, this->safe);
      auto my = gen::scope::ScopeManager::getInstance()->get("my");
      my->owned = true;

      movy->size = asmc::QWord;
      movy->to =
          "-" + std::to_string(byteMod) + "(" + generator.frameBase() + ")";
      file.text << movy;
      generator.intArgsCounter()++;
    };
    if (!isLambda && this->scope == ast::Public && !hidden)
      file.linker.push(link);

    int counter = 0;
    auto argmute = generator.GenArgs(this->args, file, *this, counter);

    if (this->asyncFunction) {
      std::vector<ast::Statement *> yieldPoints;
      collectYieldPoints(this->statement, yieldPoints);

      const auto saveCoroutineActive = generator.coroutineActive();
      const auto saveCoroutineTaskIdent = generator.coroutineTaskIdent();
      const auto saveCoroutineEndLabel = generator.coroutineEndLabel();
      const auto saveCoroutineResumeLabels = generator.coroutineResumeLabels();
      const auto saveCoroutineStateIndex = generator.coroutineStateIndex();

      generator.coroutineActive() = true;
      generator.coroutineStateIndex() = 0;
      generator.coroutineResumeLabels().clear();
      generator.coroutineEndLabel() =
          ".L" + generator.moduleId() + "_" + this->ident.ident + "_end";

      generator.coroutineResumeLabels().push_back(
          ".L" + generator.moduleId() + "_" + this->ident.ident + "_state_0");
      for (size_t i = 1; i <= yieldPoints.size(); ++i) {
        generator.coroutineResumeLabels().push_back(
            ".L" + generator.moduleId() + "_" + this->ident.ident + "_state_" +
            std::to_string(i));
      }

      if (this->ident.ident == "init" && generator.scope() != nullptr &&
          !globalLocked) {
        for (ast::DecAssign it : generator.scope()->defaultValues) {
          ast::Assign assign = ast::Assign();
          assign.Ident = ("my");
          assign.override = true;
          assign.expr = it.expr;
          assign.modList = LinkedList<std::string>();
          assign.modList.push(it.declare->ident);
          file << generator.GenSTMT(&assign);
        }
      }

      if (!yieldPoints.empty()) {
        asmc::File dispatcher;
        ast::Var *stateVar = new ast::Var();
        stateVar->Ident = generator.coroutineTaskIdent();
        stateVar->modList.push("state");
        stateVar->logicalLine = this->logicalLine;
        gen::Expr stateExpr = generator.GenExpr(stateVar, dispatcher);

        for (size_t i = 1; i < generator.coroutineResumeLabels().size(); ++i) {
          auto cmp = new asmc::Cmp();
          cmp->logicalLine = this->logicalLine;
          cmp->from = "$" + std::to_string(i);
          cmp->to = stateExpr.access;
          cmp->size = stateExpr.size;
          dispatcher.text << cmp;
          auto je = new asmc::Je();
          je->logicalLine = this->logicalLine;
          je->to = generator.coroutineResumeLabels()[i];
          dispatcher.text << je;
        }
        file << dispatcher;
      }

      asmc::Label *entryResume = new asmc::Label();
      entryResume->logicalLine = this->logicalLine;
      entryResume->label = generator.coroutineResumeLabels().front();
      file.text << entryResume;

      file << argmute;

      asmc::File statement = generator.GenSTMT(this->statement);

      file << statement;

      auto endLabel = new asmc::Label();
      endLabel->logicalLine = this->logicalLine;
      endLabel->label = generator.coroutineEndLabel();
      file.text << endLabel;

      generator.coroutineActive() = saveCoroutineActive;
      generator.coroutineTaskIdent() = saveCoroutineTaskIdent;
      generator.coroutineEndLabel() = saveCoroutineEndLabel;
      generator.coroutineResumeLabels() = saveCoroutineResumeLabels;
      generator.coroutineStateIndex() = saveCoroutineStateIndex;

      generator.scope() = saveScope;
      generator.globalScope() = saveGlobal;
      generator.inFunction() = saveIn;
      gen::scope::ScopeManager::getInstance()->popScope(&generator, file, true);

      generator.intArgsCounter() = saveIntArgs;
      generator.currentFunction() = saveFunc;
      return {file, std::nullopt};
    }

    // if the function is 'init' and scope is a class, add the default value
    if (this->ident.ident == "init" && generator.scope() != nullptr &&
        !globalLocked) {
      // add all of the default values from the scopes list
      for (ast::DecAssign it : generator.scope()->defaultValues) {
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
      if (!generator.currentFunction()->has_return) {
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
    sub->op2 = generator.registers()["%rsp"]->get(asmc::QWord);
    file.text.insert(sub, AlignmentLoc + 1);

    generator.scope() = saveScope;
    generator.globalScope() = saveGlobal;
    generator.inFunction() = saveIn;
    gen::scope::ScopeManager::getInstance()->popScope(&generator, file, true);
  }

  generator.intArgsCounter() = saveIntArgs;
  generator.currentFunction() = saveFunc;
  return {file, std::nullopt};
};

gen::Expr Function::toExpr(gen::CodeGenerator &generator) {
  gen::Expr output;
  auto tn = useType.typeName != "" ? useType.typeName : type.typeName;
  if (generator.scope() != nullptr && tn == "Self") {
    tn = generator.scope()->Ident;
  }
  output.type = this->optional ? "option<" + tn + ">"
                : this->error  ? "result<" + tn + ">"
                               : tn;
  output.size = this->optional || this->error ? asmc::QWord : this->type.size;
  output.access = generator.registers()["%rax"]->get(output.size);
  if (this->type.typeName == "float") {
    output.access = generator.registers()["%xmm0"]->get(output.size);
    output.op = asmc::Float;
  }
  output.owned = output.type != "void" && !this->returnLowOwnership;
  output.requiresImmutableBinding = this->returnImmutable;
  if (this->returnImmutable)
    output.immutableBindingSource = this->ident.ident;
  return output;
};
} // namespace ast
