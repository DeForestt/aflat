#include "CodeGenerator/CodeGenerator.hpp"

#include <execinfo.h>
#include <unistd.h>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <utility>

#include "CodeGenerator/GenerationResult.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Types.hpp"
#include "CodeGenerator/Utils.hpp"
#include "ErrorReporter.hpp"
#include "Exceptions.hpp"
#include "Parser/AST.hpp"
#include "Scanner.hpp"

using namespace gen::utils;

struct gen::CodeGenerator::Impl {
  Impl(std::string moduleId, parse::Parser &parser, const std::string &source,
       const std::string &cwd)
      : parser(parser), moduleId(std::move(moduleId)), source(source),
        cwd(cwd.empty() ? std::filesystem::current_path()
                        : std::filesystem::path(cwd)) {
    registers << asmc::Register("rax", "eax", "ax", "al");
    registers << asmc::Register("rcx", "ecx", "cx", "cl");
    registers << asmc::Register("rdx", "edx", "dx", "dl");
    registers << asmc::Register("rbx", "ebx", "bx", "bl");
    registers << asmc::Register("rsi", "esi", "si", "sil");
    registers << asmc::Register("rdi", "edi", "di", "dil");
    registers << asmc::Register("rsp", "esp", "sp", "spl");
    registers << asmc::Register("rbp", "ebp", "bp", "bpl");
    registers << asmc::Register("r8", "r8d", "r8w", "r8b");
    registers << asmc::Register("r9", "r9d", "r9w", "r9b");
    registers << asmc::Register("r11", "r11d", "r11w", "r11b");
    registers << asmc::Register("r12", "r12d", "r12w", "r12b");
    registers << asmc::Register("r13", "r13d", "r13w", "r13b");
    registers << asmc::Register("r14", "r14d", "r14w", "r14b");
    registers << asmc::Register("r15", "r15d", "r15w", "r15b");
    registers << asmc::Register("xmm0", "xmm0", "xmm0", "xmm0");
    registers << asmc::Register("xmm1", "xmm1", "xmm1", "xmm1");
    registers << asmc::Register("xmm2", "xmm2", "xmm2", "xmm2");
    registers << asmc::Register("xmm3", "xmm3", "xmm3", "xmm3");
    registers.foo = asmc::Register::compare;
    nameTable.foo = compareFunc;
    genericFunctions.foo = compareFunc;
    includedMemo = HashMap<ast::Statement *>();
    nameSpaceTable = HashMap<std::string>();
    typeList.foo = gen::Type::compare;
    TypeList.foo = ast::Type::compare;
    auto nullType = new ast::Type("NULLTYPE", asmc::QWord);
    TypeList.push(*nullType);
    intArgs = {asmc::Register("rdi", "edi", "di", "dil"),
               asmc::Register("rsi", "esi", "si", "sil"),
               asmc::Register("rdx", "edx", "dx", "dl"),
               asmc::Register("rcx", "ecx", "cx", "cl"),
               asmc::Register("r8", "r8d", "r8w", "r8b"),
               asmc::Register("r9", "r9d", "r9w", "r9b")};
  }

  parse::Parser &parser;
  gen::Class *scope = nullptr;
  ast::Match *matchScope = nullptr;
  ast::Type returnType;
  int scopePop = 0;
  int mutability = 0;
  int labelCount = 0;
  int intArgsCounter = 0;
  int selectReg = 0;
  bool globalScope = true;
  bool inFunction = false;
  bool returnFlag = false;
  bool errorFlag = false;
  HashMap<ast::Statement *> includedMemo;
  HashMap<ast::Statement *> includedClasses;
  HashMap<std::string> nameSpaceTable;
  std::unordered_map<std::string, std::string> genericTypeConversions;
  std::set<std::string> generatedFunctionNames;
  std::set<std::string> generatedClassNames;
  ast::Function *currentFunction = nullptr;
  std::string lambdaReturns;
  asmc::Size lambdaSize = asmc::AUTO;
  links::SLinkedList<gen::Type *, std::string> typeList;
  std::unordered_map<std::string, ast::Class *> genericTypes;
  links::SLinkedList<asmc::Register, std::string> registers;
  links::LinkedList<Symbol> SymbolTable;
  links::LinkedList<Symbol> GlobalSymbolTable;
  links::SLinkedList<ast::Function, std::string> nameTable;
  links::SLinkedList<ast::Function, std::string> genericFunctions;
  links::SLinkedList<ast::Type, std::string> TypeList;
  std::unordered_map<std::string, ast::Transform> transforms;
  std::vector<EnvState> envStack;
  std::array<asmc::Register, 6> intArgs;
  int logicalLine = 0;
  int tempCount = 0;
  std::string moduleId;
  std::string source;
  std::filesystem::path cwd;
  links::LinkedList<std::string> breakContext;
  links::LinkedList<std::string> continueContext;
};

