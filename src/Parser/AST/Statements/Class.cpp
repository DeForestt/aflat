#include "Parser/AST/Statements/Class.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"

namespace ast {
Class::Class(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser,
             bool safe, bool dynamic, bool pedantic,
             std::vector<parse::Annotation> &annotations) {
  this->annotations = annotations;
  this->logicalLine = tokens.peek()->lineCount;
  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto ident = *dynamic_cast<lex::LObj *>(tokens.pop());
    this->ident.ident = ident.meta;
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " class needs Ident");

  // check for the word signs
  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto sig = *dynamic_cast<lex::LObj *>(tokens.pop());
    if (sig.meta == "signs") {
      auto *ident = dynamic_cast<lex::LObj *>(tokens.pop());
      if (ident != nullptr) {
        if (parser.typeList[ident->meta] == nullptr)
          throw err::Exception(
              "Line: " + std::to_string(tokens.peek()->lineCount) + " Type " +
              ident->meta + " not found");
        this->base = ident->meta;
      } else
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " signs needs Ident");
    }
  } else {
    this->base = "";
  };

  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto op = *dynamic_cast<lex::OpSym *>(tokens.pop());
    if (op.Sym != '{')
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Unopened UDeffType");
  } else
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Unopened UDeffType");

  // check if there is a contract
  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto t = ast::Type();
    t.size = asmc::QWord;
    t.typeName = this->ident.ident;
    // Check if the class is in the typeList
    if (parser.typeList[this->ident.ident] != nullptr)
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Class " + this->ident.ident + " already exists");
    parser.typeList << t;

    auto contract = *dynamic_cast<lex::LObj *>(tokens.peek());
    if (contract.meta == "contract") {
      tokens.pop();
      auto sy = dynamic_cast<lex::OpSym *>(tokens.pop());
      if (sy == nullptr)
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unopened Contract");
      if (sy->Sym != '{')
        throw err::Exception(
            "Line: " + std::to_string(tokens.peek()->lineCount) +
            " Unopened Contract");
      this->contract = parser.parseStmt(tokens);
    } else
      this->contract = nullptr;
  } else
    this->contract = nullptr;
  this->safeType = safe;
  this->dynamic = dynamic;
  this->pedantic = pedantic;
  this->statement = parser.parseStmt(tokens);
};

gen::GenerationResult const Class::generate(gen::CodeGenerator &generator) {
  // check class Annotations for @Apply(Class) ... Composition
  std::vector<gen::Class *> applys;

  for (auto &annotations : this->annotations) {
    if (annotations.name == "Apply") {
      if (annotations.args.size() != 1)
        generator.alert("@Apply needs 1 argument");
      auto ident = annotations.args[0];
      // find the class in the typeList
      gen::Type **T = generator.typeList[ident];
      if (T == nullptr) generator.alert("Class not found");
      gen::Class *base = dynamic_cast<gen::Class *>(*T);
      if (base == nullptr) generator.alert("Class not found");
      applys.push_back(base);
    }
  }

  asmc::File OutputFile;
  gen::Class *type = new gen::Class();
  type->body = this->statement;  // save the body in case of composition
  bool saveScope = generator.globalScope;
  generator.globalScope = false;
  type->Ident = this->ident.ident;
  type->nameTable.foo = gen::utils::compareFunc;
  type->publicNameTable.foo = gen::utils::compareFunc;
  type->safeType = this->safeType;
  type->dynamic = this->dynamic;
  type->pedantic = this->pedantic;
  generator.scope = type;
  type->overloadTable.foo = [](ast::Function func, ast::Op op) {
    if (func.op == op) {
      return true;
    }
    return false;
  };
  type->SymbolTable;
  generator.typeList.push(type);
  // write any signed contracts
  if (this->base != "") {
    gen::Type **T = generator.typeList[this->base];
    if (T != nullptr) {
      gen::Class *base = dynamic_cast<gen::Class *>(*T);
      asmc::File contractFile;
      if (base != nullptr) {
        // check if the base class has a contract
        if (base->contract == nullptr)
          err::Exception("Base class does not have a contract");
        // if my contact is not nullptr stitch it with the base
        if (this->contract != nullptr) {
          ast::Sequence *seq = new ast::Sequence();
          seq->Statement1 = this->contract;
          seq->Statement2 = base->contract;
          type->contract = seq;
          type->parent = base;
          contractFile = generator.GenSTMT(seq);
          OutputFile << contractFile;
        } else {
          type->contract = base->contract;
          type->parent = base;
          contractFile = generator.GenSTMT(base->contract);
          OutputFile << contractFile;
          type->contract = this->contract;
        }
        // set class constraints to at least the base class constraints
        type->safeType = type->safeType || base->safeType;
        type->dynamic = type->dynamic || base->dynamic;
        type->pedantic = type->pedantic || base->pedantic;
      } else
        generator.alert("Base class is not a class");
    } else
      generator.alert("Base class not found");
  } else if (this->contract != nullptr) {
    asmc::File contractFile = generator.GenSTMT(this->contract);
    OutputFile << contractFile;
    type->contract = this->contract;
  }

  for (auto &apply : applys) {
    Sequence *seq = new Sequence();

    seq->Statement1 = gen::utils::extractAllDeclarations(apply->body);
    seq->Statement2 = gen::utils::extractAllFunctions(apply->body);
    auto seq2 = new Sequence();
    seq2->Statement1 = seq;
    seq2->Statement2 = type->body;

    type->body = seq2;
    this->statement = seq2;
  };

  auto hoist = gen::utils::copyAllFunctionShells(this->statement);
  if (hoist != nullptr) {
    OutputFile << generator.GenSTMT(hoist);
  }

  if (this->includer) {
    gen::utils::shellStatement(this->statement);
  }

  asmc::File file = generator.GenSTMT(this->statement);

  if (gen::utils::extract("init", this->statement) == nullptr &&
      generator.scope->defaultValues.size() > 0) {
    ast::Function *func = new ast::Function();
    func->logicalLine = this->logicalLine;
    ast::Return *ret = new ast::Return();
    ret->logicalLine = this->logicalLine;
    ast::Var *var = new ast::Var();
    var->logicalLine = this->logicalLine;
    var->Ident = "my";
    var->modList = links::LinkedList<std::string>();
    ret->expr = var;
    func->ident.ident = "init";
    func->scope = ast::Private;
    func->statement = ret;
    func->args = nullptr;
    ast::Type t;
    t.typeName = "adr";
    t.size = asmc::QWord;
    func->type = t;
    file << generator.GenSTMT(func);
  }
  OutputFile << file;
  generator.globalScope = saveScope;
  generator.scope = nullptr;

  return {OutputFile, std::nullopt};
}
}  // namespace ast
