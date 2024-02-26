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
  auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym != nullptr) {
    this->classes = false;
    if (sym->Sym == '{') {
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
    } else if (sym->Sym == '*') {
      tokens.pop();
      this->imports.push_back("*");
    } else
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Unexpected Token");
  } else {
    bool first = true;
    this->classes = true;
    do {
      if (!first)
        tokens.pop();
      else
        first = false;

      auto nt = dynamic_cast<lex::LObj *>(tokens.peek());
      if (nt != nullptr) {
        this->imports.push_back(nt->meta);
        auto t = ast::Type();
        t.size = asmc::QWord;
        t.typeName = nt->meta;
        t.opType = asmc::Hard;
        parser.typeList << t;
        tokens.pop();
      } else {
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Expected Ident");
      }
    } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
             dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
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
  if (!this->classes) generator.nameSpaceTable.insert(this->nameSpace, id);
  return {OutputFile, std::nullopt};
}
}  // namespace ast
