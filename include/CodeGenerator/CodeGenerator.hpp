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
};

class Symbol {
 public:
  std::string symbol;
  ast::Type type;
  bool mutable_ = true;
  bool mask;
  int byteMod;
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
  static bool compair(Type* t, std::string ident);
  virtual int poly() { return 0; };
  int size;
};

class Class : public Type {
 public:
  // The public Name table will hold all public functions
  // This is to be used externally
  links::SLinkedList<ast::Function, std::string> publicNameTable;
  // overload table
  links::SLinkedList<ast::Function, ast::Op> overloadTable;
  // The private Name table will hold all public and private functions
  // This is used internally
  links::SLinkedList<ast::Function, std::string> nameTable;
  ast::Statment* contract;
  std::vector<ast::DecAssign> defaultValues;
  static bool compair(Type* t, std::string ident);
  gen::Class* parent = nullptr;
};

class CodeGenerator {
 private:
#pragma region State Variables
  gen::Class* scope;
  ast::Type returnType;
  int scopePop = 0;
  int lablecount = 0;
  int intArgsCounter;
  int selectReg = 0;
  bool globalScope = true;
  bool inFunction = false;
  HashMap<ast::Statment*> includedMemo;
  HashMap<std::string> nameSpaceTable;
  ast::Function* currentFunction = nullptr;
#pragma endregion

#pragma region Item Lists
  links::SLinkedList<gen::Type*, std::string> typeList;
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

  void GenArgs(ast::Statment* STMT, asmc::File& OutputFile);
  ast::Function GenCall(ast::Call* call, asmc::File& OutputFile);
  Expr GenExpr(ast::Expr* expr, asmc::File& OutputFile,
               asmc::Size size = asmc::AUTO);
  gen::Expr prepareCompound(ast::Compound compound, asmc::File& OutputFile,
                            bool isDiv = false);
  gen::Expr genArithmatic(asmc::ArithInst*, ast::Compound compound,
                          asmc::File& OutputFile);
  bool canAssign(ast::Type type, std::string typeName, bool strict = false);
  std::string moduleId;
  std::tuple<std::string, gen::Symbol, bool, asmc::File> resolveSymbol(
      std::string ident, links::LinkedList<std::string> modList,
      asmc::File& OutputFile, links::LinkedList<ast::Expr*> indicies,
      bool internal = false);

  void genSequence(ast::Sequence* seq, asmc::File& OutputFile);
  void genFunction(ast::Function* func, asmc::File& OutputFile);
  void genDeclare(ast::Declare* dec, asmc::File& OutputFile);
  void genDecAssign(ast::DecAssign* decAssign, asmc::File& OutputFile);
  void genDecArr(ast::DecArr* dec, asmc::File& OutputFile);
  void genDecAssignArr(ast::DecAssignArr* decAssign, asmc::File& OutputFile);
  void genReturn(ast::Return* ret, asmc::File& OutputFile);
  void genAssign(ast::Assign* assign, asmc::File& OutputFile);
  void genPush(ast::Push* push, asmc::File& OutputFile);
  void genPull(ast::Pull* pull, asmc::File& OutputFile);
  void genIf(ast::If* ifStmt, asmc::File& OutputFile);

 public:
  asmc::File GenSTMT(ast::Statment* stmt);
  links::LinkedList<gen::Symbol> GenTable(
      ast::Statment* STMT, links::LinkedList<gen::Symbol>& table);
  // a function for warnings or errors
  void alert(std::string message, bool error = true);
  CodeGenerator(std::string moduleId);
  asmc::File deScope(gen::Symbol sym);
};
}  // namespace gen

#endif