bool gen::CodeGenerator::traceAlert = false;

void gen::CodeGenerator::enableAlertTrace(bool enable) { traceAlert = enable; }

void gen::CodeGenerator::alert(std::string message, bool error,
                               const char *file, int line) {
  if (traceAlert && file) {
    std::cerr << "[CG alert] " << file << ":" << line << std::endl;
    void *array[10];
    int size = backtrace(array, 10);
    char **symbols = backtrace_symbols(array, size);
    for (int i = 0; i < size; ++i)
      std::cerr << symbols[i] << std::endl;
    free(symbols);
  }
  if (error) {
    impl->errorFlag = true;
    std::string context;
    if (impl->scope != nullptr)
      context += "in class " + impl->scope->Ident + ": ";
    if (!impl->globalScope && impl->currentFunction != nullptr)
      context += "in function " + impl->currentFunction->ident.ident + ": ";
    error::report(impl->moduleId, impl->logicalLine, context + message,
                  impl->source);
    throw err::Exception("Line: " + std::to_string(impl->logicalLine) + " " +
                         context + message);
  } else {
    std::string context;
    if (impl->scope != nullptr)
      context += "in class " + impl->scope->Ident + ": ";
    if (!impl->globalScope && impl->currentFunction != nullptr)
      context += "in function " + impl->currentFunction->ident.ident + ": ";
    error::warn(impl->moduleId, impl->logicalLine, context + message,
                impl->source);
  }
};

gen::CodeGenerator::CodeGenerator(std::string moduleId, parse::Parser &parser,
                                  const std::string &source,
                                  const std::string &cwd)
    : impl(std::make_unique<Impl>(std::move(moduleId), parser, source, cwd)) {}

gen::CodeGenerator::~CodeGenerator() = default;

gen::CodeGenerator::CodeGenerator(CodeGenerator &&) noexcept = default;

gen::CodeGenerator &
gen::CodeGenerator::operator=(CodeGenerator &&) noexcept = default;

