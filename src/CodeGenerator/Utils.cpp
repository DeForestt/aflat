#include "CodeGenerator/Utils.hpp"

#include <unistd.h>

using namespace gen::utils;

void gen::utils::shellStatement(ast::Statement *stmt) {
  if (dynamic_cast<ast::Sequence *>(stmt)) {
    ast::Sequence *seq = dynamic_cast<ast::Sequence *>(stmt);
    shellStatement(seq->Statement1);
    shellStatement(seq->Statement2);
  } else if (dynamic_cast<ast::Function *>(stmt)) {
    ast::Function *func = dynamic_cast<ast::Function *>(stmt);
    // delete func->statement;
    // func->statement = nullptr;
    func->hidden = true;
  }
}

int gen::utils::sizeToInt(asmc::Size size) {
  switch (size) {
    case asmc::Size::Byte:
      return 1;
    case asmc::Size::Word:
      return 2;
    case asmc::Size::DWord:
      return 4;
    case asmc::Size::QWord:
      return 8;
    default:
      return 0;
  }
}

bool gen::utils::searchSymbol(gen::Symbol sym, std::string str) {
  if (sym.symbol == str)
    return true;
  else
    return false;
}

bool gen::utils::compareFunc(ast::Function F, std::string input) {
  if (input == F.ident.ident) {
    return true;
  }
  return false;
}

ast::Statement *gen::utils::extract(std::string ident, ast::Statement *stmt) {
  return extract(ident, stmt, "");
}

ast::Statement *gen::utils::extract(std::string ident, ast::Statement *stmt,
                                    std::string id) {
  // traverse the statement tree and return the statement with the ident
  if (stmt == nullptr) return nullptr;
  if (dynamic_cast<ast::Sequence *>(stmt) != nullptr) {
    ast::Sequence *seq = dynamic_cast<ast::Sequence *>(stmt);
    ast::Statement *temp = extract(ident, seq->Statement1, id);
    if (ident != "*" && temp != nullptr) {
      return temp;
    } else if (ident != "*") {
      return extract(ident, seq->Statement2, id);
    } else {
      extract(ident, seq->Statement2, id);
    }
  } else if (dynamic_cast<ast::Class *>(stmt)) {
    ast::Class *cls = dynamic_cast<ast::Class *>(stmt);
    if (cls->ident.ident == ident) {
      shellStatement(cls->statement);
      cls->includer = true;
      return stmt;
    }
  } else if (dynamic_cast<ast::Enum *>(stmt)) {
    ast::Enum *enm = dynamic_cast<ast::Enum *>(stmt);
    if (enm->Ident == ident) {
      return stmt;
    }
  } else if (dynamic_cast<ast::Transform *>(stmt)) {
    ast::Transform *trans = dynamic_cast<ast::Transform *>(stmt);
    ast::Transform *t = new ast::Transform();
    t->ident = trans->ident;
    t->_template = trans->_template;
    t->locked = false;
    if (t->ident == ident) {
      return t;
    }
  } else if (dynamic_cast<ast::Function *>(stmt)) {
    ast::Function *func = dynamic_cast<ast::Function *>(stmt);
    if (func->ident.ident == ident || ident == "*") {
      func->ident.ident = id + '.' + func->ident.ident;
      if (func->genericTypes.size() == 0) func->statement = nullptr;
      if (func->scope != ast::Export) func->locked = true;
      return func;
    }
  } else
    stmt->locked = true;
  if (ident == "*") {
    return stmt;
  };
  return nullptr;
}

