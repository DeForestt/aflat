#include "Parser/AST/Statements/Assign.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/Parser.hpp"

namespace ast {
Assign::Assign(const std::string &ident,
               const links::LinkedList<ast::Expr *> &indices,
               links::LinkedList<std::string> modList,
               links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->logicalLine = tokens.peek()->lineCount;
  if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
    auto s2 = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (s2->Sym == ':') {
      this->reference = true;
      tokens.pop();
    }
  } else if (dynamic_cast<lex::Symbol *>(tokens.peek()) != nullptr) {
    auto s2 = dynamic_cast<lex::Symbol *>(tokens.peek());
    if (s2->meta == ">") {
      this->to = true;
      tokens.pop();
    }
  }

  this->Ident = ident;
  this->modList = modList;
  this->expr = parser.parseExpr(tokens);
  this->indices = indices;
}

gen::GenerationResult const Assign::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  auto resolved =
      generator.resolveSymbol(this->Ident, this->modList, file, this->indices);

  if (!std::get<2>(resolved)) {
    generator.alert("undefined variable:" + this->Ident);
  }

  auto symbol = &std::get<1>(resolved);

  auto fin = symbol;
  const auto var = dynamic_cast<ast::Var *>(this->expr);

  if (symbol->type.isReference && !this->override) {
    if (this->to) {
      if (!var) {
        generator.alert("A reference can only point to an lvalue");
      }
      // create a reference rather than a var
      ast::Reference *ref = new ast::Reference();
      ref->Ident = var->Ident;
      ref->modList = var->modList;
      ref->logicalLine = var->logicalLine;
      this->expr = ref;
      this->override = true; // we can set a reference even if it is const
      auto resolve =
          generator.resolveSymbol(var->Ident, var->modList, file, var->indices);
      if (!std::get<2>(resolve)) {
        generator.alert("undefined variable:" + var->Ident);
      } else {
        auto sym = &std::get<1>(resolve);
        if (!sym->mutable_ && symbol->mutable_) {
          generator.alert("A reference to the const variable " + var->Ident +
                          " cannot be assigned to a mutable variable" +
                          "it is important to keep mutability consistent");
        } else if (sym->mutable_ && symbol->mutable_) {
          gen::scope::ScopeManager::getInstance()->addAssign(sym->symbol);
        }
      }
    } else if (!(var && var->clean)) {
      this->reference = true;
    }
  }

  // check if the symbol is a class
  gen::Type **t = generator.typeList[symbol->type.typeName];
  if (t != nullptr) {
    gen::Class *cl = dynamic_cast<gen::Class *>(*t);
    if (cl != nullptr) {
      if (cl->safeType && symbol->symbol != "my" && !this->override) {
        if (cl->nameTable["_set"] != nullptr) {
          ast::Call *callGet = new ast::Call();
          callGet->ident = this->Ident;
          callGet->modList = this->modList;
          callGet->modList << "_set";
          callGet->logicalLine = this->logicalLine;
          callGet->Args.push(this->expr);
          file << generator.GenSTMT(callGet);
          return {file, std::nullopt};
        }
      }
      if (this->modList.count == 0 && !this->override) {
        // check if the class has an overloaded operator =
        ast::Function *func = cl->overloadTable[ast::Equ];
        if (func != nullptr) {
          // call the overloaded operator =
          ast::Var *v = new ast::Var();
          v->Ident = this->Ident;
          v->modList = this->modList;
          ast::Call *call = new ast::Call();
          call->ident = func->ident.ident;
          call->modList = this->modList;
          call->Args.push(v);
          call->Args.push(this->expr);
          call->publify = cl->Ident;
          ast::CallExpr *callExpr = new ast::CallExpr();
          callExpr->call = call;
          this->expr = callExpr;
        };
      }
    }
  }

  asmc::Mov *mov = new asmc::Mov();
  mov->logicalLine = this->logicalLine;
  asmc::Mov *mov2 = new asmc::Mov();
  mov2->logicalLine = this->logicalLine;
  gen::Expr expr = generator.GenExpr(this->expr, file, symbol->type.size,
                                     symbol->type.typeName);

  if (!this->reference || symbol->type.isReference) {
    if (!generator.canAssign(
            symbol->type, expr.type,
            "symbol of type {} cannot be assigned to type {}")) {
      expr = generator.GenExpr(
          generator.imply(this->expr, symbol->type.typeName), file);
    }
  }

  mov->op = expr.op;
  mov2->op = expr.op;
  mov->size = expr.size;
  mov2->size = expr.size;
  mov2->from = expr.access;

  if (expr.op == asmc::Float)
    mov2->to = generator.registers["%xmm0"]->get(expr.size);
  else
    mov2->to = generator.registers["%rbx"]->get(expr.size);
  mov->from = mov2->to;

  this->modList.invert();

  asmc::Size size;
  std::string output = std::get<0>(resolved);
  asmc::Pop *pop = nullptr;
  if (this->reference == true) {
    //
    asmc::Mov *m1 = new asmc::Mov;
    m1->logicalLine = this->logicalLine;
    m1->from = output;
    m1->size = asmc::QWord;
    m1->to = generator.registers["%r9"]->get(asmc::QWord);
    mov->to = "(" + generator.registers["%r9"]->get(asmc::QWord) + ")";

    asmc::Push *push = new asmc::Push();
    push->logicalLine = this->logicalLine;
    push->size = asmc::QWord;
    push->op = generator.registers["%r9"]->get(asmc::QWord);
    file.text << push;

    pop = new asmc::Pop();
    pop->logicalLine = this->logicalLine;
    pop->size = asmc::QWord;
    pop->op = generator.registers["%r11"]->get(asmc::QWord);

    file.text << push;
    file.text << m1;
  } else {
    mov->to = output;
  };

  if ((!fin->mutable_ || fin->readOnly) && !this->override &&
      !(this->reference && !fin->type.isReference)) {
    generator.alert("cannot assign to immutable " + fin->symbol);
  }

  mov2->logicalLine = this->logicalLine;
  mov->logicalLine = this->logicalLine;
  file.text << mov2;
  file.text << mov;
  if (pop != nullptr) {
    file.text << pop;
  }
  file << std::get<3>(resolved);
  if (this->modList.count == 0)
    gen::scope::ScopeManager::getInstance()->addAssign(fin->symbol);

  if (this->modList.count == 0 && this->reference) {
    gen::scope::ScopeManager::getInstance()->get(fin->symbol);
  }
  fin->owned = expr.owned;

  if (generator.TypeList[fin->type.typeName] == nullptr) {
    auto t = new ast::Type();
    t->typeName = fin->type.typeName;
    t->size = fin->type.size;
    t->fPointerArgs = fin->type.fPointerArgs;
    generator.TypeList.push(*t);
  }

  return {file, std::nullopt};
}
} // namespace ast
