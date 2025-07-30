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

bool gen::CodeGenerator::traceAlert = false;

void gen::CodeGenerator::enableAlertTrace(bool enable) { traceAlert = enable; }

void gen::CodeGenerator::alert(std::string message, bool error,
                               const char *file, int line) {
  if (traceAlert && file) {
    std::cerr << "[CG alert] " << file << ":" << line << std::endl;
    void *array[10];
    int size = backtrace(array, 10);
    char **symbols = backtrace_symbols(array, size);
    for (int i = 0; i < size; ++i) std::cerr << symbols[i] << std::endl;
    free(symbols);
  }
  if (error) {
    this->errorFlag = true;
    std::string context;
    if (this->scope != nullptr)
      context += "in class " + this->scope->Ident + ": ";
    if (!this->globalScope && this->currentFunction != nullptr)
      context += "in function " + this->currentFunction->ident.ident + ": ";
    error::report(this->moduleId, this->logicalLine, context + message,
                  this->source);
    throw err::Exception("Line: " + std::to_string(this->logicalLine) + " " +
                         context + message);
  } else {
    std::string context;
    if (this->scope != nullptr)
      context += "in class " + this->scope->Ident + ": ";
    if (!this->globalScope && this->currentFunction != nullptr)
      context += "in function " + this->currentFunction->ident.ident + ": ";
    error::warn(this->moduleId, this->logicalLine, context + message,
                this->source);
  }
};

gen::CodeGenerator::CodeGenerator(std::string moduleId, parse::Parser &parser,
                                  const std::string &source,
                                  const std::string &cwd)
    : parser(parser),
      source(source),
      cwd(cwd.empty() ? std::filesystem::current_path()
                      : std::filesystem::path(cwd)) {
  this->registers << asmc::Register("rax", "eax", "ax", "al");
  this->registers << asmc::Register("rcx", "ecx", "cx", "cl");
  this->registers << asmc::Register("rdx", "edx", "dx", "dl");
  this->registers << asmc::Register("rbx", "ebx", "bx", "bl");
  this->registers << asmc::Register("rsi", "esi", "si", "sil");
  this->registers << asmc::Register("rdi", "edi", "di", "dil");
  this->registers << asmc::Register("rsp", "esp", "sp", "spl");
  this->registers << asmc::Register("rbp", "ebp", "bp", "bpl");
  this->registers << asmc::Register("r8", "r8d", "r8w", "r8b");
  this->registers << asmc::Register("r9", "r9d", "r9w", "r9b");
  this->registers << asmc::Register("r11", "r11d", "r11w", "r11b");
  this->registers << asmc::Register("r12", "r12d", "r12w", "r12b");
  this->registers << asmc::Register("r13", "r13d", "r13w", "r13b");
  this->registers << asmc::Register("r14", "r14d", "r14w", "r14b");
  this->registers << asmc::Register("r15", "r15d", "r15w", "r15b");
  this->registers << asmc::Register("xmm0", "xmm0", "xmm0", "xmm0");
  this->registers << asmc::Register("xmm1", "xmm1", "xmm1", "xmm1");
  this->registers << asmc::Register("xmm2", "xmm2", "xmm2", "xmm2");
  this->registers << asmc::Register("xmm3", "xmm3", "xmm3", "xmm3");
  this->registers.foo = asmc::Register::compare;
  this->nameTable.foo = compareFunc;
  this->genericFunctions.foo = compareFunc;
  this->includedMemo = HashMap<ast::Statement *>();
  this->nameSpaceTable = HashMap<std::string>();
  this->globalScope = true;
  this->typeList.foo = gen::Type::compare;
  this->TypeList.foo = ast::Type::compare;
  auto nullType = new ast::Type("NULLTYPE", asmc::QWord);
  this->TypeList.push(*nullType);
  this->moduleId = moduleId;
  this->scope = nullptr;
}