ast::Sequence *gen::utils::extractAllFunctions(ast::Statement *stmt) {
  // recursively traverse the statement tree and return a new tree with all the
  // functions
  if (stmt == nullptr) return nullptr;
  if (dynamic_cast<ast::Sequence *>(stmt) != nullptr) {
    ast::Sequence *seq = dynamic_cast<ast::Sequence *>(stmt);
    ast::Statement *temp = extractAllFunctions(seq->Statement1);
    if (temp != nullptr) {
      auto sequence = new ast::Sequence();
      sequence->Statement1 = temp;
      sequence->Statement2 = extractAllFunctions(seq->Statement2);
      return sequence;
    } else {
      return extractAllFunctions(seq->Statement2);
    }
  } else if (dynamic_cast<ast::Function *>(stmt)) {
    ast::Function *func = dynamic_cast<ast::Function *>(stmt);
    auto f = new ast::Function(*func, false);
    f->locked = false;
    f->useType = f->type;
    f->hidden = false;
    auto sequence = new ast::Sequence();
    sequence->Statement1 = f;
    sequence->Statement2 = nullptr;
    return sequence;
  }
  return nullptr;
}

ast::Sequence *gen::utils::copyAllFunctionShells(ast::Statement *stmt) {
  if (stmt == nullptr) return nullptr;
  if (dynamic_cast<ast::Sequence *>(stmt) != nullptr) {
    ast::Sequence *seq = dynamic_cast<ast::Sequence *>(stmt);
    ast::Statement *temp = copyAllFunctionShells(seq->Statement1);
    if (temp != nullptr) {
      auto sequence = new ast::Sequence();
      sequence->Statement1 = temp;
      sequence->Statement2 = copyAllFunctionShells(seq->Statement2);
      return sequence;
    } else {
      return copyAllFunctionShells(seq->Statement2);
    }
  } else if (dynamic_cast<ast::Function *>(stmt)) {
    ast::Function *func = dynamic_cast<ast::Function *>(stmt);
    auto sequence = new ast::Sequence();
    auto f = new ast::Function(*func, false);
    f->locked = false;
    f->useType = f->type;
    f->statement = nullptr;
    sequence->Statement1 = f;
    sequence->Statement2 = nullptr;
    return sequence;
  }
  return nullptr;
}

ast::Sequence *gen::utils::extractAllDeclarations(ast::Statement *stmt) {
  // recursively traverse the statement tree and return a new tree with all the
  // declarations
  if (stmt == nullptr) return nullptr;
  if (dynamic_cast<ast::Sequence *>(stmt) != nullptr) {
    ast::Sequence *seq = dynamic_cast<ast::Sequence *>(stmt);
    ast::Statement *temp = extractAllDeclarations(seq->Statement1);
    if (temp != nullptr) {
      auto sequence = new ast::Sequence();
      sequence->Statement1 = temp;
      sequence->Statement2 = extractAllDeclarations(seq->Statement2);
      return sequence;
    } else {
      return extractAllDeclarations(seq->Statement2);
    }
  } else if (dynamic_cast<ast::DecAssign *>(stmt)) {
    ast::DecAssign *dec = dynamic_cast<ast::DecAssign *>(stmt);
    auto dec2 = new ast::DecAssign(*dec);
    dec2->locked = false;
    auto sequence = new ast::Sequence();
    sequence->Statement1 = dec2;
    sequence->Statement2 = nullptr;
    return sequence;
  } else if (dynamic_cast<ast::Declare *>(stmt)) {
    ast::Declare *dec = dynamic_cast<ast::Declare *>(stmt);
    ast::Declare *dec2 = new ast::Declare(*dec);
    dec2->locked = false;
    auto sequence = new ast::Sequence();
    sequence->Statement1 = dec2;
    sequence->Statement2 = nullptr;
    return sequence;
  }
  return nullptr;
}

std::string gen::utils::getLibPath(std::string lib) {
  char result[200];
  ssize_t count = readlink("/proc/self/exe", result, 200);
  std::string filename = std::string(result, (count > 0) ? count : 0);
  std::string exepath = filename.substr(0, filename.find_last_of("/"));
  std::string libPath = exepath.substr(0, exepath.find_last_of("/")) +
                        "/libraries/std/" + lib + "/";
  return libPath;
}