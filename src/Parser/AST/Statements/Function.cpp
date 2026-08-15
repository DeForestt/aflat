#include "Parser/AST/Statements/Function.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST.hpp"
#include "Scanner.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <regex>

namespace ast {
namespace {

bool isConcreteGenericScope(const std::string &scopeName) {
  return scopeName.find('<') != std::string::npos;
}

std::string functionEmissionKey(std::string label) {
  label.erase(std::remove_if(label.begin(), label.end(),
                             [](unsigned char c) { return std::isspace(c); }),
              label.end());
  return label;
}

int maxFrameOffset(const asmc::File &file) {
  static const std::regex rbpOffsetPattern(R"(-([0-9]+)\(%rbp\))");
  int maxOffset = 0;
  for (auto *inst : file.text) {
    if (inst == nullptr) {
      continue;
    }
    const auto text = inst->toString();
    for (auto it =
             std::sregex_iterator(text.begin(), text.end(), rbpOffsetPattern);
         it != std::sregex_iterator(); ++it) {
      maxOffset = std::max(maxOffset, std::stoi((*it)[1].str()));
    }
  }
  return maxOffset;
}

int alignFrameSize(int bytes) { return ((bytes + 15) / 16) * 16; }

asmc::Mov *movq(const std::string &from, const std::string &to, int line) {
  auto *mov = new asmc::Mov();
  mov->from = from;
  mov->to = to;
  mov->size = asmc::QWord;
  mov->logicalLine = line;
  return mov;
}

asmc::File asyncConstructor(const Function &function,
                            const std::string &constructorLabel,
                            const std::string &bodyLabel, int capturedArgs) {
  asmc::File file;
  const int argc = capturedArgs;
  static const std::array<std::string, 6> regs = {"%rdi", "%rsi", "%rdx",
                                                  "%rcx", "%r8",  "%r9"};

  auto *global = new asmc::LinkTask();
  global->command = "global";
  global->operand = constructorLabel;
  file.linker << global;
  auto *label = new asmc::Label();
  label->label = constructorLabel;
  label->logicalLine = function.logicalLine;
  file.text << label;
  auto *push = new asmc::Push();
  push->op = "%rbp";
  push->logicalLine = function.logicalLine;
  file.text << push;
  file.text << movq("%rsp", "%rbp", function.logicalLine);
  auto *sub = new asmc::Subq();
  sub->op1 = "$64";
  sub->op2 = "%rsp";
  sub->logicalLine = function.logicalLine;
  file.text << sub;

  for (int i = 0; i < argc; ++i)
    file.text << movq(regs[i], "-" + std::to_string((i + 1) * 8) + "(%rbp)",
                      function.logicalLine);

  auto *bodyAddress = new asmc::Lea();
  bodyAddress->from = bodyLabel + "(%rip)";
  bodyAddress->to = "%rdi";
  bodyAddress->logicalLine = function.logicalLine;
  file.text << bodyAddress;
  for (int i = 0; i < std::min(argc, 5); ++i)
    file.text << movq("-" + std::to_string((i + 1) * 8) + "(%rbp)", regs[i + 1],
                      function.logicalLine);
  if (argc == 6) {
    file.text << movq("-48(%rbp)", "%rax", function.logicalLine);
    file.text << movq("%rax", "(%rsp)", function.logicalLine);
  }

  auto *call = new asmc::Call();
  call->function = "af_task_create" + std::to_string(argc);
  call->logicalLine = function.logicalLine;
  file.text << call;
  auto *ret = new asmc::Return();
  ret->logicalLine = function.logicalLine;
  file.text << ret;
  return file;
}

asmc::File asyncMainWrapper(const std::string &constructorLabel, int line) {
  asmc::File file;
  auto *global = new asmc::LinkTask();
  global->command = "global";
  global->operand = "main";
  file.linker << global;
  auto *label = new asmc::Label();
  label->label = "main";
  label->logicalLine = line;
  file.text << label;
  auto *push = new asmc::Push();
  push->op = "%rbp";
  file.text << push;
  file.text << movq("%rsp", "%rbp", line);
  auto *construct = new asmc::Call();
  construct->function = constructorLabel;
  file.text << construct;
  file.text << movq("%rax", "%rdi", line);
  auto *run = new asmc::Call();
  run->function = "af_task_run";
  file.text << run;
  auto *ret = new asmc::Return();
  file.text << ret;
  return file;
}

void replaceAsyncFrameSize(asmc::File &file, int frameSize) {
  const std::string placeholder = "$__AF_ASYNC_FRAME_SIZE__";
  const std::string replacement = "$" + std::to_string(frameSize);
  for (auto *instruction : file.text) {
    if (auto *move = dynamic_cast<asmc::Mov *>(instruction)) {
      if (move->from == placeholder)
        move->from = replacement;
      if (move->to == placeholder)
        move->to = replacement;
    }
  }
}

std::vector<asmc::Instruction *> asyncDispatch(const Function &function,
                                               int frameSize) {
  std::vector<asmc::Instruction *> instructions;
  instructions.push_back(movq("%rbp", "%rdi", function.logicalLine));
  instructions.push_back(
      movq("$" + std::to_string(frameSize), "%rsi", function.logicalLine));
  auto *restore = new asmc::Call();
  restore->function = "af_task_restore_frame";
  restore->logicalLine = function.logicalLine;
  instructions.push_back(restore);
  for (int state = 1; state <= function.asyncStateCounter; ++state) {
    auto *compare = new asmc::Cmp();
    compare->from = "$" + std::to_string(state);
    compare->to = "%eax";
    compare->size = asmc::DWord;
    compare->logicalLine = function.logicalLine;
    instructions.push_back(compare);
    auto *jump = new asmc::Je();
    jump->to = function.asyncStateLabel(state);
    jump->logicalLine = function.logicalLine;
    instructions.push_back(jump);
  }
  auto *validStart = new asmc::Cmp();
  validStart->from = "$0";
  validStart->to = "%eax";
  validStart->size = asmc::DWord;
  validStart->logicalLine = function.logicalLine;
  instructions.push_back(validStart);
  auto *startJump = new asmc::Je();
  startJump->to = function.asyncStartLabel();
  startJump->logicalLine = function.logicalLine;
  instructions.push_back(startJump);
  instructions.push_back(movq("%rax", "%rdi", function.logicalLine));
  auto *invalid = new asmc::Call();
  invalid->function = "af_task_invalid_state";
  invalid->logicalLine = function.logicalLine;
  instructions.push_back(invalid);
  auto *invalidReturn = new asmc::Return();
  invalidReturn->logicalLine = function.logicalLine;
  instructions.push_back(invalidReturn);
  auto *start = new asmc::Label();
  start->label = function.asyncStartLabel();
  start->logicalLine = function.logicalLine;
  instructions.push_back(start);
  return instructions;
}

} // namespace

void resetSharedGenericFunctionEmissions() {}

void Function::parseFunctionBody(links::LinkedList<lex::Token *> &tokens,
                                 parse::Parser &parser) {
  const int declarationLine = this->logicalLine > 0 ? this->logicalLine : 1;
  if (tokens.head == nullptr) {
    throw err::Exception(
        "Line: " + std::to_string(declarationLine) +
        " Incomplete function declaration: expected `{` to begin the body "
        "or `;` for a declaration");
  }
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto sym = *dynamic_cast<lex::OpSym *>(tokens.peek());

    // Check for a ':'
    if (sym.Sym == ':') {
      // make the next word a decorator.
      tokens.pop();
      auto *decor = dynamic_cast<lex::LObj *>(tokens.pop());
      if (decor == nullptr)
        throw err::Exception("Line: " + std::to_string(declarationLine) +
                             " Expected Identifier after ':'");
      this->decorator = decor->meta;
      this->decoratorTemplateTypes =
          parser.parseTemplateTypeList(tokens, decor->lineCount);

      auto nextSym = dynamic_cast<lex::OpSym *>(tokens.peek());
      if (nextSym == nullptr)
        throw err::Exception("Line: " + std::to_string(declarationLine) +
                             " Expected a function body after decorator");
      sym = *nextSym;
      if (sym.Sym == '.') {
        tokens.pop();
        auto *lob = dynamic_cast<lex::LObj *>(tokens.pop());
        if (lob == nullptr)
          throw err::Exception("Line: " + std::to_string(declarationLine) +
                               " Expected Identifier after '.'");
        this->decNSP = this->decorator;
        this->decorator = lob->meta;
        this->decoratorTemplateTypes =
            parser.parseTemplateTypeList(tokens, lob->lineCount);

        if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
          throw err::Exception("Line: " + std::to_string(declarationLine) +
                               " Expected a function body after decorator");
        sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
      };
      if (sym.Sym == '(') {
        tokens.pop();
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
            dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ')') {
          auto symp = dynamic_cast<lex::OpSym *>(tokens.pop());
          if (symp->Sym != ')')
            throw err::Exception("Expected closed parenthesis got " +
                                 std::string(1, symp->Sym));
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
              throw err::Exception("Expected closed parenthesis got " +
                                   std::string(1, symp->Sym));
          }
        }
      };
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr)
        throw err::Exception("Line: " + std::to_string(declarationLine) +
                             " Expected a function body");
      sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
    }

    if (sym.Sym == '{') {
      tokens.pop();
      parser.pushAsyncContext(this->isAsync);
      this->statement = parser.parseStmt(tokens);
      parser.popAsyncContext();
      if (tokens.head != nullptr)
        this->logicalLine = lex::tokenLine(tokens.peek());
    } else if (sym.Sym == ';') {
      this->statement = nullptr;
      this->logicalLine = sym.lineCount;
    } else {
      throw err::Exception(
          "Line: " + std::to_string(declarationLine) +
          " Incomplete function declaration: expected `{` to begin the body "
          "or `;` for a declaration");
    }
  } else
    throw err::Exception(
        "Line: " + std::to_string(declarationLine) +
        " Incomplete function declaration: expected `{` to begin the body "
        "or `;` for a declaration");
}

