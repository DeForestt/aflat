#include "Parser/AST/Statements/Class.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/AST.hpp"
#include "Parser/Parser.hpp"

namespace ast {
namespace {

bool isPrimitiveType(const std::string &typeName) {
  return parse::PRIMITIVE_TYPES.find(typeName) != parse::PRIMITIVE_TYPES.end();
}

bool isAdrType(const std::string &typeName) { return typeName == "adr"; }

int classByteSize(const gen::Class *type) {
  if (type == nullptr || type->SymbolTable.head == nullptr)
    return 0;
  return type->SymbolTable.head->data.byteMod;
}

ast::Statement *appendStatement(ast::Statement *head, ast::Statement *tail) {
  if (head == nullptr)
    return tail;

  auto *sequence = new ast::Sequence();
  sequence->Statement1 = head;
  sequence->Statement2 = tail;
  return sequence;
}

ast::Statement *buildAutomaticDestructorBody(const gen::Class *type,
                                             int logicalLine) {
  ast::Statement *body = nullptr;

  for (const auto &field : type->SymbolTable) {
    if (isPrimitiveType(field.type.typeName))
      continue;

    auto *condition = new ast::Compound();
    condition->logicalLine = logicalLine;
    condition->op = ast::NotEqu;

    auto *fieldRef = new ast::Var();
    fieldRef->logicalLine = logicalLine;
    fieldRef->Ident = "my";
    fieldRef->modList.push(field.symbol);
    condition->expr1 = fieldRef;

    auto *nullValue = new ast::Var();
    nullValue->logicalLine = logicalLine;
    nullValue->Ident = "NULL";
    condition->expr2 = nullValue;

    auto *deleteStmt = new ast::Delete();
    deleteStmt->logicalLine = logicalLine;
    deleteStmt->ident = "my";
    deleteStmt->modList.push(field.symbol);

    auto *ifStmt = new ast::If();
    ifStmt->logicalLine = logicalLine;
    ifStmt->expr = condition;
    ifStmt->statement = deleteStmt;
    ifStmt->elseStatement = nullptr;
    ifStmt->elseIf = nullptr;

    body = appendStatement(body, ifStmt);
  }

  return body;
}

ast::Statement *buildAutomaticInvalidateBody(gen::CodeGenerator &generator,
                                             const gen::Class *type,
                                             int logicalLine) {
  ast::Statement *body = nullptr;

  for (const auto &field : type->SymbolTable) {
    if (isPrimitiveType(field.type.typeName) && !isAdrType(field.type.typeName))
      continue;

    auto *assign = new ast::Assign();
    assign->logicalLine = logicalLine;
    assign->Ident = "my";
    assign->modList.push(field.symbol);
    assign->override = true;

    auto *nullValue = new ast::Var();
    nullValue->logicalLine = logicalLine;
    nullValue->Ident = "NULL";
    assign->expr = nullValue;

    body = appendStatement(body, assign);
  }

  if (body == nullptr)
    body = new ast::Sequence();

  return body;
}

ast::Function *buildAutomaticDestructor(const gen::Class *type,
                                        int logicalLine) {
  auto *body = buildAutomaticDestructorBody(type, logicalLine);
  if (body == nullptr)
    return nullptr;

  auto *func = new ast::Function();
  func->logicalLine = logicalLine;
  func->ident.ident = "del";
  func->scope = type->declarationOnly ? ast::Private : ast::Public;
  func->hidden = type->declarationOnly;
  func->args = nullptr;
  func->statement = body;
  func->type.typeName = "void";
  func->type.size = asmc::QWord;
  func->useType = func->type;
  return func;
}

ast::Function *buildAutomaticInvalidate(gen::CodeGenerator &generator,
                                        const gen::Class *type,
                                        int logicalLine) {
  auto *func = new ast::Function();
  func->logicalLine = logicalLine;
  func->ident.ident = "__invalidate__";
  func->scope = type->declarationOnly ? ast::Private : ast::Public;
  func->hidden = type->declarationOnly;
  func->args = nullptr;
  func->statement = buildAutomaticInvalidateBody(generator, type, logicalLine);
  func->type.typeName = "void";
  func->type.size = asmc::QWord;
  func->useType = func->type;
  return func;
}

ast::Function *buildAutomaticInvalidateSignature(int logicalLine) {
  auto *func = new ast::Function();
  func->logicalLine = logicalLine;
  func->ident.ident = "__invalidate__";
  func->scope = ast::Public;
  func->args = nullptr;
  func->statement = nullptr;
  func->type.typeName = "void";
  func->type.size = asmc::QWord;
  func->useType = func->type;
  return func;
}

ast::Function *buildAutomaticTransfer(gen::CodeGenerator &generator,
                                      const gen::Class *type, int logicalLine) {
  auto *func = new ast::Function();
  func->logicalLine = logicalLine;
  func->ident.ident = "__transfer_to__";
  func->scope = type->declarationOnly ? ast::Private : ast::Public;
  func->hidden = type->declarationOnly;

  auto *buffer = new ast::Declare();
  buffer->logicalLine = logicalLine;
  buffer->ident = "buffer";
  buffer->type = ast::Type("adr", asmc::QWord);
  buffer->mut = false;
  buffer->readOnly = true;
  func->args = buffer;
  func->argTypes.push_back(buffer->type);
  func->req = 1;
  func->readOnly.push_back(true);
  func->mutability.push_back(false);

  auto *body = new ast::Sequence();

  const int bytes = classByteSize(type);
  if (bytes > 0) {
    auto *copy = new ast::Call();
    copy->logicalLine = logicalLine;
    copy->ident = "af_memcpy";
    copy->Args = links::LinkedList<ast::Expr *>();

    auto *dst = new ast::Var();
    dst->logicalLine = logicalLine;
    dst->Ident = "buffer";
    copy->Args.push(dst);

    auto *src = new ast::Reference();
    src->logicalLine = logicalLine;
    src->Ident = "my";
    copy->Args.push(src);

    auto *size = new ast::IntLiteral();
    size->logicalLine = logicalLine;
    size->val = bytes;
    copy->Args.push(size);

    body->Statement1 = copy;
  }

  auto *invalidate = new ast::Call();
  invalidate->logicalLine = logicalLine;
  invalidate->ident = "my";
  invalidate->modList.push("__invalidate__");
  invalidate->Args = links::LinkedList<ast::Expr *>();

  if (body->Statement1 == nullptr) {
    body->Statement1 = invalidate;
  } else {
    body->Statement2 = invalidate;
  }

  func->statement = body;
  func->type.typeName = "void";
  func->type.size = asmc::QWord;
  func->useType = func->type;
  return func;
}

ast::Function *buildAutomaticTransferSignature(int logicalLine) {
  auto *func = new ast::Function();
  func->logicalLine = logicalLine;
  func->ident.ident = "__transfer_to__";
  func->scope = ast::Public;

  auto *buffer = new ast::Declare();
  buffer->logicalLine = logicalLine;
  buffer->ident = "buffer";
  buffer->type = ast::Type("adr", asmc::QWord);
  buffer->mut = false;
  buffer->readOnly = true;
  func->args = buffer;
  func->argTypes.push_back(buffer->type);
  func->req = 1;
  func->readOnly.push_back(true);
  func->mutability.push_back(false);
  func->statement = nullptr;
  func->type.typeName = "void";
  func->type.size = asmc::QWord;
  func->useType = func->type;
  return func;
}

} // namespace

Class::Class(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser,
             bool safe, bool dynamic, bool pedantic, bool unique,
             std::vector<parse::Annotation> &annotations,
             std::vector<std::string> &genericTypes) {
  this->genericTypes = genericTypes;
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
        if (parser.getTypeList()[ident->meta] == nullptr)
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

  auto t = ast::Type();
  t.size = asmc::QWord;
  t.typeName = this->ident.ident;
  // Check if the class is in the typeList
  if (parser.getTypeList()[this->ident.ident] != nullptr)
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Class " + this->ident.ident + " already exists");
  t.uniqueType = unique;
  parser.getTypeList() << t;
  // check if there is a contract
  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
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
  this->uniqueType = unique;
  this->statement = parser.parseStmt(tokens);
};

gen::GenerationResult const Class::generate(gen::CodeGenerator &generator) {
  // if the class is generic, do not generate code for it. It will be
  // generated when it is instantiated with specific types.
  if (this->genericTypes.size() > 0) {
    generator.genericTypes().insert(
        {this->ident.ident, this}); // add the class to the generic types
    return {asmc::File(), std::nullopt};
  }

  // check class Annotations for @Apply(Class) ... Composition
  std::vector<gen::Class *> applys;

  for (auto &annotations : this->annotations) {
    if (annotations.name == "Apply") {
      if (annotations.args.size() != 1)
        generator.alert("@Apply needs 1 argument");
      auto ident = annotations.args[0];
      // find the class in the typeList
      gen::Type **T = generator.typeList()[ident];
      if (T == nullptr)
        generator.alert("Class not found");
      gen::Class *base = dynamic_cast<gen::Class *>(*T);
      if (base == nullptr)
        generator.alert("Class not found");
      applys.push_back(base);
    }
  }

  asmc::File OutputFile;
  gen::Class *type = new gen::Class();
  type->hidden = this->hidden;
  type->body = this->statement; // save the body in case of composition
  bool saveScope = generator.globalScope();
  generator.globalScope() = false;
  type->Ident = this->ident.ident;
  type->nameTable.foo = gen::utils::compareFunc;
  type->publicNameTable.foo = gen::utils::compareFunc;
  type->safeType = this->safeType;
  type->dynamic = this->dynamic;
  type->pedantic = this->pedantic;
  type->uniqueType = this->uniqueType;
  type->declarationOnly = this->includer;
  generator.scope() = type;
  type->overloadTable.foo = [](const ast::Function &func, const ast::Op &op) {
    if (func.op == op) {
      return true;
    }
    return false;
  };
  type->SymbolTable;
  generator.typeList().push(type);
  // write any signed contracts
  if (this->base != "") {
    gen::Type **T = generator.typeList()[this->base];
    if (T != nullptr) {
      gen::Class *base = dynamic_cast<gen::Class *>(*T);
      asmc::File contractFile;
      if (base != nullptr) {
        // check if the base class has a contract
        if (base->contract != nullptr && this->contract != nullptr) {
          ast::Sequence *seq = new ast::Sequence();
          seq->Statement1 = this->contract;
          seq->Statement2 = base->contract;
          type->contract = seq;
          type->parent = base;
          contractFile = generator.GenSTMT(seq);
          OutputFile << contractFile;
        } else if (base->contract != nullptr) {
          type->contract = base->contract;
          type->parent = base;
          contractFile = generator.GenSTMT(base->contract);
          OutputFile << contractFile;
          type->contract = this->contract;
        } else {
          type->parent = base;
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

  if (this->includer && this->genericTypes.size() == 0) {
    gen::utils::shellStatement(this->statement);
  }

  if (type->uniqueType) {
    OutputFile << generator.GenSTMT(
        buildAutomaticInvalidateSignature(this->logicalLine));
    OutputFile << generator.GenSTMT(
        buildAutomaticTransferSignature(this->logicalLine));
  }

  asmc::File file = generator.GenSTMT(this->statement);

  if (gen::utils::extract("init", this->statement) == nullptr &&
      generator.scope()->defaultValues.size() > 0) {
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

  const bool hasExplicitDestructor =
      gen::utils::extract("del", this->statement) != nullptr;
  if (type->uniqueType && !type->declarationOnly && !hasExplicitDestructor) {
    if (auto *destructor = buildAutomaticDestructor(type, this->logicalLine)) {
      file << generator.GenSTMT(destructor);
    }
  }

  const bool hasExplicitInvalidate =
      gen::utils::extract("__invalidate__", this->statement) != nullptr;
  if (type->uniqueType && !type->declarationOnly && !hasExplicitInvalidate) {
    if (auto *invalidate =
            buildAutomaticInvalidate(generator, type, this->logicalLine)) {
      file << generator.GenSTMT(invalidate);
    }
  }

  const bool hasExplicitTransfer =
      gen::utils::extract("__transfer_to__", this->statement) != nullptr;
  if (type->uniqueType && !type->declarationOnly && !hasExplicitTransfer) {
    if (auto *transfer =
            buildAutomaticTransfer(generator, type, this->logicalLine)) {
      file << generator.GenSTMT(transfer);
    }
  }

  OutputFile << file;
  generator.globalScope() = saveScope;
  generator.scope() = nullptr;

  return {OutputFile, std::nullopt};
}
} // namespace ast
