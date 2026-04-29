#include "Parser/AST/Statements/Import.hpp"
#include <algorithm>

#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <utility>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Lower.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"

namespace ast {

namespace {

class NamespaceTableGuard {
public:
  explicit NamespaceTableGuard(gen::CodeGenerator &generator)
      : generator(generator), snapshot(generator.nameSpaceTable()) {}

  ~NamespaceTableGuard() { generator.nameSpaceTable() = snapshot; }

private:
  gen::CodeGenerator &generator;
  HashMap<std::string> snapshot;
};

class CwdGuard {
public:
  explicit CwdGuard(gen::CodeGenerator &generator)
      : generator(generator), snapshot(generator.cwd()) {}

  ~CwdGuard() { generator.cwd() = snapshot; }

private:
  gen::CodeGenerator &generator;
  std::filesystem::path snapshot;
};

class ActiveImportGuard {
public:
  ActiveImportGuard(gen::CodeGenerator &generator, std::string path)
      : generator(generator), path(std::move(path)),
        active(generator.activeImports().find(this->path) ==
               generator.activeImports().end()) {
    if (active)
      generator.activeImports().insert(this->path);
  }

  ~ActiveImportGuard() {
    if (active)
      generator.activeImports().erase(path);
  }

  bool isActive() const { return active; }

private:
  gen::CodeGenerator &generator;
  std::string path;
  bool active = false;
};

} // namespace

static void
collectImportNamespacesImpl(ast::Statement *stmt,
                            std::unordered_map<std::string, std::string> &map) {
  if (!stmt)
    return;
  if (auto seq = dynamic_cast<ast::Sequence *>(stmt)) {
    collectImportNamespacesImpl(seq->Statement1, map);
    collectImportNamespacesImpl(seq->Statement2, map);
  } else if (auto imp = dynamic_cast<ast::Import *>(stmt)) {
    std::string id = imp->path;
    if (id.find_last_of('/') != std::string::npos)
      id = id.substr(id.find_last_of('/') + 1);
    if (id.find_last_of('.') != std::string::npos)
      id = id.substr(0, id.find_last_of('.'));
    map[imp->nameSpace] = id;
  }
}

static void
collectFunctionNamesImpl(ast::Statement *stmt,
                         std::unordered_map<std::string, std::string> &map) {
  if (!stmt)
    return;
  if (auto seq = dynamic_cast<ast::Sequence *>(stmt)) {
    collectFunctionNamesImpl(seq->Statement1, map);
    collectFunctionNamesImpl(seq->Statement2, map);
  } else if (auto func = dynamic_cast<ast::Function *>(stmt)) {
    map.emplace(func->ident.ident, "");
  }
}

static void registerClassShells(ast::Statement *stmt,
                                gen::CodeGenerator &generator) {
  if (stmt == nullptr)
    return;
  if (auto seq = dynamic_cast<ast::Sequence *>(stmt)) {
    registerClassShells(seq->Statement1, generator);
    registerClassShells(seq->Statement2, generator);
    return;
  }
  if (auto cls = dynamic_cast<ast::Class *>(stmt)) {
    if (generator.typeList()[cls->ident.ident] != nullptr)
      return;

    auto *type = new gen::Class();
    type->Ident = cls->ident.ident;
    type->hidden = cls->hidden;
    type->safeType = cls->safeType;
    type->dynamic = cls->dynamic;
    type->pedantic = cls->pedantic;
    type->uniqueType = cls->uniqueType;
    type->declarationOnly = true;
    type->contract = cls->contract;
    type->body = cls->statement;

    auto prevScope = generator.scope();
    auto prevGlobal = generator.globalScope();
    generator.scope() = type;
    generator.globalScope() = false;
    generator.typeList().push(type);

    links::LinkedList<gen::Symbol> table;
    auto collect = [&](auto &&self, ast::Statement *node) -> void {
      if (node == nullptr)
        return;
      if (auto innerSeq = dynamic_cast<ast::Sequence *>(node)) {
        self(self, innerSeq->Statement1);
        self(self, innerSeq->Statement2);
        return;
      }

      std::string ident;
      ast::Type typeInfo;
      int count = 1;
      bool shouldCollect = false;

      if (auto dec = dynamic_cast<ast::Declare *>(node)) {
        ident = dec->ident;
        typeInfo = dec->type;
        count = 1;
        shouldCollect = true;
      } else if (auto decArr = dynamic_cast<ast::DecArr *>(node)) {
        ident = decArr->ident;
        typeInfo = decArr->type;
        count = decArr->count;
        shouldCollect = true;
      } else if (auto decAssign = dynamic_cast<ast::DecAssign *>(node)) {
        ident = decAssign->declare->ident;
        typeInfo = decAssign->declare->type;
        count = 1;
        shouldCollect = true;
      } else if (auto decAssignArr = dynamic_cast<ast::DecAssignArr *>(node)) {
        ident = decAssignArr->declare->ident;
        typeInfo = decAssignArr->declare->type;
        count = decAssignArr->declare->count;
        shouldCollect = true;
      }

      if (!shouldCollect || ident.empty())
        return;

      if (table.search<std::string>(gen::utils::searchSymbol, ident) != nullptr)
        return;

      gen::Symbol symbol;
      symbol.symbol = ident;
      symbol.type = typeInfo;
      symbol.mutable_ = true;
      symbol.readOnly = false;
      const int bytes = gen::utils::sizeToInt(typeInfo.size) *
                        std::max(1, count) * std::max(1, typeInfo.arraySize);
      int alignment =
          std::max(1, std::min(gen::utils::sizeToInt(typeInfo.size), 8));
      int current = table.head == nullptr ? 0 : table.peek().byteMod;
      symbol.byteMod = gen::utils::alignTo(current, alignment) + bytes;
      table.push(symbol);
    };

    collect(collect, cls->statement);
    type->SymbolTable = table;
    type->publicSymbols = table;

    generator.scope() = prevScope;
    generator.globalScope() = prevGlobal;
  }
}

void collectImportNamespaces(
    ast::Statement *stmt, std::unordered_map<std::string, std::string> &map) {
  collectImportNamespacesImpl(stmt, map);
}
Import::Import(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->logicalLine = tokens.peek()->lineCount;
  while (true) {
    auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (sym != nullptr && sym->Sym == '{') {
      this->hasFunctions = true;
      do {
        tokens.pop();
        auto importObj = dynamic_cast<lex::LObj *>(tokens.pop());
        if (importObj != nullptr) {
          this->imports.push_back(importObj->meta);
        } else {
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) +
              " Expected Ident");
        }
      } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
               dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
          dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '}') {
        tokens.pop();
      } else {
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Expected }");
      }
    } else if (sym != nullptr && sym->Sym == '*') {
      this->hasFunctions = true;
      tokens.pop();
      this->imports.push_back("*");
    } else {
      auto nt = dynamic_cast<lex::LObj *>(tokens.peek());
      if (nt != nullptr) {
        this->hasClasses = true;
        this->imports.push_back(nt->meta);
        auto t = ast::Type();
        t.size = asmc::QWord;
        t.typeName = nt->meta;
        t.opType = asmc::Hard;
        parser.getTypeList() << t;
        tokens.pop();
      } else {
        break;
      }
    }

    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
        dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',') {
      tokens.pop();
      continue;
    }
    break;
  }

  // check from from keyword
  lex::LObj *from = dynamic_cast<lex::LObj *>(tokens.pop());
  if (from == nullptr || from->meta != "from") {
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Expected from");
  };

  ast::StringLiteral *str =
      dynamic_cast<ast::StringLiteral *>(parser.parseExpr(tokens));
  if (str != nullptr) {
    this->path = str->val;
  } else {
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Expected StringLiteral");
  }

  // check for under keyword
  auto under = dynamic_cast<lex::LObj *>(tokens.peek());
  if (under != nullptr) {
    if (under->meta == "under") {
      tokens.pop();
      auto ident = dynamic_cast<lex::LObj *>(tokens.peek());
      if (ident != nullptr) {
        this->nameSpace = ident->meta;
        tokens.pop();
      } else {
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Expected Ident");
      }
    }
  } else {
    std::string id = this->path.substr(this->path.find_last_of('/') + 1,
                                       this->path.find_last_of('.'));
    this->nameSpace = id;
  }
}

