#ifndef GEN
#define GEN

#include <array>
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

  CodeGenerator(std::string moduleId, parse::Parser &parser,
                const std::string &source = "", const std::string &cwd = "");
  ~CodeGenerator();
  CodeGenerator(CodeGenerator &&) noexcept;
  CodeGenerator &operator=(CodeGenerator &&) noexcept;
  CodeGenerator(const CodeGenerator &) = delete;
  CodeGenerator &operator=(const CodeGenerator &) = delete;

  Type **getType(std::string ident, asmc::File &OutputFile);
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

  void pushEnv();
  void popEnv();

  std::tuple<std::string, gen::Symbol, bool, asmc::File, gen::Symbol *>
  resolveSymbol(std::string ident, links::LinkedList<std::string> modList,
                asmc::File &OutputFile, links::LinkedList<ast::Expr *> indicies,
                bool internal = false);

  asmc::File GenSTMT(ast::Statement *stmt);
  asmc::File ImportsOnly(ast::Statement *stmt);
  links::LinkedList<gen::Symbol>
  GenTable(ast::Statement *STMT, links::LinkedList<gen::Symbol> &table);
  bool whenSatisfied(const ast::When &when);
  void alert(std::string message, bool error = true, const char *file = nullptr,
             int line = 0);
  static void enableAlertTrace(bool enable);
  gen::Type **instantiateGenericClass(ast::Class *cls,
                                      const std::vector<std::string> &types,
                                      std::string &newName,
                                      asmc::File &OutputFile);
  asmc::File *deScope(gen::Symbol &sym);
  bool hasError() const;

  parse::Parser &parser();
  const parse::Parser &parser() const;
  gen::Class *&scope();
  const gen::Class *scope() const;
  ast::Match *&matchScope();
  const ast::Match *matchScope() const;
  ast::Type &returnType();
  const ast::Type &returnType() const;
  int &scopePop();
  const int &scopePop() const;
  int &mutability();
  const int &mutability() const;
  int &labelCount();
  const int &labelCount() const;
  int &intArgsCounter();
  const int &intArgsCounter() const;
  int &selectReg();
  const int &selectReg() const;
  bool &globalScope();
  const bool &globalScope() const;
  bool &inFunction();
  const bool &inFunction() const;
  bool &returnFlag();
  const bool &returnFlag() const;
  bool &errorFlag();
  const bool &errorFlag() const;
  HashMap<ast::Statement *> &includedMemo();
  HashMap<ast::Statement *> &includedClasses();
  HashMap<std::string> &nameSpaceTable();
  std::unordered_map<std::string, std::string> &genericTypeConversions();
  std::set<std::string> &generatedFunctionNames();
  const std::set<std::string> &generatedFunctionNames() const;
  std::set<std::string> &generatedClassNames();
  ast::Function *&currentFunction();
  const ast::Function *currentFunction() const;
  std::string &lambdaReturns();
  const std::string &lambdaReturns() const;
  asmc::Size &lambdaSize();
  const asmc::Size &lambdaSize() const;
  links::SLinkedList<gen::Type *, std::string> &typeList();
  const links::SLinkedList<gen::Type *, std::string> &typeList() const;
  std::unordered_map<std::string, ast::Class *> &genericTypes();
  const std::unordered_map<std::string, ast::Class *> &genericTypes() const;
  links::SLinkedList<asmc::Register, std::string> &registers();
  const links::SLinkedList<asmc::Register, std::string> &registers() const;
  links::LinkedList<Symbol> &SymbolTable();
  const links::LinkedList<Symbol> &SymbolTable() const;
  links::LinkedList<Symbol> &GlobalSymbolTable();
  const links::LinkedList<Symbol> &GlobalSymbolTable() const;
  links::SLinkedList<ast::Function, std::string> &nameTable();
  const links::SLinkedList<ast::Function, std::string> &nameTable() const;
  links::SLinkedList<ast::Function, std::string> &genericFunctions();
  const links::SLinkedList<ast::Function, std::string> &
  genericFunctions() const;
  links::SLinkedList<ast::Type, std::string> &TypeList();
  const links::SLinkedList<ast::Type, std::string> &TypeList() const;
  std::unordered_map<std::string, ast::Transform> &transforms();
  const std::unordered_map<std::string, ast::Transform> &transforms() const;
  std::vector<EnvState> &envStack();
  const std::vector<EnvState> &envStack() const;
  std::array<asmc::Register, 6> &intArgs();
  const std::array<asmc::Register, 6> &intArgs() const;
  int &logicalLine();
  const int &logicalLine() const;
  int &tempCount();
  const int &tempCount() const;
  std::filesystem::path &cwd();
  const std::filesystem::path &cwd() const;
  std::string &moduleId();
  const std::string &moduleId() const;
  std::string &source();
  const std::string &source() const;
  links::LinkedList<std::string> &breakContext();
  const links::LinkedList<std::string> &breakContext() const;
  links::LinkedList<std::string> &continueContext();
  const links::LinkedList<std::string> &continueContext() const;

private:
  static bool traceAlert;
  struct Impl;
  std::unique_ptr<Impl> impl;
};
} // namespace gen

#endif