bool gen::CodeGenerator::canAssign(ast::Type type, std::string typeName,
                                   std::string fmt, bool strict, bool panic) {
  if (type.typeName == typeName)
    return true;
  if (type.fPointerArgs.returnType != nullptr && typeName == "adr")
    return true;
  if (type.typeName == "adr" && typeName.find("~") != std::string::npos)
    return true;
  if (typeName == "void" && type.typeName == "NULLTYPE")
    return true;
  if (typeName == "NULLTYPE" && type.typeName == "void")
    return true;
  if (typeName == "generic" && type.typeName == "NULLTYPE")
    return true;
  if (typeName == "NULLTYPE" && type.typeName == "generic")
    return true;
  if (typeName == "void")
    this->alert("cannot use void function as value", true, __FILE__, __LINE__);
  if (typeName == "--std--flex--function" || typeName == "any" ||
      type.typeName == "any")
    return true;
  if (type.typeName == "int" && typeName == "float")
    return true;
  if (type.typeName == "float" && typeName == "int")
    return true;
  if (type.typeName == "short" && typeName == "number")
    return true;
  if (type.typeName == "int" && typeName == "number")
    return true;
  if (type.typeName == "long" && typeName == "number")
    return true;
  if (type.typeName == "byte" && typeName == "number")
    return true;
  if (type.typeName == "number" && typeName == "short")
    return true;
  if (type.typeName == "number" && typeName == "int")
    return true;
  if (type.typeName == "number" && typeName == "long")
    return true;
  if (type.typeName == "number" && typeName == "float")
    return true;
  if (type.typeName == "number" && typeName == "byte")
    return true;

  gen::Type **expected = impl->typeList[type.typeName];
  gen::Class *cl = nullptr;
  if (expected) {
    cl = dynamic_cast<gen::Class *>(*expected);
  }

  if (typeName != "generic") {
    // search the type list for the type
    gen::Type **udef = impl->typeList[typeName];
    if (udef != nullptr) {
      // if the type is a class
      gen::Class *cl = dynamic_cast<gen::Class *>(*udef);
      if (cl != nullptr) {
        if (type.typeName == "adr" && !cl->pedantic)
          return true;
        gen::Class *parent = cl->parent;
        if (parent != nullptr) {
          if (parent->Ident == type.typeName)
            return true;
        }
      } else {
        if (type.typeName == "adr")
          return true;
      }
    }

    if (cl && !strict) {
      if (cl->nameTable.count > 0) {
        ast::Function *init = cl->nameTable["init"];
        if (init) {
          if (init->req == 1 &&
              this->canAssign(init->argTypes[0], typeName, fmt)) {
            return false;
          }
        }
      }
    }
  }

  if ((type.size == asmc::QWord &&
       (typeName == "adr" || typeName == "generic")) &&
      (strict || !cl || !cl->pedantic)) {
    return true;
  }

  if (strict && (type.typeName == "adr" || typeName == "generic"))
    return true;

  // compare two function pointers
  if (type.fPointerArgs.returnType != nullptr &&
      typeName.find("~") != std::string::npos) {
    // we need to check return type and args
    auto type2 = impl->TypeList[typeName];
    if (type2 == nullptr)
      return false;
    if (type2->fPointerArgs.returnType == nullptr)
      return false;
    this->canAssign(*type2->fPointerArgs.returnType,
                    type.fPointerArgs.returnType->typeName, fmt);
    // type two should have at least as many required args as type
    if (type2->fPointerArgs.requiredArgs < type.fPointerArgs.requiredArgs)
      return false;
    for (int i = 0; i < type.fPointerArgs.requiredArgs; i++) {
      this->canAssign(type.fPointerArgs.argTypes[i],
                      type2->fPointerArgs.argTypes[i].typeName, fmt);
    };

    // now check the types of any extra optional args
    if (type2->fPointerArgs.argTypes.size() > type.fPointerArgs.requiredArgs) {
      for (int i = type.fPointerArgs.requiredArgs - 1;
           i < type2->fPointerArgs.argTypes.size(); i++) {
        this->canAssign(type.fPointerArgs.argTypes[i],
                        type2->fPointerArgs.argTypes[i].typeName, fmt);
      }
    }

    // Check that the optional convertion indices are the same
    for (int i = 0; i < type.fPointerArgs.optConvertionIndices.size(); i++) {
      if (type.fPointerArgs.optConvertionIndices[i] !=
          type2->fPointerArgs.optConvertionIndices[i])
        return false;
    }

    return true;
  };

  if (panic) {
    alert(format("Type mismatch on line {}: " + fmt, impl->logicalLine,
                 type.typeName, typeName),
          true, __FILE__, __LINE__);
  }
  return false;
}

bool gen::CodeGenerator::hasError() const { return impl->errorFlag; }

parse::Parser &gen::CodeGenerator::parser() { return impl->parser; }
const parse::Parser &gen::CodeGenerator::parser() const { return impl->parser; }

gen::Class *&gen::CodeGenerator::scope() { return impl->scope; }
const gen::Class *gen::CodeGenerator::scope() const { return impl->scope; }

ast::Match *&gen::CodeGenerator::matchScope() { return impl->matchScope; }
const ast::Match *gen::CodeGenerator::matchScope() const {
  return impl->matchScope;
}

ast::Type &gen::CodeGenerator::returnType() { return impl->returnType; }
const ast::Type &gen::CodeGenerator::returnType() const {
  return impl->returnType;
}

int &gen::CodeGenerator::scopePop() { return impl->scopePop; }
const int &gen::CodeGenerator::scopePop() const { return impl->scopePop; }

int &gen::CodeGenerator::mutability() { return impl->mutability; }
const int &gen::CodeGenerator::mutability() const { return impl->mutability; }

int &gen::CodeGenerator::labelCount() { return impl->labelCount; }
const int &gen::CodeGenerator::labelCount() const { return impl->labelCount; }

int &gen::CodeGenerator::intArgsCounter() { return impl->intArgsCounter; }
const int &gen::CodeGenerator::intArgsCounter() const {
  return impl->intArgsCounter;
}

int &gen::CodeGenerator::selectReg() { return impl->selectReg; }
const int &gen::CodeGenerator::selectReg() const { return impl->selectReg; }

bool &gen::CodeGenerator::globalScope() { return impl->globalScope; }
const bool &gen::CodeGenerator::globalScope() const {
  return impl->globalScope;
}