Function::Function(const string &ident, const ScopeMod &scope, const Type &type,
                   const Op op, const std::string &scopeName,
                   links::LinkedList<lex::Token *> &tokens,
                   parse::Parser &parser, bool optional, bool safe)
    : scope(scope), type(type), op(op), scopeName(scopeName),
      optional(optional), safe(safe) {
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
                   bool safe, bool isAsync)
    : scope(scope), genericTypes(genericTypes), safe(safe), isAsync(isAsync) {
  // updated function syntax
  // func <ident>(<args>) -> <type> { <body> }
  const int declarationLine =
      tokens.head != nullptr ? lex::tokenLine(tokens.peek()) : 1;
  const auto ident = tokens.head != nullptr
                         ? dynamic_cast<lex::LObj *>(tokens.pop())
                         : nullptr;
  if (ident == nullptr)
    throw err::Exception("Line: " + std::to_string(declarationLine) +
                         " Expected Identifier in function declaration");

  this->ident.ident = ident->meta;
  this->logicalLine = ident->lineCount;

  auto openBracket = tokens.head != nullptr
                         ? dynamic_cast<lex::OpSym *>(tokens.pop())
                         : nullptr;
  if (openBracket == nullptr || openBracket->Sym != '(')
    throw err::Exception("Line: " + std::to_string(ident->lineCount) +
                         " Expected '(' after function name");
  this->args = parser.parseArgs(tokens, ',', ')', this->argTypes, this->req,
                                this->mutability, this->optConvertionIndices,
                                this->readOnly);

  auto arrow = tokens.head != nullptr
                   ? dynamic_cast<lex::Symbol *>(tokens.peek())
                   : nullptr;
  if (arrow && arrow->meta == "->") {
    tokens.pop();

    while (true) {
      if (auto modifier = tokens.head != nullptr
                              ? dynamic_cast<lex::LObj *>(tokens.peek())
                              : nullptr) {
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

    auto typeName = tokens.head != nullptr
                        ? dynamic_cast<lex::LObj *>(tokens.pop())
                        : nullptr;
    if (typeName == nullptr)
      throw err::Exception("Line: " + std::to_string(ident->lineCount) +
                           " Expected a return type after `->`");
    auto type = parser.getTypeList()[typeName->meta];
    if (type == nullptr)
      type = new Type(typeName->meta, asmc::QWord);
    auto templateTypeList =
        parser.parseTemplateTypeList(tokens, typeName->lineCount);

    auto typenameStr = type->typeName;
    if (!templateTypeList.empty()) {
      typenameStr += "<";
      for (size_t i = 0; i < templateTypeList.size(); ++i) {
        typenameStr += templateTypeList[i];
        if (i != templateTypeList.size() - 1) {
          typenameStr += ",";
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

  auto optional = tokens.head != nullptr
                      ? dynamic_cast<lex::Ref *>(tokens.peek())
                      : nullptr;
  if (optional != nullptr) {
    this->optional = true;
    tokens.pop();
  } else if (auto opSym = tokens.head != nullptr
                              ? dynamic_cast<lex::OpSym *>(tokens.peek())
                              : nullptr) {
    if (opSym->Sym == '!') {
      this->error = true;
      tokens.pop();
    }
  }

  parseFunctionBody(tokens, parser);
}

gen::GenerationResult const Function::generate(gen::CodeGenerator &generator) {
  auto registerGenericFunction = [&]() {
    auto &functions = generator.genericFunctions();
    const std::string baseIdent = this->ident.ident;
    int nextOverload = 0;
    const std::string overloadPrefix = baseIdent + "_ovl";
    for (const auto &candidate : functions) {
      if (candidate.ident.ident == baseIdent ||
          candidate.ident.ident.rfind(overloadPrefix, 0) == 0)
        nextOverload = std::max(nextOverload, candidate.overloadIndex + 1);
    }
    if (nextOverload > 0) {
      this->overloadIndex = nextOverload;
      this->ident.ident += "_ovl" + std::to_string(nextOverload);
    }
    functions << *this;
  };

  if (this->isAsync && this->statement != nullptr) {
    if (!this->genericTypes.empty()) {
      registerGenericFunction();
      return {asmc::File(), std::nullopt};
    }
    const bool asyncMethod = generator.scope() != nullptr && !globalLocked;
    const bool hiddenDefinition =
        this->hidden ||
        (generator.scope() != nullptr && generator.scope()->hidden);
    if (hiddenDefinition) {
      auto *bodyStatement = this->statement;
      this->statement = nullptr;
      auto registration = this->generate(generator);
      this->statement = bodyStatement;
      return registration;
    }
    const std::string declaredIdent = this->ident.ident;
    const int capturedArgs =
        static_cast<int>(this->argTypes.size()) + (asyncMethod ? 1 : 0);
    if (capturedArgs > 6)
      generator.alert("async functions support at most six arguments", true,
                      __FILE__, __LINE__);
    if (this->type.opType == asmc::Float)
      generator.alert("async functions cannot return float yet", true, __FILE__,
                      __LINE__);
    for (const auto &arg : this->argTypes) {
      if (arg.opType == asmc::Float)
        generator.alert("async functions cannot capture float arguments yet",
                        true, __FILE__, __LINE__);
    }

    auto *bodyStatement = this->statement;
    this->statement = nullptr;
    auto registration = this->generate(generator);
    this->statement = bodyStatement;
    if (asyncMethod)
      this->ident.ident = declaredIdent;

    Function body(*this, false);
    body.ident.ident = "__af_async_body_" + this->ident.ident;
    body.isAsync = false;
    body.isAsyncBody = true;
    body.globalLocked = !asyncMethod;
    auto bodyFile = body.generate(generator).file;

    const bool isMain = this->ident.ident == "main";
    const std::string constructorLabel =
        isMain        ? "__af_async_main"
        : asyncMethod ? "pub_" + this->scopeName + "_" + this->ident.ident
                      : this->ident.ident;
    const std::string bodyLabel =
        asyncMethod ? "pub_" + this->scopeName + "_" + body.ident.ident
                    : body.ident.ident;
    asmc::File file;
    file << registration.file;
    file << bodyFile;
    auto constructorFile =
        asyncConstructor(*this, constructorLabel, bodyLabel, capturedArgs);
    if (this->scope == ast::Export) {
      const std::string exportedLabel =
          generator.moduleId() + '.' + constructorLabel;
      auto *global = new asmc::LinkTask();
      global->command = "global";
      global->operand = exportedLabel;
      global->logicalLine = this->logicalLine;
      file.linker << global;
      auto *label = new asmc::Label();
      label->label = exportedLabel;
      label->logicalLine = this->logicalLine;
      // Generated instruction lists are reversed before assembly is written.
      // Append the alias after the constructor label here so it appears
      // immediately before that label in the emitted assembly.
      constructorFile.text.append(label);
    }
    file << constructorFile;
    if (isMain) {
      if (!this->argTypes.empty())
        generator.alert("async main cannot take arguments", true, __FILE__,
                        __LINE__);
      file << asyncMainWrapper(constructorLabel, this->logicalLine);
    }
    return {file, std::nullopt};
  }
  // if the function is generic, do not generate code for it. It will be
  // generated when it is called with specific types.
  if (this->genericTypes.size() > 0) {
    registerGenericFunction();
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
        const bool concreteGenericBody =
            this->statement != nullptr && !this->hidden &&
            isConcreteGenericScope(this->scopeName);
        if (concreteGenericBody) {
          for (const auto &candidate : table) {
            if (candidate.ident.ident == this->ident.ident &&
                candidate.scopeName == this->scopeName &&
                (candidate.hidden || candidate.statement == nullptr))
              return;
          }
        }
        if (auto *firstInstance = table[this->ident.ident]) {
          bool forwardDeclaration = (firstInstance->statement == nullptr &&
                                     this->statement != nullptr);
          const bool deferredHiddenBody =
              firstInstance->hidden && !this->hidden &&
              firstInstance->scopeName == this->scopeName &&
              firstInstance->ident.ident == this->ident.ident;
          if (!forwardDeclaration && !deferredHiddenBody &&
              !firstInstance->wasGeneric &&
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
    std::string emittedLabel;
    if (generator.scope() == nullptr || this->isLambda || this->globalLocked)
      emittedLabel = this->ident.ident;
    else
      emittedLabel =
          "pub_" + generator.scope()->Ident + "_" + this->ident.ident;
    if (this->scopeName != "global")
      emittedLabel = "pub_" + this->scopeName + "_" + this->ident.ident;

    const auto emissionKey = functionEmissionKey(emittedLabel);
    if (isConcreteGenericScope(this->scopeName) &&
        generator.generatedLazyConcreteMethodNames().find(emissionKey) ==
            generator.generatedLazyConcreteMethodNames().end())
      return {file, std::nullopt};

    if (isConcreteGenericScope(this->scopeName) &&
        generator.suppressLazyMethodEmission())
      return {file, std::nullopt};

    if (!generator.generatedFunctionNames().insert(emissionKey).second)
      return {file, std::nullopt};
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
      movy->to = "-" + std::to_string(byteMod) + +"(%rbp)";
      file.text << movy;
      generator.intArgsCounter()++;
    };
    int counter = 0;
    auto argmute = generator.GenArgs(this->args, file, *this, counter);
    if (!isLambda && this->scope == ast::Public &&
        (!hidden || isConcreteGenericScope(this->scopeName))) {
      if (isConcreteGenericScope(this->scopeName))
        link->command = "weak";
      file.linker.push(link);
    }

    file << argmute;
    const int asyncDispatchLocation = file.text.count;

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
    const int trackedFrame =
        gen::scope::ScopeManager::getInstance()->getStackAlignment();
    const int emittedFrame = alignFrameSize(maxFrameOffset(file));
    const int frameSize = std::max(trackedFrame, emittedFrame);
    sub->op1 = "$" + std::to_string(frameSize);
    sub->op2 = generator.registers()["%rsp"]->get(asmc::QWord);
    file.text.insert(sub, AlignmentLoc + 1);
    if (this->isAsyncBody) {
      replaceAsyncFrameSize(file, frameSize);
      auto dispatch = asyncDispatch(*this, frameSize);
      int location = asyncDispatchLocation + 2;
      for (auto *instruction : dispatch)
        file.text.insert(instruction, location++);
    }

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
  if (this->isAsync)
    output.type = "task<" + output.type + ">";
  output.size = this->isAsync || this->optional || this->error
                    ? asmc::QWord
                    : this->type.size;
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
