#ifndef GEN
#define GEN

#include <filesystem>
#include <set>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "ASM.hpp"
#include "CodeGenerator/Expr.hpp"
#include "CodeGenerator/Types.hpp"
#include "HashMap.hpp"
#include "LinkedList.hpp"
#include "LinkedListS.hpp"
#include "Parser/AST.hpp"
#include "Parser/AST/Statements/Match.hpp"
#include "Parser/Parser.hpp"
namespace gen {

class CodeGenerator {
 public:
  Type **getType(std::string ident, asmc::File &OutputFile);
#pragma region State Variables
  parse::Parser &parser;
  gen::Class *scope;
  ast::Match *matchScope = nullptr;
  ast::Type returnType;
  int scopePop = 0;
  int mutability = 0;
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
  std::set<std::string> generatedClassNames;
  ast::Function *currentFunction = nullptr;
  std::string lambdaReturns = "";
  asmc::Size lambdaSize = asmc::AUTO;
#pragma endregion

#pragma region Item Lists
  links::SLinkedList<gen::Type *, std::string> typeList;
  std::unordered_map<std::string, ast::Class *> genericTypes;
  links::SLinkedList<asmc::Register, std::string> registers;
  links::LinkedList<Symbol> SymbolTable;
  links::LinkedList<Symbol> GlobalSymbolTable;
  links::SLinkedList<ast::Function, std::string> nameTable;
  links::SLinkedList<ast::Function, std::string> genericFunctions;
  links::SLinkedList<ast::Type, std::string> TypeList;
  std::unordered_map<std::string, ast::Transform> transforms;
#pragma endregion

  struct EnvState {
    links::LinkedList<Symbol> SymbolTable;
    links::LinkedList<Symbol> GlobalSymbolTable;
    links::SLinkedList<ast::Function, std::string> nameTable;
    links::SLinkedList<ast::Function, std::string> genericFunctions;
    std::unordered_map<std::string, ast::Class *> genericTypes;
    HashMap<ast::Statement *> includedMemo;
    HashMap<ast::Statement *> includedClasses;
    HashMap<std::string> nameSpaceTable;
    std::unordered_map<std::string, std::string> genericTypeConversions;
    std::set<std::string> generatedFunctionNames;
    links::SLinkedList<gen::Type *, std::string> typeList;
    links::SLinkedList<ast::Type, std::string> TypeList;
    std::unordered_map<std::string, ast::Transform> transforms;
    gen::Class *scope;
    ast::Type returnType;
    bool inFunction;
    bool globalScope;
    std::string lambdaReturns;
    asmc::Size lambdaSize;
    int tempCount;
    ast::Function *currentFunction;
    ast::Match *matchScope;
  };

  std::vector<EnvState> envStack;

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
               asmc::Size size = asmc::AUTO, std::string typeHint = "");
  gen::Expr prepareCompound(ast::Compound compound, asmc::File &OutputFile,
                            bool isDiv = false);
  gen::Expr genArithmetic(asmc::ArithInst *, ast::Compound compound,
                          asmc::File &OutputFile);
  asmc::File memMove(std::string from, std::string to, int bytes);
  asmc::File setOffset(std::string to, int offset, std::string from,
                       asmc::Size size);
  ast::Expr *imply(ast::Expr *expr, std::string typeName);
  bool canAssign(ast::Type type, std::string typeName, std::string fmt,
                 bool strict = false, bool panic = true);

  std::string moduleId;
  std::string source;
  std::filesystem::path cwd;

  links::LinkedList<std::string> breakContext;
  links::LinkedList<std::string> continueContext;

  void pushEnv();
  void popEnv();

  std::tuple<std::string, gen::Symbol, bool, asmc::File, gen::Symbol *>
  resolveSymbol(std::string ident, links::LinkedList<std::string> modList,
                asmc::File &OutputFile, links::LinkedList<ast::Expr *> indicies,
                bool internal = false);

  asmc::File GenSTMT(ast::Statement *stmt);
  asmc::File ImportsOnly(ast::Statement *stmt);
  links::LinkedList<gen::Symbol> GenTable(
      ast::Statement *STMT, links::LinkedList<gen::Symbol> &table);
  bool whenSatisfied(const ast::When &when);
  // a function for warnings or errors
  void alert(std::string message, bool error = true, const char *file = nullptr,
             int line = 0);
  static void enableAlertTrace(bool enable);
  gen::Type **instantiateGenericClass(ast::Class *cls,
                                      const std::vector<std::string> &types,
                                      std::string &newName,
                                      asmc::File &OutputFile);
  CodeGenerator(std::string moduleId, parse::Parser &parser,
                const std::string &source = "", const std::string &cwd = "");
  asmc::File *deScope(gen::Symbol &sym);
  bool hasError() const { return errorFlag; }

 private:
  static bool traceAlert;
};
}  // namespace gen

#endif