bool &gen::CodeGenerator::inFunction() { return impl->inFunction; }
const bool &gen::CodeGenerator::inFunction() const { return impl->inFunction; }

bool &gen::CodeGenerator::returnFlag() { return impl->returnFlag; }
const bool &gen::CodeGenerator::returnFlag() const { return impl->returnFlag; }

bool &gen::CodeGenerator::errorFlag() { return impl->errorFlag; }
const bool &gen::CodeGenerator::errorFlag() const { return impl->errorFlag; }

HashMap<ast::Statement *> &gen::CodeGenerator::includedMemo() {
  return impl->includedMemo;
}

HashMap<ast::Statement *> &gen::CodeGenerator::includedClasses() {
  return impl->includedClasses;
}

HashMap<std::string> &gen::CodeGenerator::nameSpaceTable() {
  return impl->nameSpaceTable;
}

std::unordered_map<std::string, std::string> &
gen::CodeGenerator::genericTypeConversions() {
  return impl->genericTypeConversions;
}

std::set<std::string> &gen::CodeGenerator::generatedFunctionNames() {
  return impl->generatedFunctionNames;
}

const std::set<std::string> &
gen::CodeGenerator::generatedFunctionNames() const {
  return impl->generatedFunctionNames;
}

std::set<std::string> &gen::CodeGenerator::generatedClassNames() {
  return impl->generatedClassNames;
}

ast::Function *&gen::CodeGenerator::currentFunction() {
  return impl->currentFunction;
}

const ast::Function *gen::CodeGenerator::currentFunction() const {
  return impl->currentFunction;
}

std::string &gen::CodeGenerator::lambdaReturns() { return impl->lambdaReturns; }

const std::string &gen::CodeGenerator::lambdaReturns() const {
  return impl->lambdaReturns;
}

asmc::Size &gen::CodeGenerator::lambdaSize() { return impl->lambdaSize; }
const asmc::Size &gen::CodeGenerator::lambdaSize() const {
  return impl->lambdaSize;
}

links::SLinkedList<gen::Type *, std::string> &gen::CodeGenerator::typeList() {
  return impl->typeList;
}

const links::SLinkedList<gen::Type *, std::string> &
gen::CodeGenerator::typeList() const {
  return impl->typeList;
}

std::unordered_map<std::string, ast::Class *> &
gen::CodeGenerator::genericTypes() {
  return impl->genericTypes;
}

const std::unordered_map<std::string, ast::Class *> &
gen::CodeGenerator::genericTypes() const {
  return impl->genericTypes;
}

links::SLinkedList<asmc::Register, std::string> &
gen::CodeGenerator::registers() {
  return impl->registers;
}

const links::SLinkedList<asmc::Register, std::string> &
gen::CodeGenerator::registers() const {
  return impl->registers;
}

links::LinkedList<gen::Symbol> &gen::CodeGenerator::SymbolTable() {
  return impl->SymbolTable;
}

const links::LinkedList<gen::Symbol> &gen::CodeGenerator::SymbolTable() const {
  return impl->SymbolTable;
}

links::LinkedList<gen::Symbol> &gen::CodeGenerator::GlobalSymbolTable() {
  return impl->GlobalSymbolTable;
}

const links::LinkedList<gen::Symbol> &
gen::CodeGenerator::GlobalSymbolTable() const {
  return impl->GlobalSymbolTable;
}

links::SLinkedList<ast::Function, std::string> &
gen::CodeGenerator::nameTable() {
  return impl->nameTable;
}

const links::SLinkedList<ast::Function, std::string> &
gen::CodeGenerator::nameTable() const {
  return impl->nameTable;
}

links::SLinkedList<ast::Function, std::string> &
gen::CodeGenerator::genericFunctions() {
  return impl->genericFunctions;
}

const links::SLinkedList<ast::Function, std::string> &
gen::CodeGenerator::genericFunctions() const {
  return impl->genericFunctions;
}

links::SLinkedList<ast::Type, std::string> &gen::CodeGenerator::TypeList() {
  return impl->TypeList;
}

const links::SLinkedList<ast::Type, std::string> &
gen::CodeGenerator::TypeList() const {
  return impl->TypeList;
}

std::unordered_map<std::string, ast::Transform> &
gen::CodeGenerator::transforms() {
  return impl->transforms;
}