gen::GenerationResult const Import::generate(gen::CodeGenerator &generator) {
  auto OutputFile = asmc::File();
  std::filesystem::path importPath = this->path;
  if (importPath.is_relative()) {
    if (!this->path.empty() && this->path[0] == '.') {
      importPath = generator.cwd() / importPath;
    } else {
      importPath = gen::utils::getLibPath("src") / importPath;
    }
  }
  if (importPath.extension() != ".af")
    importPath += ".af";
  importPath = std::filesystem::absolute(importPath).lexically_normal();
  this->path = importPath.string();
  this->cwd = importPath.parent_path().string();

  std::string id = this->path.substr(this->path.find_last_of("/") + 1);
  // remove the .af extension
  id = id.substr(0, id.find_last_of("."));
  ast::Statement *added;
  std::ifstream file(this->path);
  if (!file.is_open()) {
    this->path = this->path.substr(0, this->path.find_last_of(".")) + "/mod.af";
    file.open(this->path);
    if (!file.is_open()) {
      generator.alert("File " + this->path + " not found");
      return {OutputFile, std::nullopt};
    }
    this->cwd = std::filesystem::path(this->path).parent_path().string();
  }

  if (generator.activeImports().find(this->path) !=
      generator.activeImports().end()) {
    if (generator.includedMemo().contains(this->path))
      registerClassShells(generator.includedMemo().get(this->path), generator);
    return {OutputFile, std::nullopt};
  }
  ActiveImportGuard activeGuard(generator, this->path);
  CwdGuard cwdGuard(generator);
  generator.cwd() = std::filesystem::path(this->path).parent_path();

  if (generator.includedMemo().contains(this->path))
    added = generator.includedMemo().get(this->path);
  else {

    std::string text = std::string((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
    lex::Lexer l = lex::Lexer();
    PreProcessor pp = PreProcessor();

    auto tokens = l.Scan(pp.PreProcess(text, gen::utils::getLibPath("head"),
                                       generator.cwd().string()));
    tokens.invert();
    // parse the file
    parse::Parser p = parse::Parser();
    ast::Statement *statement = p.parseStmt(tokens);
    auto Lowerer = parse::lower::Lowerer(statement);
    added = statement;
    generator.includedMemo().insert(this->path, added);
  }
  {
    NamespaceTableGuard namespaceGuard(generator);
    generator.ImportsOnly(added, true);
    std::unordered_map<std::string, std::string> nsMap;
    collectImportNamespaces(added, nsMap);
    for (std::string ident : this->imports) {
      if (ident == "*") {
        auto seq = dynamic_cast<ast::Sequence *>(added);
        if (seq != nullptr) {
          auto allStmts = gen::utils::extractAllFunctions(added, id);
          std::unordered_map<std::string, std::string> functionNsMap;
          collectFunctionNamesImpl(added, functionNsMap);
          for (const auto &[name, _] : functionNsMap) {
            nsMap[name] = id;
          }
          allStmts->namespaceSwap(nsMap);
          OutputFile << generator.GenSTMT(allStmts);
        }
        continue;
      }
      if (generator.includedClasses().contains(id + "::" + ident))
        continue;
      generator.includedClasses().insert(id + "::" + ident, nullptr);
      auto allStmts = gen::utils::extractAll(ident, added, id);
      if (allStmts.size() <= 0) {
        generator.alert("Identifier " + ident + " not found to import");
      };

      for (auto stmt : allStmts) {
        stmt->namespaceSwap(nsMap);
        OutputFile << generator.GenSTMT(stmt);
      }
    }
  }
  if (this->hasFunctions)
    generator.nameSpaceTable().insert(this->nameSpace, id);
  return {OutputFile, std::nullopt};
}

gen::GenerationResult const
Import::generateClasses(gen::CodeGenerator &generator) {
  auto OutputFile = asmc::File();
  std::filesystem::path importPath = this->path;
  if (importPath.is_relative()) {
    if (!this->path.empty() && this->path[0] == '.') {
      importPath = generator.cwd() / importPath;
    } else {
      importPath = gen::utils::getLibPath("src") / importPath;
    }
  }
  if (importPath.extension() != ".af")
    importPath += ".af";
  importPath = std::filesystem::absolute(importPath).lexically_normal();
  this->path = importPath.string();
  this->cwd = importPath.parent_path().string();

  std::string id = this->path.substr(this->path.find_last_of("/") + 1);
  id = id.substr(0, id.find_last_of("."));
  ast::Statement *added;
  std::ifstream file(this->path);
  if (!file.is_open()) {
    this->path = this->path.substr(0, this->path.find_last_of(".")) + "/mod.af";
    file.open(this->path);
    if (!file.is_open()) {
      generator.alert("File " + this->path + " not found");
      return {OutputFile, std::nullopt};
    }
    this->cwd = std::filesystem::path(this->path).parent_path().string();
  }

  if (generator.activeImports().find(this->path) !=
      generator.activeImports().end()) {
    if (generator.includedMemo().contains(this->path))
      registerClassShells(generator.includedMemo().get(this->path), generator);
    return {OutputFile, std::nullopt};
  }
  ActiveImportGuard activeGuard(generator, this->path);
  CwdGuard cwdGuard(generator);
  generator.cwd() = std::filesystem::path(this->path).parent_path();

  if (generator.includedMemo().contains(this->path))
    added = generator.includedMemo().get(this->path);
  else {

    std::string text = std::string((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
    lex::Lexer l = lex::Lexer();
    PreProcessor pp = PreProcessor();

    auto tokens = l.Scan(pp.PreProcess(text, gen::utils::getLibPath("head"),
                                       generator.cwd().string()));
    tokens.invert();
    parse::Parser p = parse::Parser();
    if (this->path.find("./") != std::string::npos)
      p.setMutability(generator.mutability());
    ast::Statement *statement = p.parseStmt(tokens);
    auto Lowerer = parse::lower::Lowerer(statement);
    added = statement;
    generator.includedMemo().insert(this->path, added);
    generator.ImportsOnly(added, false);
  }

  std::unordered_map<std::string, std::string> nsMap;
  collectImportNamespaces(added, nsMap);

  for (std::string ident : this->imports) {
    ast::Statement *statement = gen::utils::extract(ident, added, id);
    if (statement == nullptr)
      continue;
    if (dynamic_cast<ast::Class *>(statement) == nullptr &&
        dynamic_cast<ast::Enum *>(statement) == nullptr &&
        dynamic_cast<ast::Transform *>(statement) == nullptr)
      continue;
    if (generator.includedClasses().contains(id + "::" + ident))
      continue;
    generator.includedClasses().insert(id + "::" + ident, nullptr);
    statement->namespaceSwap(nsMap);
    OutputFile << generator.GenSTMT(statement);
  }

  return {OutputFile, std::nullopt};
}
} // namespace ast
