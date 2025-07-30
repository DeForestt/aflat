#include "CodeGenerator/Utils.hpp"

#include <unistd.h>

#include "CodeGenerator/CodeGenerator.hpp"

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

asmc::Size gen::utils::toSize(size_t size) {
  switch (size) {
    case 1:
      return asmc::Size::Byte;
    case 2:
      return asmc::Size::Word;
    case 4:
      return asmc::Size::DWord;
    case 8:
      return asmc::Size::QWord;
    default:
      return asmc::Size::AUTO;
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
      if (cls->genericTypes.size() == 0) {
        shellStatement(cls->statement);
        cls->includer = true;
        return cls;
      }
      return ast::deepCopy(cls);
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

std::string gen::utils::generateUUID() {
  std::string uuid;
  for (int i = 0; i < 16; i++) {
    uuid += std::to_string(rand() % 10);
  }
  return uuid;
}

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

std::tuple<std::string, std::unordered_map<std::string, std::string>>
gen::utils::parseGenericName(const std::string &name,
                             const std::vector<std::string> &templateTypeNames,
                             CodeGenerator &generator) {
  std::string base;
  std::unordered_map<std::string, std::string> mapping;

  auto lt_pos = name.find('<');
  if (lt_pos == std::string::npos) {
    // No generics — check mismatch with templateTypeNames
    if (!templateTypeNames.empty()) {
      generator.alert(
          "Generic type parameters expected but not provided in " + name, true,
          __FILE__, __LINE__);
    }
    return {name, mapping};
  }

  base = name.substr(0, lt_pos);

  auto gt_pos = name.rfind('>');
  if (gt_pos == std::string::npos || gt_pos <= lt_pos) {
    generator.alert("Invalid generic type syntax in " + name, true, __FILE__,
                    __LINE__);
    return {base, mapping};
  }

  // Extract substring between < and >
  std::string inner = name.substr(lt_pos + 1, gt_pos - lt_pos - 1);

  // Parse inner parameters respecting nesting
  int depth = 0;
  std::string current;
  std::vector<std::string> actualTypes;

  for (char c : inner) {
    if (c == '<') {
      depth++;
      current += c;
    } else if (c == '>') {
      if (depth == 0) {
        generator.alert("Mismatched '>' in " + name, true, __FILE__, __LINE__);
        return {base, mapping};
      }
      depth--;
      current += c;
    } else if (c == ',' && depth == 0) {
      // Split at top-level commas
      size_t start = current.find_first_not_of(" \t");
      size_t end = current.find_last_not_of(" \t");
      if (start != std::string::npos) {
        actualTypes.push_back(current.substr(start, end - start + 1));
      }
      current.clear();
    } else {
      current += c;
    }
  }

  // Add last type
  size_t start = current.find_first_not_of(" \t");
  size_t end = current.find_last_not_of(" \t");
  if (start != std::string::npos) {
    actualTypes.push_back(current.substr(start, end - start + 1));
  }

  if (depth != 0) {
    generator.alert("Unbalanced '<' and '>' in " + name, true, __FILE__,
                    __LINE__);
    return {base, mapping};
  }

  // Map template names to actual types
  if (actualTypes.size() != templateTypeNames.size()) {
    generator.alert("Generic parameter count mismatch in " + name +
                        " (expected " +
                        std::to_string(templateTypeNames.size()) + ", got " +
                        std::to_string(actualTypes.size()) + ")",
                    true, __FILE__, __LINE__);
  }

  size_t count = std::min(actualTypes.size(), templateTypeNames.size());
  for (size_t i = 0; i < count; ++i) {
    mapping[templateTypeNames[i]] = actualTypes[i];
  }

  return {base, mapping};
}