const std::unordered_map<std::string, ast::Transform> &
gen::CodeGenerator::transforms() const {
  return impl->transforms;
}

std::vector<gen::CodeGenerator::EnvState> &gen::CodeGenerator::envStack() {
  return impl->envStack;
}

const std::vector<gen::CodeGenerator::EnvState> &
gen::CodeGenerator::envStack() const {
  return impl->envStack;
}

std::array<asmc::Register, 6> &gen::CodeGenerator::intArgs() {
  return impl->intArgs;
}

const std::array<asmc::Register, 6> &gen::CodeGenerator::intArgs() const {
  return impl->intArgs;
}

int &gen::CodeGenerator::logicalLine() { return impl->logicalLine; }
const int &gen::CodeGenerator::logicalLine() const { return impl->logicalLine; }

int &gen::CodeGenerator::tempCount() { return impl->tempCount; }
const int &gen::CodeGenerator::tempCount() const { return impl->tempCount; }

std::filesystem::path &gen::CodeGenerator::cwd() { return impl->cwd; }
const std::filesystem::path &gen::CodeGenerator::cwd() const {
  return impl->cwd;
}

std::string &gen::CodeGenerator::moduleId() { return impl->moduleId; }
const std::string &gen::CodeGenerator::moduleId() const {
  return impl->moduleId;
}

std::string &gen::CodeGenerator::source() { return impl->source; }
const std::string &gen::CodeGenerator::source() const { return impl->source; }

links::LinkedList<std::string> &gen::CodeGenerator::breakContext() {
  return impl->breakContext;
}

const links::LinkedList<std::string> &gen::CodeGenerator::breakContext() const {
  return impl->breakContext;
}

links::LinkedList<std::string> &gen::CodeGenerator::continueContext() {
  return impl->continueContext;
}

const links::LinkedList<std::string> &
gen::CodeGenerator::continueContext() const {
  return impl->continueContext;
}

std::vector<std::string> splitTypeName(const std::string &typeName) {
  std::vector<std::string> parts;
  std::stringstream ss(typeName);
  std::string part;

  while (std::getline(ss, part, '.')) {
    if (!part.empty()) {
      parts.push_back(part);
    }
  }

  return parts;
}

gen::Type **gen::CodeGenerator::getType(std::string typeName,
                                        asmc::File &OutputFile) {
  gen::Type **type = impl->typeList[typeName];
  if (type == nullptr) {
    // find . in the type name
    if (typeName.find('<') == std::string::npos)
      this->alert("Type " + typeName +
                      " not found in type list, did you forget to "
                      "include the file that defines it?",
                  true, __FILE__, __LINE__);

    auto parsedTemplate = gen::utils::parseGenericName(typeName, *this);

    auto typeNamePart = std::get<0>(parsedTemplate);
    auto templates = std::get<1>(parsedTemplate);
    // find the class of this type...
    auto cl = impl->genericTypes[typeNamePart];

    if (cl == nullptr) {
      this->alert("Type " + typeNamePart +
                      " not found in type list, did you forget to "
                      "include the file that defines it?",
                  true, __FILE__, __LINE__);
    }

    if (cl->genericTypes.size() != templates.size()) {
      this->alert("Type " + typeName + " has " +
                      std::to_string(cl->genericTypes.size()) +
                      " generic types, but " +
                      std::to_string(templates.size()) +
                      " were provided. Please check the type definition.",
                  true, __FILE__, __LINE__);
    }

    for (auto &templateType : templates) {
      if (parse::PRIMITIVE_TYPES.find(templateType) ==
          parse::PRIMITIVE_TYPES.end()) {
        getType(templateType, OutputFile); // This ensures that the types are
                                           // valid and registered in the type
                                           // list if they are also generic
      }
    }

    return this->instantiateGenericClass(cl, templates, typeName, OutputFile);
  }
  return type;
}

