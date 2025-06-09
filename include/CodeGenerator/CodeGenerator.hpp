#ifndef GEN
#define GEN

#include <optional>
#include <set>
#include <tuple>
#include <unordered_map>

#include "ASM.hpp"
#include "CodeGenerator/Expr.hpp"
#include "HashMap.hpp"
#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
namespace gen {

class Symbol {
 public:
  std::string symbol;
  ast::Type type;
  bool mutable_ = true;
  bool readOnly = false;
  bool mask = false;
  int byteMod;
  bool usable = true;
  int sold = -1;
  int underscores = 0;
  int refCount = 0;
  int assignCount = 0;
};

class Type {
 public:
  std::string Ident;

  // The public Symbol table will hold all public symbols
  // This is to be used externally
  links::LinkedList<Symbol> publicSymbols;

  // The private Symbol table will hold all public and private symbols
  // This is used internally
  links::LinkedList<Symbol> SymbolTable;
  static bool compare(Type *t, std::string ident);
  virtual int poly() { return 0; };
  int size;
};

class Class : public Type {
 public:
  bool dynamic = false;
  bool safeType =
      false;  // if true, this class cannot be passed as an argument.
  bool pedantic = false;  // if true, this class cannot be assigned to a void
                          // pointer (`adr`)
  // The public Name table will hold all public functions
  // This is to be used externally
  links::SLinkedList<ast::Function, std::string> publicNameTable;
  // overload table
  links::SLinkedList<ast::Function, ast::Op> overloadTable;
  // The private Name table will hold all public and private functions
  // This is used internally
  links::SLinkedList<ast::Function, std::string> nameTable;
  ast::Statement *contract;
  ast::Statement *body;
  std::vector<ast::DecAssign> defaultValues;
  static bool compare(Type *t, std::string ident);
  gen::Class *parent = nullptr;
};

class Enum : public Type {
 public:
  struct EnumValue {
   public:
    std::string name;
    int value;
    EnumValue() = default;
    EnumValue(std::string name, int value) : name(name), value(value){};
  };

  Enum();

  links::SLinkedList<EnumValue, std::string> values;
  static bool compare(Type *t, std::string ident);
  static bool compareEnum(EnumValue e, std::string ident);
};

class CodeGenerator {
 public:
#pragma region State Variables
  parse::Parser &parser;
  gen::Class *scope;
  ast::Type returnType;
  int scopePop = 0;
  int labelCount = 0;
  int intArgsCounter;
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
  ast::Function *currentFunction = nullptr;
  std::string lambdaReturns = "";
  asmc::Size lambdaSize = asmc::AUTO;
#pragma endregion

#pragma region Item Lists
  links::SLinkedList<gen::Type *, std::string> typeList;
  links::SLinkedList<ast::Class *, std::string> genericTypes;
  links::SLinkedList<asmc::Register, std::string> registers;
  links::LinkedList<Symbol> SymbolTable;
  links::LinkedList<Symbol> GlobalSymbolTable;
  links::SLinkedList<ast::Function, std::string> nameTable;
  links::SLinkedList<ast::Function, std::string> genericFunctions;
  links::SLinkedList<ast::Type, std::string> TypeList;
  std::unordered_map<std::string, ast::Transform> transforms;
#pragma endregion

  int getBytes(asmc::Size size);

  asmc::Register intArgs[6] = {asmc::Register("rdi", "edi", "di", "dil"),
                               asmc::Register("rsi", "esi", "si", "sil"),
                               asmc::Register("rdx", "edx", "dx", "dl"),
                               asmc::Register("rcx", "ecx", "cx", "cl"),
                               asmc::Register("r8", "r8d", "r8w", "r8b"),
                               asmc::Register("r9", "r9d", "r9w", "r9b")};
  int logicalLine = 0;
  int tempCount = 0;
  asmc::File GenArgs(ast::Statement *STMT, asmc::File &OutputFile,
                     const ast::Function &func, int &index);
  ast::Function GenCall(ast::Call *call, asmc::File &OutputFile);
  Expr GenExpr(ast::Expr *expr, asmc::File &OutputFile,
               asmc::Size size = asmc::AUTO);
  gen::Expr prepareCompound(ast::Compound compound, asmc::File &OutputFile,
                            bool isDiv = false);
  gen::Expr genArithmetic(asmc::ArithInst *, ast::Compound compound,
                          asmc::File &OutputFile);
  ast::Expr *imply(ast::Expr *expr, std::string typeName);
  bool canAssign(ast::Type type, std::string typeName, std::string fmt,
                 bool strict = false);

  std::string moduleId;
  std::string source;

  links::LinkedList<std::string> breakContext;
  links::LinkedList<std::string> continueContext;

  std::tuple<std::string, gen::Symbol, bool, asmc::File, gen::Symbol *>
  resolveSymbol(std::string ident, links::LinkedList<std::string> modList,
                asmc::File &OutputFile, links::LinkedList<ast::Expr *> indicies,
                bool internal = false);

  asmc::File GenSTMT(ast::Statement *stmt);
  asmc::File ImportsOnly(ast::Statement *stmt);
  links::LinkedList<gen::Symbol> GenTable(
      ast::Statement *STMT, links::LinkedList<gen::Symbol> &table);
  // a function for warnings or errors
  void alert(std::string message, bool error = true);
  CodeGenerator(std::string moduleId, parse::Parser &parser,
                const std::string &source = "");
  asmc::File *deScope(gen::Symbol &sym);
  bool hasError() const { return errorFlag; }
};
}  // namespace gen

#endif
