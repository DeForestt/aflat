#include "CodeGenerator/CodeGenerator.hpp"

#include <execinfo.h>
#include <unistd.h>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
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
                                  const std::string &source)
    : parser(parser), source(source) {
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
                                   std::string fmt, bool strict) {
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

  alert(format("Type mismatch on line {}: " + fmt, this->logicalLine,
               type.typeName, typeName),
        true, __FILE__, __LINE__);
  return false;
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
      this->alert("Type " + typeName +
                      " not found in type list, did you forget to "
                      "include the file that defines it?",
                  true, __FILE__, __LINE__);
    }

    if (cl->genericTypes.size() != typeNameParts.size() - 1) {
      this->alert("Type " + typeName +
                      " not found in type list, did you forget to "
                      "include the file that defines it?",
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