bool gen::CodeGenerator::canAssign(ast::Type type, std::string typeName,
                                   std::string fmt, bool strict, bool panic) {
  if (type.typeName == typeName) return true;
  if (type.fPointerArgs.returnType != nullptr && typeName == "adr") return true;
  if (type.typeName == "adr" && typeName.find("~") != std::string::npos)
    return true;
  if (typeName == "void" && type.typeName == "NULLTYPE") return true;
  if (typeName == "NULLTYPE" && type.typeName == "void") return true;
  if (typeName == "generic" && type.typeName == "NULLTYPE") return true;
  if (typeName == "NULLTYPE" && type.typeName == "generic") return true;
  if (typeName == "void")
    this->alert("cannot use void function as value", true, __FILE__, __LINE__);
  if (typeName == "--std--flex--function" || typeName == "any" ||
      type.typeName == "any")
    return true;
  if (type.typeName == "int" && typeName == "float") return true;
  if (type.typeName == "float" && typeName == "int") return true;
  if (type.typeName == "short" && typeName == "number") return true;
  if (type.typeName == "int" && typeName == "number") return true;
  if (type.typeName == "long" && typeName == "number") return true;
  if (type.typeName == "byte" && typeName == "number") return true;
  if (type.typeName == "number" && typeName == "short") return true;
  if (type.typeName == "number" && typeName == "int") return true;
  if (type.typeName == "number" && typeName == "long") return true;
  if (type.typeName == "number" && typeName == "float") return true;
  if (type.typeName == "number" && typeName == "byte") return true;

  gen::Type **expected = this->typeList[type.typeName];
  gen::Class *cl = nullptr;
  if (expected) {
    cl = dynamic_cast<gen::Class *>(*expected);
  }

  if (typeName != "generic") {
    // search the type list for the type
    gen::Type **udef = this->typeList[typeName];
    if (udef != nullptr) {
      // if the type is a class
      gen::Class *cl = dynamic_cast<gen::Class *>(*udef);
      if (cl != nullptr) {
        if (type.typeName == "adr" && !cl->pedantic) return true;
        gen::Class *parent = cl->parent;
        if (parent != nullptr) {
          if (parent->Ident == type.typeName) return true;
        }
      } else {
        if (type.typeName == "adr") return true;
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

  if (strict && (type.typeName == "adr" || typeName == "generic")) return true;

  // compare two function pointers
  if (type.fPointerArgs.returnType != nullptr &&
      typeName.find("~") != std::string::npos) {
    // we need to check return type and args
    auto type2 = this->TypeList[typeName];
    if (type2 == nullptr) return false;
    if (type2->fPointerArgs.returnType == nullptr) return false;
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
    alert(format("Type mismatch on line {}: " + fmt, this->logicalLine,
                 type.typeName, typeName),
          true, __FILE__, __LINE__);
  }
  return false;
}

std::vector<std::string> splitTypeName(const std::string &typeName) {
  std::vector<std::string> parts;
  std::string current;
  int depth = 0;

  for (char c : typeName) {
    if (c == '<') {
      depth++;
      current += c;
    } else if (c == '>') {
      depth--;
      current += c;
    } else if (c == '.' && depth == 0) {
      if (!current.empty()) {
        parts.push_back(current);
        current.clear();
      }
    } else if (c == ',' && depth == 1) {
      parts.push_back(current);
      current.clear();
    } else {
      current += c;
    }
  }
  if (!current.empty()) parts.push_back(current);
  return parts;
}

gen::Type **gen::CodeGenerator::getType(std::string typeName,
                                        asmc::File &OutputFile) {
  gen::Type **type = this->typeList[typeName];
  if (type == nullptr) {
    // find . in the type name
    if (typeName.find('.') == std::string::npos)
      this->alert("Type " + typeName +
                      " not found in type list, did you forget to "
                      "include the file that defines it?",
                  true, __FILE__, __LINE__);

    auto typeNameParts = splitTypeName(typeName);
    auto typeNamePart = typeNameParts.front();

    // find the class of this type...
    auto cl = this->genericTypes[typeNamePart];

    if (cl == nullptr) {
      this->alert("Type " + typeNamePart +
                      " not found in type list, did you forget to "
                      "include the file that defines it?",
                  true, __FILE__, __LINE__);
    }

    if (cl->genericTypes.size() != typeNameParts.size() - 1) {
      this->alert("Type " + typeName + " has " +
                      std::to_string(cl->genericTypes.size()) +
                      " generic types, but " +
                      std::to_string(typeNameParts.size() - 1) +
                      " were provided. Please check the type definition.",
                  true, __FILE__, __LINE__);
    }
    // typeNameParts without the first parts
    auto typeNameWithoutFirstPart = typeNameParts;
    typeNameWithoutFirstPart.erase(typeNameWithoutFirstPart.begin());

    return this->instantiateGenericClass(cl, typeNameWithoutFirstPart, typeName,
                                         OutputFile);
  }
  return type;
}

asmc::File gen::CodeGenerator::memMove(std::string from, std::string to,
                                       int bytes) {
  asmc::File file;

  asmc::Push *pushRsi = new asmc::Push();
  pushRsi->op = this->registers["%rsi"]->get(asmc::QWord);
  pushRsi->size = asmc::QWord;
  pushRsi->logicalLine = this->logicalLine;
  file.text << pushRsi;

  asmc::Push *pushRdi = new asmc::Push();
  pushRdi->op = this->registers["%rdi"]->get(asmc::QWord);
  pushRdi->size = asmc::QWord;
  pushRdi->logicalLine = this->logicalLine;
  file.text << pushRdi;

  asmc::Push *pushRcx = new asmc::Push();
  pushRcx->op = this->registers["%rcx"]->get(asmc::QWord);
  pushRcx->size = asmc::QWord;
  pushRcx->logicalLine = this->logicalLine;
  file.text << pushRcx;

  asmc::Push *pushRax = new asmc::Push();
  pushRax->op = this->registers["%rax"]->get(asmc::QWord);
  pushRax->size = asmc::QWord;
  pushRax->logicalLine = this->logicalLine;
  file.text << pushRax;

  asmc::Mov *movFrom = new asmc::Mov();
  movFrom->from = from;
  movFrom->to = this->registers["%rsi"]->get(asmc::QWord);
  movFrom->size = asmc::QWord;
  movFrom->logicalLine = this->logicalLine;
  file.text << movFrom;

  asmc::Mov *movTo = new asmc::Mov();
  movTo->from = to;
  movTo->to = this->registers["%rdi"]->get(asmc::QWord);
  movTo->size = asmc::QWord;
  movTo->logicalLine = this->logicalLine;
  file.text << movTo;

  asmc::Mov *movCount = new asmc::Mov();
  movCount->from = "$" + std::to_string(bytes);
  movCount->to = this->registers["%rcx"]->get(asmc::QWord);
  movCount->size = asmc::QWord;
  movCount->logicalLine = this->logicalLine;
  file.text << movCount;

  std::string loopLabelStr = "L_" + gen::utils::generateUUID();
  asmc::Label *loopLabel = new asmc::Label();
  loopLabel->label = loopLabelStr;
  loopLabel->logicalLine = this->logicalLine;
  file.text << loopLabel;

  asmc::Mov *load = new asmc::Mov();
  load->from = '(' + this->registers["%rsi"]->get(asmc::QWord) + ')';
  load->to = this->registers["%al"]->get(asmc::Byte);
  load->size = asmc::Byte;
  load->logicalLine = this->logicalLine;
  file.text << load;

  asmc::Mov *store = new asmc::Mov();
  store->from = this->registers["%al"]->get(asmc::Byte);
  store->to = '(' + this->registers["%rdi"]->get(asmc::QWord) + ')';
  store->size = asmc::Byte;
  store->logicalLine = this->logicalLine;
  file.text << store;

  asmc::Add *incSrc = new asmc::Add();
  incSrc->op1 = "$1";
  incSrc->op2 = this->registers["%rsi"]->get(asmc::QWord);
  incSrc->size = asmc::QWord;
  incSrc->logicalLine = this->logicalLine;
  file.text << incSrc;

  asmc::Add *incDst = new asmc::Add();
  incDst->op1 = "$1";
  incDst->op2 = this->registers["%rdi"]->get(asmc::QWord);
  incDst->size = asmc::QWord;
  incDst->logicalLine = this->logicalLine;
  file.text << incDst;

  asmc::Sub *dec = new asmc::Sub();
  dec->op1 = "$1";
  dec->op2 = this->registers["%rcx"]->get(asmc::QWord);
  dec->size = asmc::QWord;
  dec->logicalLine = this->logicalLine;
  file.text << dec;

  asmc::Cmp *cmp = new asmc::Cmp();
  cmp->from = "$0";
  cmp->to = this->registers["%rcx"]->get(asmc::QWord);
  cmp->size = asmc::QWord;
  cmp->logicalLine = this->logicalLine;
  file.text << cmp;

  asmc::Jne *jne = new asmc::Jne();
  jne->to = loopLabelStr;
  jne->logicalLine = this->logicalLine;
  file.text << jne;

  asmc::Pop *popRax = new asmc::Pop();
  popRax->op = this->registers["%rax"]->get(asmc::QWord);
  popRax->size = asmc::QWord;
  popRax->logicalLine = this->logicalLine;
  file.text << popRax;

  asmc::Pop *popRcx = new asmc::Pop();
  popRcx->op = this->registers["%rcx"]->get(asmc::QWord);
  popRcx->size = asmc::QWord;
  popRcx->logicalLine = this->logicalLine;
  file.text << popRcx;

  asmc::Pop *popRdi = new asmc::Pop();
  popRdi->op = this->registers["%rdi"]->get(asmc::QWord);
  popRdi->size = asmc::QWord;
  popRdi->logicalLine = this->logicalLine;
  file.text << popRdi;

  asmc::Pop *popRsi = new asmc::Pop();
  popRsi->op = this->registers["%rsi"]->get(asmc::QWord);
  popRsi->size = asmc::QWord;
  popRsi->logicalLine = this->logicalLine;
  file.text << popRsi;

  return file;
}

asmc::File gen::CodeGenerator::setOffset(std::string to, int offset,
                                         std::string from, asmc::Size size) {
  asmc::File file;

  asmc::Push *pushRdi = new asmc::Push();
  pushRdi->op = this->registers["%rdi"]->get(asmc::QWord);
  pushRdi->size = asmc::QWord;
  pushRdi->logicalLine = this->logicalLine;
  file.text << pushRdi;

  asmc::Push *pushRax = new asmc::Push();
  pushRax->op = this->registers["%rax"]->get(asmc::QWord);
  pushRax->size = asmc::QWord;
  pushRax->logicalLine = this->logicalLine;
  file.text << pushRax;

  asmc::Mov *movPtr = new asmc::Mov();
  movPtr->from = to;
  movPtr->to = this->registers["%rdi"]->get(asmc::QWord);
  movPtr->size = asmc::QWord;
  movPtr->logicalLine = this->logicalLine;
  file.text << movPtr;

  if (offset != 0) {
    asmc::Add *addOffset = new asmc::Add();
    addOffset->op1 = "$" + std::to_string(offset);
    addOffset->op2 = this->registers["%rdi"]->get(asmc::QWord);
    addOffset->size = asmc::QWord;
    addOffset->logicalLine = this->logicalLine;
    file.text << addOffset;
  }

  asmc::Mov *movVal = new asmc::Mov();
  movVal->from = from;
  movVal->to = this->registers["%rax"]->get(size);
  movVal->size = size;
  movVal->logicalLine = this->logicalLine;
  file.text << movVal;

  asmc::Mov *store = new asmc::Mov();
  store->from = this->registers["%rax"]->get(size);
  store->to = '(' + this->registers["%rdi"]->get(asmc::QWord) + ')';
  store->size = size;
  store->logicalLine = this->logicalLine;
  file.text << store;

  asmc::Pop *popRax = new asmc::Pop();
  popRax->op = this->registers["%rax"]->get(asmc::QWord);
  popRax->size = asmc::QWord;
  popRax->logicalLine = this->logicalLine;
  file.text << popRax;

  asmc::Pop *popRdi = new asmc::Pop();
  popRdi->op = this->registers["%rdi"]->get(asmc::QWord);
  popRdi->size = asmc::QWord;
  popRdi->logicalLine = this->logicalLine;
  file.text << popRdi;

  return file;
}

bool gen::CodeGenerator::whenSatisfied(const ast::When &when) {
  auto evalPred = [this](const ast::WhenPredicat &pred) -> bool {
    if (pred.op == ast::WhenOperator::IS && pred.ident == "primitive") {
      bool isPrim = parse::PRIMITIVE_TYPES.find(pred.typeName) !=
                    parse::PRIMITIVE_TYPES.end();
      if (!pred.negated && !isPrim) return false;
      if (pred.negated && isPrim) return false;
      return true;
    } else if (pred.op == ast::WhenOperator::IS) {
      auto equal = pred.typeName == pred.ident;
      if (!pred.negated && !equal) return false;
      if (pred.negated && equal) return false;
      return true;
    } else if (pred.op == ast::WhenOperator::HAS) {
      gen::Type **type = this->typeList[pred.typeName];
      auto has = false;
      if (type != nullptr) {
        gen::Class *cl = dynamic_cast<gen::Class *>(*type);
        if (cl != nullptr) {
          has = cl->publicNameTable[pred.ident] != nullptr;
        }
      }
      if (!pred.negated && !has) return false;
      if (pred.negated && has) return false;
      return true;
    }
    return false;
  };

  if (when.predicates.empty()) return true;
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
