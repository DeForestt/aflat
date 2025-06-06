#include "Parser/AST/Statements/Import.hpp"

#include <fstream>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Lower.hpp"
#include "Parser/Parser.hpp"
#include "PreProcessor.hpp"

namespace ast {
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
          throw err::Exception("Expected Ident", tokens.peek());
        }
      } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
               dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
          dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '}') {
        tokens.pop();
      } else {
        throw err::Exception("Expected }", tokens.peek());
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
    throw err::Exception("Expected from", tokens.peek());
  };

  ast::StringLiteral *str =
      dynamic_cast<ast::StringLiteral *>(parser.parseExpr(tokens));
  if (str != nullptr) {
    this->path = str->val;
  } else {
    throw err::Exception("Expected StringLiteral", tokens.peek());
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
        throw err::Exception("Expected Ident", tokens.peek());
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
  if (this->path.find("./") == std::string::npos) {
    this->path = gen::utils::getLibPath("src") + this->path;
  };

  if (this->path.substr(this->path.length() - 3, 3) != ".af") {
    this->path = this->path + ".af";
  };

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
        return {OutputFile, std::nullopt};
      }
    }

    std::string text = std::string((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
    lex::Lexer l = lex::Lexer();
    PreProcessor pp = PreProcessor();

    auto tokens = l.Scan(pp.PreProcess(text, gen::utils::getLibPath("head")));
    tokens.invert();
    // parse the file
    parse::Parser p = parse::Parser();
    ast::Statement *statement = p.parseStmt(tokens);
    auto Lowerer = parse::lower::Lowerer(statement);
    added = statement;
    generator.includedMemo.insert(this->path, added);
    generator.ImportsOnly(added);
  }
  for (std::string ident : this->imports) {
    if (generator.includedClasses.contains(id + "::" + ident)) continue;
    generator.includedClasses.insert(id + "::" + ident, nullptr);
    ast::Statement *statement = gen::utils::extract(ident, added, id);
    if (statement == nullptr)
      generator.alert("Identifier " + ident + " not found to import");
    OutputFile << generator.GenSTMT(statement);
  }
  if (this->hasFunctions) generator.nameSpaceTable.insert(this->nameSpace, id);
  return {OutputFile, std::nullopt};
}

gen::GenerationResult const Import::generateClasses(
    gen::CodeGenerator &generator) {
  auto OutputFile = asmc::File();
  if (this->path.find("./") == std::string::npos) {
    this->path = gen::utils::getLibPath("src") + this->path;
  };

  if (this->path.substr(this->path.length() - 3, 3) != ".af") {
    this->path = this->path + ".af";
  };

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
        return {OutputFile, std::nullopt};
      }
    }

    std::string text = std::string((std::istreambuf_iterator<char>(file)),
                                   std::istreambuf_iterator<char>());
    lex::Lexer l = lex::Lexer();
    PreProcessor pp = PreProcessor();

    auto tokens = l.Scan(pp.PreProcess(text, gen::utils::getLibPath("head")));
    tokens.invert();
    parse::Parser p = parse::Parser();
    ast::Statement *statement = p.parseStmt(tokens);
    auto Lowerer = parse::lower::Lowerer(statement);
    added = statement;
    generator.includedMemo.insert(this->path, added);
    generator.ImportsOnly(added);
  }

  for (std::string ident : this->imports) {
    ast::Statement *statement = gen::utils::extract(ident, added, id);
    if (statement == nullptr) continue;
    if (dynamic_cast<ast::Class *>(statement) == nullptr &&
        dynamic_cast<ast::Enum *>(statement) == nullptr &&
        dynamic_cast<ast::Transform *>(statement) == nullptr)
      continue;
    if (generator.includedClasses.contains(id + "::" + ident)) continue;
    generator.includedClasses.insert(id + "::" + ident, nullptr);
    OutputFile << generator.GenSTMT(statement);
  }

  return {OutputFile, std::nullopt};
}
}  // namespace ast