asmc::File gen::CodeGenerator::memMove(std::string from, std::string to,
                                       int bytes) {
  asmc::File file;

  asmc::Push *pushRsi = new asmc::Push();
  pushRsi->op = impl->registers["%rsi"]->get(asmc::QWord);
  pushRsi->size = asmc::QWord;
  pushRsi->logicalLine = impl->logicalLine;
  file.text << pushRsi;

  asmc::Push *pushRdi = new asmc::Push();
  pushRdi->op = impl->registers["%rdi"]->get(asmc::QWord);
  pushRdi->size = asmc::QWord;
  pushRdi->logicalLine = impl->logicalLine;
  file.text << pushRdi;

  asmc::Push *pushRcx = new asmc::Push();
  pushRcx->op = impl->registers["%rcx"]->get(asmc::QWord);
  pushRcx->size = asmc::QWord;
  pushRcx->logicalLine = impl->logicalLine;
  file.text << pushRcx;

  asmc::Push *pushRax = new asmc::Push();
  pushRax->op = impl->registers["%rax"]->get(asmc::QWord);
  pushRax->size = asmc::QWord;
  pushRax->logicalLine = impl->logicalLine;
  file.text << pushRax;

  asmc::Mov *movFrom = new asmc::Mov();
  movFrom->from = from;
  movFrom->to = impl->registers["%rsi"]->get(asmc::QWord);
  movFrom->size = asmc::QWord;
  movFrom->logicalLine = impl->logicalLine;
  file.text << movFrom;

  asmc::Mov *movTo = new asmc::Mov();
  movTo->from = to;
  movTo->to = impl->registers["%rdi"]->get(asmc::QWord);
  movTo->size = asmc::QWord;
  movTo->logicalLine = impl->logicalLine;
  file.text << movTo;

  asmc::Mov *movCount = new asmc::Mov();
  movCount->from = "$" + std::to_string(bytes);
  movCount->to = impl->registers["%rcx"]->get(asmc::QWord);
  movCount->size = asmc::QWord;
  movCount->logicalLine = impl->logicalLine;
  file.text << movCount;

  std::string loopLabelStr = "L_" + gen::utils::generateUUID();
  asmc::Label *loopLabel = new asmc::Label();
  loopLabel->label = loopLabelStr;
  loopLabel->logicalLine = impl->logicalLine;
  file.text << loopLabel;

  asmc::Mov *load = new asmc::Mov();
  load->from = '(' + impl->registers["%rsi"]->get(asmc::QWord) + ')';
  load->to = impl->registers["%al"]->get(asmc::Byte);
  load->size = asmc::Byte;
  load->logicalLine = impl->logicalLine;
  file.text << load;

  asmc::Mov *store = new asmc::Mov();
  store->from = impl->registers["%al"]->get(asmc::Byte);
  store->to = '(' + impl->registers["%rdi"]->get(asmc::QWord) + ')';
  store->size = asmc::Byte;
  store->logicalLine = impl->logicalLine;
  file.text << store;

  asmc::Add *incSrc = new asmc::Add();
  incSrc->op1 = "$1";
  incSrc->op2 = impl->registers["%rsi"]->get(asmc::QWord);
  incSrc->size = asmc::QWord;
  incSrc->logicalLine = impl->logicalLine;
  file.text << incSrc;

  asmc::Add *incDst = new asmc::Add();
  incDst->op1 = "$1";
  incDst->op2 = impl->registers["%rdi"]->get(asmc::QWord);
  incDst->size = asmc::QWord;
  incDst->logicalLine = impl->logicalLine;
  file.text << incDst;

  asmc::Sub *dec = new asmc::Sub();
  dec->op1 = "$1";
  dec->op2 = impl->registers["%rcx"]->get(asmc::QWord);
  dec->size = asmc::QWord;
  dec->logicalLine = impl->logicalLine;
  file.text << dec;

  asmc::Cmp *cmp = new asmc::Cmp();
  cmp->from = "$0";
  cmp->to = impl->registers["%rcx"]->get(asmc::QWord);
  cmp->size = asmc::QWord;
  cmp->logicalLine = impl->logicalLine;
  file.text << cmp;

  asmc::Jne *jne = new asmc::Jne();
  jne->to = loopLabelStr;
  jne->logicalLine = impl->logicalLine;
  file.text << jne;

  asmc::Pop *popRax = new asmc::Pop();
  popRax->op = impl->registers["%rax"]->get(asmc::QWord);
  popRax->size = asmc::QWord;
  popRax->logicalLine = impl->logicalLine;
  file.text << popRax;

  asmc::Pop *popRcx = new asmc::Pop();
  popRcx->op = impl->registers["%rcx"]->get(asmc::QWord);
  popRcx->size = asmc::QWord;
  popRcx->logicalLine = impl->logicalLine;
  file.text << popRcx;

  asmc::Pop *popRdi = new asmc::Pop();
  popRdi->op = impl->registers["%rdi"]->get(asmc::QWord);
  popRdi->size = asmc::QWord;
  popRdi->logicalLine = impl->logicalLine;
  file.text << popRdi;

  asmc::Pop *popRsi = new asmc::Pop();
  popRsi->op = impl->registers["%rsi"]->get(asmc::QWord);
  popRsi->size = asmc::QWord;
  popRsi->logicalLine = impl->logicalLine;
  file.text << popRsi;

  return file;
}

