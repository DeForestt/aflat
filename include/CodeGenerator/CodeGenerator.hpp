#ifndef GEN
#define GEN

#include <tuple>

#include "ASM.hpp"
#include "HashMap.hpp"
#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"
namespace gen {

class Expr {
 public:
  asmc::OpType op = asmc::Hard;
  std::string access;
  std::string type = "";
  asmc::Size size;
  bool passable = true;
};

class Symbol {
 public:
  std::string symbol;
  ast::Type type;
  bool mutable_ = true;
  bool readOnly = false;
  bool mask;
  int byteMod;
  bool usable = true;
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
  // The public Name table will hold all public functions
  // This is to be used externally
  links::SLinkedList<ast::Function, std::string> publicNameTable;
  // overload table
  links::SLinkedList<ast::Function, ast::Op> overloadTable;
  // The private Name table will hold all public and private functions
  // This is used internally
  links::SLinkedList<ast::Function, std::string> nameTable;
  ast::Statement *contract;
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
 protected:
#pragma region State Variables
  gen::Class *scope;
  ast::Type returnType;
  int scopePop = 0;
  int labelCount = 0;
  int intArgsCounter;
  int selectReg = 0;
  bool globalScope = true;
  bool inFunction = false;
  bool returnFlag = false;
  HashMap<ast::Statement *> includedMemo;
  HashMap<ast::Statement *> includedClasses;
  HashMap<std::string> nameSpaceTable;
  ast::Function *currentFunction = nullptr;
  std::string lambdaReturns = "";
  asmc::Size lambdaSize = asmc::AUTO;
#pragma endregion

#pragma region Item Lists
  links::SLinkedList<gen::Type *, std::string> typeList;
  links::SLinkedList<asmc::Register, std::string> registers;
  links::LinkedList<Symbol> SymbolTable;
  links::LinkedList<Symbol> GlobalSymbolTable;
  links::SLinkedList<ast::Function, std::string> nameTable;
  links::SLinkedList<ast::Type, std::string> TypeList;
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
  void GenArgs(ast::Statement *STMT, asmc::File &OutputFile);
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

  links::LinkedList<std::string> breakContext;
  links::LinkedList<std::string> continueContext;

  std::tuple<std::string, gen::Symbol, bool, asmc::File> resolveSymbol(
      std::string ident, links::LinkedList<std::string> modList,
      asmc::File &OutputFile, links::LinkedList<ast::Expr *> indicies,
      bool internal = false);

  void genSequence(ast::Sequence *seq, asmc::File &OutputFile);
  void genFunction(ast::Function *func, asmc::File &OutputFile);
  void genDeclare(ast::Declare *dec, asmc::File &OutputFile);
  void genDecAssign(ast::DecAssign *decAssign, asmc::File &OutputFile);
  void genDecArr(ast::DecArr *dec, asmc::File &OutputFile);
  void genDecAssignArr(ast::DecAssignArr *decAssign, asmc::File &OutputFile);
  void genReturn(ast::Return *ret, asmc::File &OutputFile);
  void genAssign(ast::Assign *assign, asmc::File &OutputFile);
  void genPush(ast::Push *push, asmc::File &OutputFile);
  void genPull(ast::Pull *pull, asmc::File &OutputFile);
  void genIf(ast::If *ifStmt, asmc::File &OutputFile);
  void genWhile(ast::While *loop, asmc::File &OutputFile);
  void genFor(ast::For *loop, asmc::File &OutputFile);
  void genUDef(ast::Struct *udef, asmc::File &OutputFile);
  void genClass(ast::Class *deff, asmc::File &OutputFile);
  void genEnum(ast::Enum *deff, asmc::File &OutputFile);
  void genInc(ast::Inc *inc, asmc::File &OutputFile);
  void genDec(ast::Dec *inc, asmc::File &OutputFile);
  void genImport(ast::Import *imp, asmc::File &OutputFile);
  void genDelete(ast::Delete *del, asmc::File &OutputFile);
  void genBreak(ast::Break *brk, asmc::File &OutputFile);
  void genContinue(ast::Continue *cont, asmc::File &OutputFile);

 public:
  asmc::File GenSTMT(ast::Statement *stmt);
  asmc::File ImportsOnly(ast::Statement *stmt);
  links::LinkedList<gen::Symbol> GenTable(
      ast::Statement *STMT, links::LinkedList<gen::Symbol> &table);
  // a function for warnings or errors
  void alert(std::string message, bool error = true);
  CodeGenerator(std::string moduleId);
  asmc::File *deScope(gen::Symbol &sym);
};
}  // namespace gen

#endif