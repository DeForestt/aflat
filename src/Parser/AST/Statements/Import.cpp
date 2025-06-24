#include "Parser/AST/Statements/Import.hpp"

#include <filesystem>
#include <fstream>
#include <unordered_map>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Lower.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"

namespace ast {

static void collectImportNamespacesImpl(
    ast::Statement *stmt, std::unordered_map<std::string, std::string> &map) {
  if (!stmt) return;
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
        parser.typeList << t;
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
      importPath = generator.cwd / importPath;
    } else {
      importPath = gen::utils::getLibPath("src") / importPath;
    }
  }
  if (importPath.extension() != ".af") importPath += ".af";
  importPath = std::filesystem::absolute(importPath).lexically_normal();
  this->path = importPath.string();
  this->cwd = importPath.parent_path().string();
  auto prevCwd = generator.cwd;
  generator.cwd = importPath.parent_path();

  std::string id = this->path.substr(this->path.find_last_of("/") + 1);
  // remove the .af extension
  id = id.substr(0, id.find_last_of("."));
  ast::Statement *added;
  if (generator.includedMemo.contains(this->path))
    added = generator.includedMemo.get(this->path);
  else {
    // scan the file
    std::ifstream file(this->path);
    // check if the file exists
    if (!file.is_open()) {
      // switch the filename path to be mod.af
      this->path =
          this->path.substr(0, this->path.find_last_of(".")) + "/mod.af";
      file.open(this->path);
      if (!file.is_open()) {
        generator.alert("File " + this->path + " not found");
        generator.cwd = prevCwd;
        return {OutputFile, std::nullopt};
      }
      this->cwd = std::filesystem::path(this->path).parent_path().string();
      generator.cwd = std::filesystem::path(this->path).parent_path();
    }

    std::string text = std::string((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
    lex::Lexer l = lex::Lexer();
    PreProcessor pp = PreProcessor();

    auto tokens = l.Scan(pp.PreProcess(text, gen::utils::getLibPath("head"),
                                       generator.cwd.string()));
    tokens.invert();
    // parse the file
    parse::Parser p = parse::Parser();
    ast::Statement *statement = p.parseStmt(tokens);
    auto Lowerer = parse::lower::Lowerer(statement);
    added = statement;
    generator.includedMemo.insert(this->path, added);
    generator.ImportsOnly(added);
  }
  std::unordered_map<std::string, std::string> nsMap;
  collectImportNamespaces(added, nsMap);
  for (std::string ident : this->imports) {
    if (generator.includedClasses.contains(id + "::" + ident)) continue;
    generator.includedClasses.insert(id + "::" + ident, nullptr);
    ast::Statement *statement = gen::utils::extract(ident, added, id);
    if (statement == nullptr) {
      generator.alert("Identifier " + ident + " not found to import");
    } else {
      statement->namespaceSwap(nsMap);
      OutputFile << generator.GenSTMT(statement);
    }
  }
  if (this->hasFunctions) generator.nameSpaceTable.insert(this->nameSpace, id);
  generator.cwd = prevCwd;
  return {OutputFile, std::nullopt};
}

gen::GenerationResult const Import::generateClasses(
    gen::CodeGenerator &generator) {
  auto OutputFile = asmc::File();
  std::filesystem::path importPath = this->path;
  if (importPath.is_relative()) {
    if (!this->path.empty() && this->path[0] == '.') {
      importPath = generator.cwd / importPath;
    } else {
      importPath = gen::utils::getLibPath("src") / importPath;
    }
  }
  if (importPath.extension() != ".af") importPath += ".af";
  importPath = std::filesystem::absolute(importPath).lexically_normal();
  this->path = importPath.string();
  this->cwd = importPath.parent_path().string();
  auto prevCwd = generator.cwd;
  generator.cwd = importPath.parent_path();

  std::string id = this->path.substr(this->path.find_last_of("/") + 1);
  id = id.substr(0, id.find_last_of("."));
  ast::Statement *added;
  if (generator.includedMemo.contains(this->path))
    added = generator.includedMemo.get(this->path);
  else {
    std::ifstream file(this->path);
    if (!file.is_open()) {
      this->path =
          this->path.substr(0, this->path.find_last_of(".")) + "/mod.af";
      file.open(this->path);
      if (!file.is_open()) {
        generator.alert("File " + this->path + " not found");
        generator.cwd = prevCwd;
        return {OutputFile, std::nullopt};
      }
      this->cwd = std::filesystem::path(this->path).parent_path().string();
      generator.cwd = std::filesystem::path(this->path).parent_path();
    }

    std::string text = std::string((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
    lex::Lexer l = lex::Lexer();
    PreProcessor pp = PreProcessor();

    auto tokens = l.Scan(pp.PreProcess(text, gen::utils::getLibPath("head"),
                                       generator.cwd.string()));
    tokens.invert();
    parse::Parser p = parse::Parser();
    if (this->path.find("./") != std::string::npos)
      p.mutability = generator.mutability;
    ast::Statement *statement = p.parseStmt(tokens);
    auto Lowerer = parse::lower::Lowerer(statement);
    added = statement;
    generator.includedMemo.insert(this->path, added);
    generator.ImportsOnly(added);
  }

  std::unordered_map<std::string, std::string> nsMap;
  collectImportNamespaces(added, nsMap);

  for (std::string ident : this->imports) {
    ast::Statement *statement = gen::utils::extract(ident, added, id);
    if (statement == nullptr) continue;
    if (dynamic_cast<ast::Class *>(statement) == nullptr &&
        dynamic_cast<ast::Enum *>(statement) == nullptr &&
        dynamic_cast<ast::Transform *>(statement) == nullptr)
      continue;
    if (generator.includedClasses.contains(id + "::" + ident)) continue;
    generator.includedClasses.insert(id + "::" + ident, nullptr);
    statement->namespaceSwap(nsMap);
    OutputFile << generator.GenSTMT(statement);
  }

  generator.cwd = prevCwd;
  return {OutputFile, std::nullopt};
}
}  // namespace ast
