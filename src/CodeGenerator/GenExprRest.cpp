#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <iostream>

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/GenerationResult.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"

using namespace gen::utils;

namespace gen {
gen::Expr gen::CodeGenerator::GenExprRest(ast::Expr *expr,
                                          asmc::File &OutputFile,
                                          asmc::Size size,
                                          std::string typeHint) {
  gen::Expr output;
  output.op = asmc::Hard;
  this->logicalLine = expr->logicalLine;

  if (dynamic_cast<ast::IntLiteral *>(expr) != nullptr) {
    ast::IntLiteral *intLit = dynamic_cast<ast::IntLiteral *>(expr);

    output.access = '$' + std::to_string(intLit->val);
    output.size = asmc::DWord;
    if (size != asmc::AUTO) output.size = size;

    output.type = "number";
  } else if (dynamic_cast<ast::LongLiteral *>(expr) != nullptr) {
    ast::LongLiteral *intLit = dynamic_cast<ast::LongLiteral *>(expr);
    output.type = "long";
    output.access = '$' + std::to_string(intLit->val);
    output.size = asmc::QWord;
  } else if (dynamic_cast<ast::CharLiteral *>(expr) != nullptr) {
    ast::CharLiteral *intLit = dynamic_cast<ast::CharLiteral *>(expr);
    output.type = "char";
    output.access = '$' + std::to_string(intLit->value);
    output.size = asmc::Byte;
  } else if (dynamic_cast<ast::CallExpr *>(expr) != nullptr) {
    output = this->genCallExpr(dynamic_cast<ast::CallExpr *>(expr), OutputFile,
                               size);
  } else if (dynamic_cast<ast::Var *>(expr) != nullptr) {
    output =
        this->genVarExpr(*dynamic_cast<ast::Var *>(expr), OutputFile, size);
  } else if (dynamic_cast<ast::Buy *>(expr) != nullptr) {
    auto buy = dynamic_cast<ast::Buy *>(expr);
    // for now, we will onlt support buying of a variable (lvalue)
    auto var = dynamic_cast<ast::Var *>(buy->expr);
    if (var == nullptr) {
      this->alert("buying of non-variable not supported", true, __FILE__,
                  __LINE__);
    }

    auto resolved =
        this->resolveSymbol(var->Ident, var->modList, OutputFile,
                            links::LinkedList<ast::Expr *>(), false);

    if (std::get<2>(resolved) == false) {
      this->alert("attemptted to buy an undeclared variable: " + var->Ident,
                  true, __FILE__, __LINE__);
    }

    gen::Symbol *sym = &std::get<1>(resolved);

    // find the _sell function if it exists
    gen::Type **type = this->typeList[sym->type.typeName];
    if (type != nullptr) {
      gen::Class *classType = dynamic_cast<gen::Class *>(*type);
      if (classType != nullptr) {
        // check if the class has a destructor
        ast::Function *destructor = classType->nameTable["_sell"];

        if (destructor != nullptr) {
          ast::Call *call = new ast::Call();
          call->ident = var->Ident;
          call->modList = var->modList;
          call->modList.push("_sell");
          call->Args = LinkedList<ast::Expr *>();
          ast::CallExpr *callExpr = new ast::CallExpr();
          callExpr->call = call;
          callExpr->logicalLine = this->logicalLine;
          output = this->GenExpr(callExpr, OutputFile);
        } else {
          // just sell it normally
          output = this->GenExpr(var, OutputFile);
        }
      } else {
        output = this->GenExpr(var, OutputFile);
      }
    } else {
      output = this->GenExpr(var, OutputFile);
    }

    // set the symbol to sold...
    std::get<4>(resolved)->sold = this->logicalLine;
  } else if (dynamic_cast<ast::Reference *>(expr) != nullptr) {
    ast::Reference ref = *dynamic_cast<ast::Reference *>(expr);

    auto resolved =
        this->resolveSymbol(ref.Ident, ref.modList, OutputFile,
                            links::LinkedList<ast::Expr *>(), ref.internal);

    asmc::Lea *lea = new asmc::Lea();
    lea->logicalLine = this->logicalLine;

    if (std::get<2>(resolved)) {
      lea->from = std::get<0>(resolved);
    } else
      alert("variable not found " + ref.Ident, true, __FILE__, __LINE__);
    lea->to = this->registers["%rax"]->get(asmc::QWord);

    output.access = registers["%rax"]->get(asmc::QWord);
    output.access = registers["%rax"]->get(asmc::QWord);
    output.size = asmc::QWord;
    output.op = asmc::OpType::Hard;
    output.type = "adr";
    OutputFile.text << lea;
    OutputFile << std::get<3>(resolved);
  } else if (dynamic_cast<ast::StringLiteral *>(expr) != nullptr) {
    ast::StringLiteral str = *dynamic_cast<ast::StringLiteral *>(expr);
    asmc::StringLiteral *strLit = new asmc::StringLiteral();
    asmc::Label *label = new asmc::Label();
    strLit->logicalLine = this->logicalLine;
    label->logicalLine = this->logicalLine;
    if (this->scope == nullptr)
      label->label = ".str" + this->nameTable.head->data.ident.ident +
                     std::to_string(this->labelCount);
    else
      label->label = ".str" + scope->Ident + '.' +
                     scope->nameTable.head->data.ident.ident +
                     std::to_string(this->labelCount);
    this->labelCount++;
    strLit->value = str.val;
    OutputFile.data << label;
    OutputFile.data << strLit;
    output.access = "$" + label->label;
    output.size = asmc::QWord;
    output.type = "adr";
  } else if (dynamic_cast<ast::FStringLiteral *>(expr) != nullptr) {
    ast::FStringLiteral str = *dynamic_cast<ast::FStringLiteral *>(expr);

    auto call = new ast::CallExpr();
    call->call = new ast::Call();
    call->call->ident = "_fCstr";
    call->call->logicalLine = this->logicalLine;
    call->logicalLine = this->logicalLine;

    ast::StringLiteral *strLit = new ast::StringLiteral();
    strLit->val = str.val;
    strLit->logicalLine = this->logicalLine;
    call->call->Args << strLit;

    ast::StructList *list = new ast::StructList();
    list->logicalLine = this->logicalLine;

    // swap the placeholders with correct wildcard
    std::string::size_type pos;
    for (auto expr : str.args) {
      pos = strLit->val.find("%%%", 0);
      if (pos == std::string::npos)
        this->alert("too many arguments for format string", true, __FILE__,
                    __LINE__);

      asmc::File file;

      auto exp = this->GenExpr(expr, file);

      gen::Type **t = this->typeList[exp.type];
      if (t) {
        gen::Class *cl = dynamic_cast<gen::Class *>(*t);
        if (cl && cl->Ident != "string") {
          if (cl->nameTable["toString"] == nullptr) {
            if (cl->parent != nullptr) {
              if (cl->parent->nameTable["toString"] == nullptr) {
                this->alert("class " + cl->Ident +
                                " does not contain a toString method",
                            true, __FILE__, __LINE__);
              } else {
                cl = cl->parent;
              }
            } else {
              this->alert(
                  "class " + cl->Ident + " does not contain a toString method",
                  true, __FILE__, __LINE__);
            }
          }

          ast::CallExpr *call = new ast::CallExpr();
          call->call = new ast::Call();
          call->call->ident = "toString";
          call->call->logicalLine = this->logicalLine;
          call->logicalLine = this->logicalLine;
          call->call->Args << expr;
          call->call->publify = cl->Ident;
          expr = call;
        }
        exp = this->GenExpr(expr, file);
      }

      if (exp.type == "adr")
        strLit->val.replace(pos, 3, "%a");
      else if (exp.type == "int" || exp.type == "number")
        strLit->val.replace(pos, 3, "%d");
      else if (exp.type == "string")
        strLit->val.replace(pos, 3, "%s");
      else if (exp.type == "bool")
        strLit->val.replace(pos, 3, "%b");
      else if (exp.type == "char")
        strLit->val.replace(pos, 3, "%c");
      else {
        this->alert("unable to format type of " + exp.type, true, __FILE__,
                    __LINE__);
      }

      list->args << expr;
    };

    call->call->Args << list;
    output = this->GenExpr(call, OutputFile);
    output.type = "string";
  } else if (dynamic_cast<ast::FloatLiteral *>(expr) != nullptr) {
    ast::FloatLiteral *floatLit = dynamic_cast<ast::FloatLiteral *>(expr);
    asmc::FloatLiteral *fltLit = new asmc::FloatLiteral();
    asmc::Label *label = new asmc::Label();
    fltLit->logicalLine = this->logicalLine;
    label->logicalLine = this->logicalLine;
    if (this->scope == nullptr)
      label->label = ".float" + this->nameTable.head->data.ident.ident +
                     std::to_string(this->labelCount);
    else
      label->label = ".float" + scope->Ident + '.' +
                     scope->nameTable.head->data.ident.ident +
                     std::to_string(this->labelCount);
    this->labelCount++;
    fltLit->value = floatLit->val;

    // move value to the xmm0 register
    asmc::Mov *mov = new asmc::Mov();
    mov->size = asmc::DWord;
    mov->op = asmc::Float;
    mov->to = this->registers["%xmm0"]->get(asmc::DWord);
    mov->from = label->label;

    output.op = asmc::Float;
    mov->logicalLine = this->logicalLine;

    OutputFile.text << mov;
    OutputFile.data << label;
    OutputFile.data << fltLit;
    output.access = this->registers["%xmm0"]->get(asmc::DWord);
    output.size = asmc::DWord;
    output.type = "int";
  } else if (dynamic_cast<ast::DeReference *>(expr)) {
    ast::DeReference deRef = *dynamic_cast<ast::DeReference *>(expr);

    auto resolved =
        this->resolveSymbol(deRef.Ident, deRef.modList, OutputFile,
                            links::LinkedList<ast::Expr *>(), false);

    if (!std::get<2>(resolved)) {
      alert("variable not found to deRef" + deRef.Ident, true, __FILE__,
            __LINE__);
    }

    gen::Symbol *sym = &std::get<1>(resolved);

    asmc::Mov *mov = new asmc::Mov();
    asmc::Mov *mov2 = new asmc::Mov();

    mov->logicalLine = this->logicalLine;
    mov2->logicalLine = this->logicalLine;

    mov->size = asmc::QWord;
    mov->from = std::get<0>(resolved);
    mov->to = this->registers["%rax"]->get(asmc::QWord);

    mov2->from = "(" + this->registers["%rax"]->get(asmc::QWord) + ")";
    mov2->size = deRef.type.size;
    mov2->to = this->registers["%rax"]->get(deRef.type.size);

    output.access = mov2->to;
    output.size = mov2->size;
    output.type = deRef.type.typeName;

    OutputFile.text << mov;
    OutputFile.text << mov2;
  } else if (dynamic_cast<ast::Compound *>(expr) != nullptr) {
    ast::Compound comp = *dynamic_cast<ast::Compound *>(expr);
    asmc::File Dummy = asmc::File();
    ast::Function *opor = nullptr;
    std::string tname = "";
    // gen expr 1 and check if it is a class
    asmc::File dd = asmc::File();
    std::string optn = this->GenExpr(comp.expr1, dd).type;
    gen::Type **type = this->typeList[optn];
    if (type != nullptr) {
      gen::Class *cls = dynamic_cast<gen::Class *>(*type);
      if (cls != nullptr) {
        tname = optn;
        opor = cls->overloadTable[comp.op];
      }
    }

    if (opor != nullptr) {
      ast::CallExpr *call = new ast::CallExpr();
      call->logicalLine = this->logicalLine;
      call->call = new ast::Call;
      call->call->logicalLine = comp.logicalLine;
      call->call->ident = opor->ident.ident;
      call->call->Args = links::LinkedList<ast::Expr *>();
      call->call->Args.push(comp.expr1);
      call->call->Args.push(comp.expr2);
      call->call->publify = tname;

      gen::Expr afterCall = this->GenExpr(call, OutputFile);
      output.access = afterCall.access;
      output.size = afterCall.size;
      output.type = opor->type.typeName;
    } else {
      // push rdi and rdx to stack
      asmc::Push *push1 = new asmc::Push();
      push1->logicalLine = this->logicalLine;
      push1->op = this->registers["%rdi"]->get(asmc::QWord);
      OutputFile.text << push1;
      asmc::Push *push2 = new asmc::Push();
      push2->logicalLine = this->logicalLine;
      push2->op = this->registers["%rdx"]->get(asmc::QWord);
      OutputFile.text << push2;
      output.op = asmc::Hard;
      output.type = "--std--flex--function";
      switch (comp.op) {
        case ast::Plus: {
          asmc::Add *add = new asmc::Add();
          output = this->genArithmetic(add, comp, OutputFile);
          break;
        }
        case ast::Minus: {
          asmc::Sub *sub = new asmc::Sub();
          output = this->genArithmetic(sub, comp, OutputFile);
          break;
        }
        case ast::AndBit: {
          asmc::And *andBit = new asmc::And();
          output = this->genArithmetic(andBit, comp, OutputFile);
          break;
        }
        case ast::OrBit: {
          asmc::Or *orBit = new asmc::Or();
          output = this->genArithmetic(orBit, comp, OutputFile);
          break;
        }
        case ast::Less: {
          asmc::Sal *andBit = new asmc::Sal();
          gen::Expr expr1 = this->GenExpr(comp.expr1, Dummy);
          gen::Expr expr2 = this->GenExpr(comp.expr2, Dummy);

          this->prepareCompound(comp, OutputFile);

          std::string to1 = this->registers["%cl"]->get(expr1.size);
          std::string to2 = this->registers["%rdi"]->get(expr1.size);
          output.access = this->registers["%rdi"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            to1 = this->registers["%xmm1"]->get(asmc::DWord);
            to2 = this->registers["%xmm0"]->get(asmc::DWord);
            output.access = "%xmm0";
            output.op = asmc::Float;
          }
          // Move the value from edx to ecx
          asmc::Mov *mov = new asmc::Mov();
          mov->logicalLine = this->logicalLine;
          mov->to = to1;
          mov->from = this->registers["%rdx"]->get(expr1.size);
          mov->size = expr1.size;

          OutputFile.text << mov;

          andBit->op2 = to2;
          andBit->op1 = "%cl";
          andBit->size = expr1.size;
          andBit->logicalLine = this->logicalLine;
          OutputFile.text << andBit;
          output.access = this->registers["%rdi"]->get(expr1.size);

          // move access to rax
          asmc::Mov *mov2 = new asmc::Mov();
          mov2->logicalLine = this->logicalLine;
          mov2->to = this->registers["%rax"]->get(expr1.size);
          mov2->from = output.access;
          mov2->size = expr1.size;
          OutputFile.text << mov2;
          output.access = this->registers["%rax"]->get(expr1.size);

          break;
        }
        case ast::Great: {
          asmc::Sar *andBit = new asmc::Sar();
          gen::Expr expr1 = this->GenExpr(comp.expr1, Dummy);
          gen::Expr expr2 = this->GenExpr(comp.expr2, Dummy);

          this->prepareCompound(comp, OutputFile);

          std::string to1 = this->registers["%cl"]->get(expr1.size);
          std::string to2 = this->registers["%rdi"]->get(expr1.size);
          output.access = this->registers["%rdi"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            to1 = this->registers["%xmm1"]->get(asmc::DWord);
            to2 = this->registers["%xmm0"]->get(asmc::DWord);
            output.access = "%xmm0";
            output.op = asmc::Float;
          }

          // Move the value from edx to ecx
          asmc::Mov *mov = new asmc::Mov();
          mov->to = to1;
          mov->from = this->registers["%rdx"]->get(expr1.size);
          mov->size = expr1.size;
          mov->logicalLine = this->logicalLine;

          OutputFile.text << mov;

          andBit->op2 = to2;
          andBit->op1 = "%cl";
          andBit->size = expr1.size;
          andBit->logicalLine = this->logicalLine;

          OutputFile.text << andBit;

          output.size = expr1.size;
          output.type = expr1.type;
          output.access = to2;

          // move access to rax
          asmc::Mov *mov2 = new asmc::Mov();
          mov2->to = this->registers["%rax"]->get(expr1.size);
          mov2->from = output.access;
          mov2->size = expr1.size;
          mov2->logicalLine = this->logicalLine;
          OutputFile.text << mov2;
          output.access = this->registers["%rax"]->get(expr1.size);
          break;
        }
        case ast::Mul: {
          asmc::Mul *mul = new asmc::Mul();
          output = this->genArithmetic(mul, comp, OutputFile);
          break;
        }
        case ast::Div: {
          asmc::Div *div = new asmc::Div();
          div->logicalLine = this->logicalLine;

          gen::Expr expr1 = this->GenExpr(comp.expr1, Dummy);
          gen::Expr expr2 = this->GenExpr(comp.expr2, Dummy, expr1.size);

          div->op1 = expr2.access;
          div->opType = expr1.op;
          div->size = expr1.size;

          std::string to1 = this->registers["%rdx"]->get(expr1.size);
          std::string to2 = this->registers["%rax"]->get(expr1.size);
          output.access = this->registers["%rax"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            to1 = this->registers["%xmm1"]->get(asmc::DWord);
            to2 = this->registers["%xmm0"]->get(asmc::DWord);
            output.access = "%xmm0";
            div->op1 = to1;
            div->op2 = to2;
            div->logicalLine = this->logicalLine;
            this->prepareCompound(comp, OutputFile);
            output.op = asmc::Float;
          } else {
            asmc::Mov *mov = new asmc::Mov;
            mov->logicalLine = this->logicalLine;
            asmc::Mov *eax = new asmc::Mov;
            eax->logicalLine = this->logicalLine;
            expr1 = this->GenExpr(comp.expr1, OutputFile);

            eax->from = expr1.access;
            eax->to = this->registers["%rax"]->get(expr1.size);
            eax->size = expr1.size;
            OutputFile.text << eax;
            mov->op = expr1.op;

            expr2 = this->GenExpr(comp.expr2, OutputFile, expr1.size);
            mov->from = expr2.access;
            mov->to = this->registers["%rcx"]->get(expr1.size);
            mov->size = expr1.size;
            mov->op = expr1.op;
            div->op1 = mov->to;
            output.access = this->registers["%rax"]->get(expr1.size);

            OutputFile.text << mov;
          }

          OutputFile.text << div;
          output.size = expr1.size;
          output.type = expr1.type;
          break;
        }
        case ast::Mod: {
          asmc::Div *div = new asmc::Div();
          div->logicalLine = this->logicalLine;

          this->selectReg = 0;
          gen::Expr expr1 = this->GenExpr(comp.expr1, Dummy);
          this->selectReg = 1;
          gen::Expr expr2 = this->GenExpr(comp.expr2, Dummy);

          div->op1 = expr2.access;

          if (expr1.op == asmc::Float) {
            std::string to1 = this->registers["%xmm1"]->get(asmc::DWord);
            std::string to2 = this->registers["%xmm0"]->get(asmc::DWord);
            output.access = "%xmm1";
            output.op = asmc::Float;
            div->op1 = to1;
            div->op2 = to2;
            this->prepareCompound(comp, OutputFile);
          } else {
            asmc::Mov *mov = new asmc::Mov;
            mov->logicalLine = this->logicalLine;
            asmc::Mov *eax = new asmc::Mov;
            eax->logicalLine = this->logicalLine;
            expr1 = this->GenExpr(comp.expr1, OutputFile);

            eax->from = expr1.access;
            eax->to = this->registers["%rax"]->get(expr1.size);
            eax->size = expr1.size;
            OutputFile.text << eax;
            mov->op = expr1.op;

            expr2 = this->GenExpr(comp.expr2, OutputFile, expr1.size);
            mov->from = expr2.access;
            mov->to = this->registers["%rcx"]->get(expr1.size);
            mov->size = expr1.size;
            mov->op = expr1.op;
            div->op1 = mov->to;
            output.access = this->registers["%rax"]->get(expr1.size);

            OutputFile.text << mov;
          }

          // Move the value from edx to eax
          asmc::Mov *mov = new asmc::Mov();
          mov->logicalLine = this->logicalLine;
          mov->to = output.access;
          mov->from = this->registers["%rdx"]->get(expr1.size);
          mov->size = expr1.size;

          OutputFile.text << div;
          if (expr1.op != asmc::Float) OutputFile.text << mov;
          output.size = asmc::DWord;
          output.type = "int";
          break;
        }
        case ast::CompEqu: {
          asmc::Cmp *cmp = new asmc::Cmp();
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Sete *sete = new asmc::Sete();
          sete->logicalLine = this->logicalLine;
          sete->op = this->registers["%rax"]->get(asmc::Byte);

          OutputFile.text << cmp;
          OutputFile.text << sete;

          output.size = asmc::Byte;
          output.type = "bool";
          output.access = "%al";
          break;
        }
        case ast::LessCmp: {
          asmc::Cmp *cmp = new asmc::Cmp();
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Setl *setl = new asmc::Setl();
          setl->logicalLine = this->logicalLine;
          setl->op = this->registers["%rax"]->get(asmc::Byte);

          OutputFile.text << cmp;
          OutputFile.text << setl;

          output.size = asmc::Byte;
          output.type = "bool";
          output.access = "%al";
          break;
        }
        case ast::GreatCmp: {
          asmc::Cmp *cmp = new asmc::Cmp();
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Setg *setg = new asmc::Setg();
          setg->logicalLine = this->logicalLine;
          setg->op = this->registers["%rax"]->get(asmc::Byte);

          OutputFile.text << cmp;
          OutputFile.text << setg;

          output.size = asmc::Byte;
          output.type = "bool";
          output.access = "%al";
          break;
        }
        case ast::Leq: {
          asmc::Cmp *cmp = new asmc::Cmp();
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Setle *setle = new asmc::Setle();
          setle->logicalLine = this->logicalLine;
          setle->op = this->registers["%rax"]->get(asmc::Byte);

          OutputFile.text << cmp;
          OutputFile.text << setle;

          output.size = asmc::Byte;
          output.type = "bool";
          output.access = "%al";
          break;
        }
        case ast::Geq: {
          asmc::Cmp *cmp = new asmc::Cmp();
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Setge *setge = new asmc::Setge();
          setge->logicalLine = this->logicalLine;
          setge->op = this->registers["%rax"]->get(asmc::Byte);

          OutputFile.text << cmp;
          OutputFile.text << setge;

          output.size = asmc::Byte;
          output.type = "bool";
          output.access = "%al";
          break;
        }
        case ast::NotEqu: {
          asmc::Cmp *cmp = new asmc::Cmp();
          cmp->logicalLine = this->logicalLine;
          gen::Expr expr1 = this->prepareCompound(comp, OutputFile);

          cmp->size = expr1.size;
          cmp->to = this->registers["%rdi"]->get(expr1.size);
          cmp->from = this->registers["%rdx"]->get(expr1.size);

          if (expr1.op == asmc::Float) {
            cmp->to = this->registers["%xmm0"]->get(asmc::DWord);
            cmp->from = this->registers["%xmm1"]->get(asmc::DWord);
            cmp->op = asmc::Float;
          }

          asmc::Setne *setne = new asmc::Setne();
          setne->logicalLine = this->logicalLine;
          setne->op = this->registers["%rax"]->get(asmc::Byte);

          OutputFile.text << cmp;
          OutputFile.text << setne;

          output.size = asmc::Byte;
          output.type = "bool";
          output.access = "%al";
          break;
        }
        default: {
          alert("Unhandled operator in comparison", true, __FILE__, __LINE__);
          break;
        }
      }
      // pop rdx and rdi
      asmc::Pop *pop = new asmc::Pop();
      pop->logicalLine = this->logicalLine;
      pop->op = this->registers["%rdx"]->get(asmc::QWord);
      OutputFile.text << pop;
      asmc::Pop *pop2 = new asmc::Pop();
      pop2->logicalLine = this->logicalLine;
      pop2->op = this->registers["%rdi"]->get(asmc::QWord);
      OutputFile.text << pop2;
    }

    if (comp.typeCast != "") {
      if (output.type != "any" && output.type != "adr" &&
          output.type != "generic")
        this->alert(
            "Can only explicitly cast to a type from any, adr, "
            "or generic",
            true, __FILE__, __LINE__);
      output.type = comp.typeCast;
    }
  } else if (dynamic_cast<ast::Lambda *>(expr) != nullptr) {
    ast::Lambda lambda = *dynamic_cast<ast::Lambda *>(expr);
    ast::Function *func = lambda.function;
    bool inFunc = this->inFunction;
    bool gScope = this->globalScope;

    auto mills = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();
    std::string id =
        "lambda_" + std::to_string(mills) + "_" + std::to_string(labelCount);
    func->ident.ident = id;
    labelCount++;
    func->scopeName = "global";
    func->isLambda = true;

    if (OutputFile.lambdas == nullptr) OutputFile.lambdas = new asmc::File;
    OutputFile.hasLambda = true;

    this->nameTable.push(*func);
    ast::Type saveRetType = this->returnType;

    auto saveLambdaReturns = this->lambdaReturns = "void";
    auto saveLambdaSize = this->lambdaSize = asmc::QWord;

    if (func->autoType) {
      ;
      this->returnType.typeName = "--std--flex--function";
    } else {
      this->returnType = func->type;
      this->lambdaReturns = func->type.typeName;
      this->lambdaSize = func->type.size;
    }

    gen::scope::ScopeManager::getInstance()->pushScope(true);
    OutputFile.lambdas->operator<<(this->GenSTMT(func));
    gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile);

    this->returnType = saveRetType;

    this->nameTable.pop();

    auto typeName = lambdaReturns + "~";
    for (auto arg : func->argTypes) {
      typeName += arg.typeName;
      typeName += ",";
    }
    // remove the last comma if it exists
    if (typeName[typeName.size() - 1] == ',') typeName.pop_back();
    typeName += "~";
    auto type = new ast::Type(typeName, asmc::QWord);
    type->fPointerArgs.argTypes = func->argTypes;
    type->fPointerArgs.returnType =
        new ast::Type(lambdaReturns, this->lambdaSize);
    type->fPointerArgs.isFPointer = true;
    type->fPointerArgs.requiredArgs = func->req;
    this->TypeList.push(*type);

    this->inFunction = inFunc;
    this->globalScope = gScope;
    this->lambdaReturns = saveLambdaReturns;
    this->lambdaSize = saveLambdaSize;

    output.access = "$" + id;
    output.size = asmc::QWord;
    output.type = typeName;
  } else if (dynamic_cast<ast::NewExpr *>(expr) != nullptr) {
    ast::NewExpr newExpr = *dynamic_cast<ast::NewExpr *>(expr);
    ast::Function *malloc = this->nameTable["malloc"];

    if (newExpr.type.typeName == "Map" &&
        typeHint.rfind("unordered_map", 0) == 0) {
      std::istringstream iss(typeHint);
      std::string typeName;
      std::getline(iss, typeName, '.');
      newExpr.type.typeName = typeName;

      while (std::getline(iss, typeName, '.')) {
        newExpr.templateTypes.push_back(typeName);
      }
    }

    if (malloc == nullptr)
      alert(
          "Please import std library in order to use new operator.\n\n -> "
          ".needs <std> \n\n",
          true, __FILE__, __LINE__);
    gen::Type **type = this->typeList[newExpr.type.typeName];
    if (type == nullptr) {
      auto cls = this->genericTypes[newExpr.type.typeName];
      if (cls != nullptr) {
        std::string new_class_name;
        type = this->instantiateGenericClass(cls, newExpr.templateTypes,
                                             new_class_name, OutputFile);
        newExpr.type.typeName = new_class_name;
      }
    }
    if (type == nullptr)
      alert("Type " + newExpr.type.typeName + " not found", true, __FILE__,
            __LINE__);
    // check if the function is a class
    auto cl = dynamic_cast<gen::Class *>(*type);
    if (cl == nullptr)
      alert("The new operator can only be used with classes. Type " +
                newExpr.type.typeName + " is not a class",
            true, __FILE__, __LINE__);
    // check if the class has a constructor
    ast::Function *init = cl->nameTable["init"];

    // first call malloc with the size of the class
    ast::CallExpr *callMalloc = new ast::CallExpr();
    callMalloc->logicalLine = newExpr.logicalLine;
    callMalloc->call = new ast::Call;
    callMalloc->call->logicalLine = newExpr.logicalLine;
    callMalloc->call->ident = malloc->ident.ident;
    callMalloc->call->Args = links::LinkedList<ast::Expr *>();
    ast::IntLiteral *size = new ast::IntLiteral();
    size->val = cl->SymbolTable.head->data.byteMod;
    callMalloc->call->Args.push(size);
    gen::Expr afterMalloc = this->GenExpr(callMalloc, OutputFile);

    if (init == nullptr) {
      output.access = afterMalloc.access;
      output.size = asmc::QWord;
      output.type = newExpr.type.typeName;
    } else {
      // creat the call STMT
      ast::CallExpr *callInit = new ast::CallExpr();
      callInit->logicalLine = newExpr.logicalLine;
      callInit->call = new ast::Call;
      callInit->call->logicalLine = newExpr.logicalLine;
      callInit->call->ident = "init";
      callInit->call->Args = newExpr.args;
      callInit->call->modList = links::LinkedList<std::string>();
      callInit->call->publify = cl->Ident;
      asmc::Mov *mov = new asmc::Mov();
      mov->logicalLine = this->logicalLine;
      asmc::Push *push = new asmc::Push();
      push->logicalLine = this->logicalLine;
      push->op = mov->to = this->registers["%rdi"]->get(asmc::QWord);
      OutputFile.text << push;
      mov->size = asmc::QWord;
      mov->from = this->registers["%eax"]->get(asmc::QWord);
      OutputFile.text << mov;
      gen::Expr afterInit = this->GenExpr(callInit, OutputFile);
      output.access = afterInit.access;
      output.size = asmc::QWord;
      output.type = newExpr.type.typeName;
    };
  } else if (dynamic_cast<ast::ParenExpr *>(expr) != nullptr) {
    ast::ParenExpr parenExpr = *dynamic_cast<ast::ParenExpr *>(expr);
    output = this->GenExpr(parenExpr.expr, OutputFile);
  } else if (dynamic_cast<ast::Not *>(expr) != nullptr) {
    ast::Not no = *dynamic_cast<ast::Not *>(expr);
    gen::Expr expr = this->GenExpr(no.expr, OutputFile);

    asmc::Movzbl *movzbl = new asmc::Movzbl();
    movzbl->logicalLine = this->logicalLine;
    movzbl->from = expr.access;
    movzbl->to = this->registers["%eax"]->get(asmc::DWord);

    asmc::Xor *xr = new asmc::Xor();
    xr->logicalLine = this->logicalLine;
    xr->op1 = "$1";
    xr->op2 = this->registers["%eax"]->get(asmc::DWord);

    ast::Type boolType = ast::Type();

    boolType.typeName = "bool";
    boolType.opType = asmc::Hard;
    boolType.size = asmc::Byte;
    this->canAssign(boolType, expr.type,
                    "the not operator can only be used with bool types. "
                    "Cannot resolve {} and {}");

    OutputFile.text << movzbl;
    OutputFile.text << xr;
    output.size = asmc::Byte;
    output.type = boolType.typeName;
    output.access = this->registers["%eax"]->get(asmc::Byte);
  } else if (dynamic_cast<ast::StructList *>(expr) != nullptr) {
    ast::StructList structList = *dynamic_cast<ast::StructList *>(expr);
    structList.args.invert();
    structList.args.reset();

    // calculate the size of the struct
    int size = 0;
    while (structList.args.trail() > 0) {
      asmc::File tempFile;
      gen::Expr expr = this->GenExpr(structList.args.shift(), tempFile);
      size += gen::utils::sizeToInt(expr.size);
    };
    structList.args.reset();

    auto mills = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();
    std::string uniqueIdent = std::to_string(mills);
    uniqueIdent = uniqueIdent + gen::utils::generateUUID();

    ast::Type structType = ast::Type();
    structType.size = asmc::Byte;
    structType.arraySize = size;
    structType.typeName = "struct";
    // save a symbol
    int bMod = gen::scope::ScopeManager::getInstance()->assign(
        uniqueIdent, structType, false, false);

    // load
    int offset = 0;

    while (structList.args.trail() > 0) {
      ast::Expr *expr = structList.args.shift();
      gen::Expr genExpr = this->GenExpr(expr, OutputFile);
      asmc::Mov *mov = new asmc::Mov();
      mov->logicalLine = this->logicalLine;
      mov->from = genExpr.access;
      mov->to = this->registers["%eax"]->get(genExpr.size);
      mov->size = genExpr.size;
      OutputFile.text << mov;
      asmc::Mov *mov2 = new asmc::Mov();
      mov2->logicalLine = this->logicalLine;
      mov2->from = this->registers["%eax"]->get(genExpr.size);
      mov2->to = "-" + std::to_string(bMod - offset) + "(%rbp)";
      mov2->size = genExpr.size;
      OutputFile.text << mov2;
      offset += gen::utils::sizeToInt(genExpr.size);
    };

    // create a pointer to the struct
    asmc::Lea *lea = new asmc::Lea();
    lea->logicalLine = this->logicalLine;
    lea->from = "-" + std::to_string(bMod) + "(%rbp)";
    lea->to = this->registers["%rax"]->get(asmc::QWord);
    OutputFile.text << lea;

    output.access = this->registers["%rax"]->get(asmc::QWord);
    output.size = asmc::QWord;
    output.type = "adr";
  } else if (dynamic_cast<ast::IfExpr *>(expr) != nullptr) {
    auto ifExpr = dynamic_cast<ast::IfExpr *>(expr);

    asmc::Label *label1 = new asmc::Label();
    label1->logicalLine = ifExpr->logicalLine;
    label1->label = ".L" + this->nameTable.head->data.ident.ident +
                    std::to_string(this->labelCount);
    this->labelCount++;
    asmc::Label *label2 = new asmc::Label();
    label2->logicalLine = ifExpr->logicalLine;
    label2->label = ".L" + this->nameTable.head->data.ident.ident +
                    std::to_string(this->labelCount);
    this->labelCount++;

    gen::Expr cond = this->GenExpr(ifExpr->expr, OutputFile);
    ast::Type boolType = ast::Type();
    boolType.typeName = "bool";
    boolType.opType = asmc::Hard;
    boolType.size = asmc::Byte;
    this->canAssign(boolType, cond.type,
                    "if condition must be a bool cannot resolve {} and {}");

    asmc::Mov *mov = new asmc::Mov();
    mov->logicalLine = this->logicalLine;
    mov->from = cond.access;
    mov->to = this->registers["%eax"]->get(asmc::Byte);
    mov->size = asmc::Byte;
    OutputFile.text << mov;

    asmc::Cmp *cmp = new asmc::Cmp();
    cmp->logicalLine = this->logicalLine;
    cmp->from = "$0";
    cmp->to = this->registers["%eax"]->get(asmc::Byte);
    cmp->size = asmc::Byte;

    OutputFile.text << cmp;

    asmc::Je *je = new asmc::Je();
    je->logicalLine = this->logicalLine;
    je->to = label1->label;
    OutputFile.text << je;

    gen::Expr trueExpr = this->GenExpr(ifExpr->trueExpr, OutputFile);
    asmc::Mov *mov2 = new asmc::Mov();
    mov2->logicalLine = this->logicalLine;
    mov2->from = trueExpr.access;
    mov2->to = this->registers["%eax"]->get(trueExpr.size);
    mov2->size = trueExpr.size;
    OutputFile.text << mov2;

    asmc::Jmp *jmp = new asmc::Jmp();
    jmp->logicalLine = this->logicalLine;
    jmp->to = label2->label;
    OutputFile.text << jmp;

    OutputFile.text << label1;
    if (!ifExpr->falseExpr)
      this->alert("if expression must have a false branch", true, __FILE__,
                  __LINE__);
    gen::Expr falseExpr = this->GenExpr(ifExpr->falseExpr, OutputFile);
    asmc::Mov *mov3 = new asmc::Mov();
    mov3->logicalLine = this->logicalLine;
    mov3->from = falseExpr.access;
    mov3->to = this->registers["%eax"]->get(falseExpr.size);
    mov3->size = falseExpr.size;
    OutputFile.text << mov3;

    OutputFile.text << label2;

    ast::Type type = ast::Type();
    type.typeName = trueExpr.type;
    this->canAssign(
        type, falseExpr.type,
        "if expression must return the same type cannot resolve {} and {}");

    output.access = this->registers["%eax"]->get(trueExpr.size);
    output.size = trueExpr.size;
    output.type = trueExpr.type;
  } else {
    this->alert("Unhandled expression", true, __FILE__, __LINE__);
  }