asmc::File gen::CodeGenerator::setOffset(std::string to, int offset,
                                         std::string from, asmc::Size size) {
  asmc::File file;

  asmc::Push *pushRdi = new asmc::Push();
  pushRdi->op = impl->registers["%rdi"]->get(asmc::QWord);
  pushRdi->size = asmc::QWord;
  pushRdi->logicalLine = impl->logicalLine;
  file.text << pushRdi;

  asmc::Push *pushRax = new asmc::Push();
  pushRax->op = impl->registers["%rax"]->get(asmc::QWord);
  pushRax->size = asmc::QWord;
  pushRax->logicalLine = impl->logicalLine;
  file.text << pushRax;

  asmc::Mov *movPtr = new asmc::Mov();
  movPtr->from = to;
  movPtr->to = impl->registers["%rdi"]->get(asmc::QWord);
  movPtr->size = asmc::QWord;
  movPtr->logicalLine = impl->logicalLine;
  file.text << movPtr;

  if (offset != 0) {
    asmc::Add *addOffset = new asmc::Add();
    addOffset->op1 = "$" + std::to_string(offset);
    addOffset->op2 = impl->registers["%rdi"]->get(asmc::QWord);
    addOffset->size = asmc::QWord;
    addOffset->logicalLine = impl->logicalLine;
    file.text << addOffset;
  }

  asmc::Mov *movVal = new asmc::Mov();
  movVal->from = from;
  movVal->to = impl->registers["%rax"]->get(size);
  movVal->size = size;
  movVal->logicalLine = impl->logicalLine;
  file.text << movVal;

  asmc::Mov *store = new asmc::Mov();
  store->from = impl->registers["%rax"]->get(size);
  store->to = '(' + impl->registers["%rdi"]->get(asmc::QWord) + ')';
  store->size = size;
  store->logicalLine = impl->logicalLine;
  file.text << store;

  asmc::Pop *popRax = new asmc::Pop();
  popRax->op = impl->registers["%rax"]->get(asmc::QWord);
  popRax->size = asmc::QWord;
  popRax->logicalLine = impl->logicalLine;
  file.text << popRax;

  asmc::Pop *popRdi = new asmc::Pop();
  popRdi->op = impl->registers["%rdi"]->get(asmc::QWord);
  popRdi->size = asmc::QWord;
  popRdi->logicalLine = impl->logicalLine;
  file.text << popRdi;

  return file;
}

bool gen::CodeGenerator::whenSatisfied(const ast::When &when) {
  auto evalPred = [this](const ast::WhenPredicat &pred) -> bool {
    if (pred.op == ast::WhenOperator::IS && pred.ident == "primitive") {
      bool isPrim = parse::PRIMITIVE_TYPES.find(pred.typeName) !=
                    parse::PRIMITIVE_TYPES.end();
      if (!pred.negated && !isPrim)
        return false;
      if (pred.negated && isPrim)
        return false;
      return true;
    } else if (pred.op == ast::WhenOperator::IS) {
      auto equal = pred.typeName == pred.ident;
      if (!pred.negated && !equal)
        return false;
      if (pred.negated && equal)
        return false;
      return true;
    } else if (pred.op == ast::WhenOperator::HAS) {
      gen::Type **type = impl->typeList[pred.typeName];
      auto has = false;
      if (type != nullptr) {
        gen::Class *cl = dynamic_cast<gen::Class *>(*type);
        if (cl != nullptr) {
          has = cl->publicNameTable[pred.ident] != nullptr;
        }
      }
      if (!pred.negated && !has)
        return false;
      if (pred.negated && has)
        return false;
      return true;
    }
    return false;
  };

  if (when.predicates.empty())
    return true;
  bool result = evalPred(when.predicates[0]);
  for (size_t i = 1; i < when.predicates.size(); ++i) {
    bool cur = evalPred(when.predicates[i]);
    auto join = when.predicates[i - 1].join;
    if (join == ast::WhenJoiner::AND)
      result = result && cur;
    else
      result = result || cur;
  }
  return result;
}