  if (expr->extention != nullptr) {
    auto type = ast::Type();
    type.typeName = output.type;
    type.size = output.size;

    const auto tempName = "$" + std::to_string(this->tempCount++) + "_temp";

    auto mod =
        gen::scope::ScopeManager::getInstance()->assign(tempName, type, false);
    auto mov2 = new asmc::Mov();
    mov2->logicalLine = this->logicalLine;
    mov2->from = output.access;
    mov2->to = "-" + std::to_string(mod) + "(%rbp)";
    mov2->size = output.size;
    mov2->op = output.op;
    OutputFile.text << mov2;

    auto call = dynamic_cast<ast::CallExpr *>(expr->extention);
    auto var = dynamic_cast<ast::Var *>(expr->extention);

    if (call != nullptr) {
      call->call->modList.invert();
      call->call->modList.push(call->call->ident);
      call->call->modList.invert();
      call->call->ident = tempName;
    } else if (var != nullptr) {
      var->modList.invert();
      var->modList.push(var->Ident);
      var->modList.invert();
      var->Ident = tempName;
    } else {
      this->alert("Cannot extend an expression of this type", true, __FILE__,
                  __LINE__);
    }

    output = this->GenExpr(expr->extention, OutputFile, size);
  }

  return output;
}

}  // namespace gen
