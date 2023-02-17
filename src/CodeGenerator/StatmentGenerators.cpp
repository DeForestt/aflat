#include <unistd.h>
#include <chrono>
#include <fstream>
#include <iostream>

#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Utils.hpp"
#include "PreProcessor.hpp"
#include "Scanner.hpp"
#include "Parser/Lower.hpp"

using namespace gen::utils;

void gen::CodeGenerator::genSequence(ast::Sequence* seq,
                                     asmc::File& OutputFile) {
  OutputFile << this->GenSTMT(seq->Statement1);
  OutputFile << this->GenSTMT(seq->Statement2);
};

void gen::CodeGenerator::genFunction(ast::Function* func,
                                     asmc::File& OutputFile) {
  
  ast::Function* saveFunc = this->currentFunction;
  int saveIntArgs = intArgsCounter;
  bool isLambda = func->isLambda;

  if (this->scope == nullptr) {
    if (!func->isLambda) this->nameTable << *func;
  } else {
    if (!func->isLambda) func->scopeName = this->scope->Ident;
    this->scope->nameTable << *func;
    if (func->op != ast::None)
      if (!func->isLambda) func->scopeName = this->scope->Ident;
    this->scope->overloadTable << *func;
    if (func->scope == ast::Public)
      if (!func->isLambda) this->scope->publicNameTable << *func;
  }

  if (func->statement != nullptr) {
    gen::scope::ScopeManager::getInstance()->pushScope(true);
    this->currentFunction = func;
    bool saveIn = this->inFunction;
    this->inFunction = true;
    gen::Class* saveScope = this->scope;
    bool saveGlobal = this->globalScope;
    this->globalScope = false;

    auto label = new asmc::Label;
    label->logicalLine = func->logicalLine;
    if (this->scope == nullptr || func->isLambda)
      label->label = func->ident.ident;
    else
      label->label = "pub_" + scope->Ident + "_" + func->ident.ident;
    if (func->scopeName != "global") {
      label->label = "pub_" + func->scopeName + "_" + func->ident.ident;
      gen::Type* tScope = *this->typeList[func->scopeName];
      if (tScope == nullptr) alert("Failed to locate function Scope");
      if (dynamic_cast<gen::Class*>(tScope) == nullptr)
        alert("Can only scope to  a class");
      this->scope = dynamic_cast<gen::Class*>(tScope);
    }

    asmc::Push* push = new asmc::Push();
    push->logicalLine = func->logicalLine;
    push->op = "%rbp";
    asmc::Mov* mov = new asmc::Mov();
    mov->logicalLine = func->logicalLine;
    mov->size = asmc::QWord;
    mov->from = "%rsp";
    mov->to = "%rbp";

    if (func->scope == ast::Export) {
      auto link = new asmc::LinkTask();
      link->logicalLine = func->logicalLine;
      link->command = "global";
      link->operand = this->moduleId + '.' + label->label;
      OutputFile.linker.push(link);
      auto label2 = new asmc::Label();
      label2->label = this->moduleId + '.' + label->label;
      OutputFile.text << label2;
    }

    OutputFile.text.push(label);
    OutputFile.text.push(push);
    OutputFile.text.push(mov);
    // push the callee preserved registers
    auto push2 = new asmc::Push();
    push2->logicalLine = func->logicalLine;
    push2->op = "%rbx";
    OutputFile.text.push(push2);
    auto push3 = new asmc::Push();
    push3->logicalLine = func->logicalLine;

    int AlignmentLoc = OutputFile.text.count;
    this->intArgsCounter = 0;
    this->returnType = func->type;
    auto link = new asmc::LinkTask();
    link->logicalLine = func->logicalLine;
    link->command = "global";
    link->operand = label->label;

    if (this->scope != nullptr && !func->isLambda) {
      // add the opo to the arguments of the function
      int offset = this->getBytes(asmc::QWord);
      int size = asmc::QWord;
      gen::Symbol symbol;
      auto movy = new asmc::Mov();
      movy->logicalLine = func->logicalLine;
      movy->from = this->intArgs[intArgsCounter].get(asmc::QWord);

      symbol.symbol = "my";
      symbol.mutable_ = false;

      auto ty = ast::Type();
      ty.typeName = scope->Ident;
      ty.size = asmc::QWord;
      symbol.type = ty;

      int byteMod =
          gen::scope::ScopeManager::getInstance()->assign("my", ty, false);

      movy->size = asmc::QWord;
      movy->to = "-" + std::to_string(byteMod) + +"(%rbp)";
      OutputFile.text << movy;
      this->intArgsCounter++;
    };
    this->GenArgs(func->args, OutputFile);
    if (!isLambda && func->scope == ast::Public) OutputFile.linker.push(link);

    // if the function is 'init' and scope is a class, add the default value
    if (func->ident.ident == "init" && this->scope != nullptr) {
      // add all of the default values from the scopes list
      for (ast::DecAssign it : this->scope->defaultValues) {
        ast::Assign assign = ast::Assign();
        assign.Ident = ("my");
        assign.override = true;
        assign.expr = it.expr;
        assign.modList = LinkedList<std::string>();
        assign.modList.push(it.declare->Ident);
        OutputFile << this->GenSTMT(&assign);
      }
    }

    asmc::File file = this->GenSTMT(func->statement);
    // check if the last statement is a return statement
    if (file.text.count > 0) {
      asmc::Instruction* last = file.text.peek();
      while (dynamic_cast<asmc::nop*>(last) != nullptr) {
        file.text.pop();
        last = file.text.peek();
      };
      if (dynamic_cast<asmc::Return*>(last) == nullptr) {
        // if the function name is init then we need to alert to return 'my'
        if (func->ident.ident == "init") {
          gen::Symbol* my = gen::scope::ScopeManager::getInstance()->get("my");
          asmc::Mov* mov = new asmc::Mov();
          mov->logicalLine = func->logicalLine;
          mov->size = asmc::QWord;
          mov->from = '-' + std::to_string(my->byteMod) + "(%rbp)";
          ;
          mov->to = this->registers["%rax"]->get(asmc::QWord);
          // pop rbx
          asmc::Pop* pop = new asmc::Pop();
          pop->logicalLine = func->logicalLine;
          pop->op = "%rbx";
          file.text.push(pop);
          file.text.push(mov);
        }
        asmc::Return* ret = new asmc::Return();
        ret->logicalLine = func->logicalLine;
        file.text.push(ret);
      }
    } else {
      auto pop = new asmc::Pop();
      pop->logicalLine = func->logicalLine;
      pop->op = "%rbx";
      file.text.push(pop);
      auto ret = new asmc::Return();
      ret->logicalLine = func->logicalLine;
      file.text.push(ret);
    }
    OutputFile << file;

    auto sub = new asmc::Subq;
    sub->logicalLine = func->logicalLine;
    sub->op1 =
        "$" + std::to_string(
                  gen::scope::ScopeManager::getInstance()->getStackAlignment());
    sub->op2 = this->registers["%rsp"]->get(asmc::QWord);
    OutputFile.text.insert(sub, AlignmentLoc + 1);

    this->scope = saveScope;
    this->globalScope = saveGlobal;
    this->inFunction = saveIn;
    bool funcPop = !isLambda;
    gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile, true);
  }
  this->intArgsCounter = saveIntArgs;
  this->currentFunction = saveFunc;
}

void gen::CodeGenerator::genDeclare(ast::Declare* dec, asmc::File& OutputFile) {
  int offset = this->getBytes(dec->type.size);
  links::LinkedList<gen::Symbol>* Table;

  if (!this->globalScope) {
    // if the there  is no scope use the scope manager otherwise use the scope
    if (this->scope == nullptr || this->inFunction) {
      auto mod = gen::scope::ScopeManager::getInstance()->assign(dec->Ident, dec->type,
                                                      dec->mask, dec->mut);
      auto def = new asmc::Define();
      def->logicalLine = dec->logicalLine;
      def->name = dec->Ident;
      def->type = dec->type.size;
      def->value = "-" + std::to_string(mod) + "(%rbp)";
      OutputFile.text << def;
    } else {
      // add the symbol to the class symbol table
      Table = &this->scope->SymbolTable;

      if (Table->search<std::string>(searchSymbol, dec->Ident) != nullptr)
        alert("redefined variable: " + dec->Ident);
      gen::Symbol Symbol;
      if (Table->head == nullptr) {
        Symbol.byteMod = offset;
      } else {
        Symbol.byteMod = Table->head->data.byteMod + offset;
      }
      Symbol.type = dec->type;
      Symbol.symbol = dec->Ident;
      Symbol.mutable_ = dec->mut;
      Table->push(Symbol);
      // if the symbol is public add it to the public symbol table
      if (dec->scope == ast::Public && this->scope != nullptr)
        this->scope->publicSymbols.push(Symbol);

    };
  } else {
    Table = &this->GlobalSymbolTable;
    auto var = new asmc::LinkTask();
    auto label = new asmc::Label();
    if (Table->search<std::string>(searchSymbol, dec->Ident) != nullptr)
      alert("redefined global variable: " + dec->Ident);

    label->label = dec->Ident;
    if (dec->type.size = asmc::QWord) {
      var->command = "quad";
    };
    gen::Symbol Symbol;

    Symbol.type = dec->type;
    Symbol.symbol = dec->Ident;
    Symbol.mutable_ = dec->mut;
    OutputFile.bss << label;
    OutputFile.bss << var;
    Table->push(Symbol);
  }
}

void gen::CodeGenerator::genDecArr(ast::DecArr* dec, asmc::File& OutputFile) {
  int offset = this->getBytes(dec->type.size);

  int index = 1;
  dec->indices.reset();
  links::LinkedList<int> typeHolder;
  while (dec->indices.pos != nullptr) {
    ast::IntLiteral* lit = dynamic_cast<ast::IntLiteral*>(dec->indices.shift());
    if (lit == nullptr) alert("array index must be an integer");
    index *= lit->val;
    typeHolder.push(lit->val);
  }
  typeHolder.invert();

  offset *= index;

  ast::Type type = dec->type;
  type.arraySize = index;
  dec->type.arraySize = index;
  links::LinkedList<gen::Symbol>* Table;
  if (this->scope == nullptr || this->inFunction) {
    int bMod = gen::scope::ScopeManager::getInstance()->assign("." + dec->ident,
                                                               type, false, false);
    // create a pointer to the array
    ast::Type adr;
    adr.arraySize = 1;
    adr.opType = asmc::Hard;
    adr.typeName = "adr";
    adr.typeHint = &dec->type;
    adr.size = asmc::QWord;
    adr.indices = typeHolder;
    ast::Reference* ref = new ast::Reference();
    ref->Ident = "." + dec->ident;

    ast::DecAssign* assign = new ast::DecAssign();
    auto _dec = ast::Declare();
    assign->declare = &_dec;
    assign->declare->Ident = dec->ident;
    assign->declare->type = adr;
    assign->expr = ref;
    assign->mute = dec->mut;
    assign->declare->scope = dec->scope;

    OutputFile << this->GenSTMT(assign);

  } else {
    Table = &this->scope->SymbolTable;

    if (Table->search<std::string>(searchSymbol, dec->ident) != nullptr)
      alert("redefined variable:" + dec->ident);

    gen::Symbol Symbol;

    if (Table->head == nullptr) {
      Symbol.byteMod = offset;
    } else {
      Symbol.byteMod = Table->head->data.byteMod + offset;
    }
    Symbol.type = dec->type;
    Symbol.symbol = "." + dec->ident;
    Table->push(Symbol);

    // create a pointer to the array
    ast::Type adr;
    adr.arraySize = 1;
    adr.opType = asmc::Hard;
    adr.typeName = "adr";
    adr.typeHint = &dec->type;
    adr.indices = typeHolder;
    adr.size = asmc::QWord;
    ast::Reference* ref = new ast::Reference();
    ref->Ident = "my";
    ref->modList.push("." + dec->ident);

    ast::DecAssign* assign = new ast::DecAssign();
    auto __dec = ast::Declare();
    assign->declare = &__dec;
    assign->declare->Ident = dec->ident;
    assign->declare->type = adr;
    assign->expr = ref;
    assign->mute = dec->mut;
    assign->declare->scope = dec->scope;
    OutputFile << this->GenSTMT(assign);
  }
}

void gen::CodeGenerator::genDecAssign(ast::DecAssign* decAssign,
                                      asmc::File& OutputFile) {
  ast::Declare* dec = decAssign->declare;
  int offset = this->getBytes(dec->type.size);

  links::LinkedList<gen::Symbol>* Table;
  Table = &this->SymbolTable;

  if (!this->globalScope) {
    if (this->scope == nullptr || this->inFunction) {
      auto mov = new asmc::Mov();
      mov->logicalLine = decAssign->logicalLine;
      gen::Expr expr =
          this->GenExpr(decAssign->expr, OutputFile, dec->type.size);

      if (dec->type.typeName != "let" && !this->canAssign(dec->type, expr.type)){
        expr = this->GenExpr(imply(decAssign->expr, dec->type.typeName), OutputFile);
      };

      if (dec->type.typeName == "let") {
        if (expr.type == "generic" || expr.type == "any" || expr.type == "let") {
          alert("Cannot infer type of variable because the expression is of type " + expr.type);
        }
        dec->type.typeName = expr.type;
        dec->type.size = expr.size;
        dec->type.opType = expr.op;
      }
      int byteMod = gen::scope::ScopeManager::getInstance()->assign(
          dec->Ident, dec->type, dec->mask, decAssign->mute);
      auto s = gen::scope::ScopeManager::getInstance()->get(dec->Ident);
      s->usable = false;
      s->refCount--;

      auto mov2 = new asmc::Mov();
      mov2->logicalLine = decAssign->logicalLine;
      mov2->size = dec->type.size;
      mov2->from = expr.access;
      if (expr.op == asmc::Float)
        mov2->to = this->registers["%xmm0"]->get(expr.size);
      else
        mov2->to = this->registers["%rbx"]->get(dec->type.size);

      mov->op = expr.op;

      mov2->op = expr.op;
      mov->size = dec->type.size;
      mov->from = this->registers["%rbx"]->get(dec->type.size);
      mov->to = "-" + std::to_string(byteMod) + "(%rbp)";

      mov->from = mov2->to;

      OutputFile.text << mov2;
      OutputFile.text << mov;
      s->usable = true;
    } else {
      // add the decAssign to the class default list
      this->scope->defaultValues.push_back(*decAssign);
      // generate the declare
      dec->mut = decAssign->mute;
      OutputFile << this->GenSTMT(dec);
    }
  } else {
    gen::Symbol Symbol;

    Symbol.type = dec->type;
    Symbol.symbol = dec->Ident;
    Symbol.mutable_ = decAssign->mute;
    Table = &this->GlobalSymbolTable;

    auto var = new asmc::LinkTask();
    auto label = new asmc::Label();

    label->label = dec->Ident;
    if (dec->type.size = asmc::QWord) {
      var->command = "quad";
    }
    gen::Expr exp = this->GenExpr(decAssign->expr, OutputFile);
    if (!this->canAssign(dec->type, exp.type)){
      exp = this->GenExpr(imply(decAssign->expr, dec->type.typeName), OutputFile);
    };
    var->operand = exp.access.erase(0, 1);
    Symbol.type.opType = exp.op;
    OutputFile.data << label;
    OutputFile.data << var;
    if (Table->search<std::string>(searchSymbol, dec->Ident) != nullptr)
      alert("redefined variable:" + dec->Ident);
    Table->push(Symbol);
  };
}

void gen::CodeGenerator::genDecAssignArr(ast::DecAssignArr* decAssign,
                                         asmc::File& OutputFile) {
  ast::DecArr* dec = decAssign->declare;
  ast::Type adr;
  adr.arraySize = 1;
  dec->indices.reset();
  while (dec->indices.pos != nullptr) {
    ast::IntLiteral* lit = dynamic_cast<ast::IntLiteral*>(dec->indices.shift());
    if (lit == nullptr) alert("array index must be an integer");
    adr.indices.push(lit->val);
  }
  adr.indices.invert();
  adr.opType = asmc::Hard;
  adr.size = asmc::QWord;
  adr.typeName = "adr";
  adr.typeHint = &dec->type;

  ast::DecAssign* assign = new ast::DecAssign();
  assign->declare = new ast::Declare();
  assign->declare->Ident = dec->ident;
  assign->declare->type = adr;
  assign->expr = decAssign->expr;
  assign->mute = decAssign->mute;
  assign->declare->scope = dec->scope;
  OutputFile << this->GenSTMT(assign);
}

void gen::CodeGenerator::genReturn(ast::Return* ret, asmc::File& OutputFile) {
  auto mov = new asmc::Mov();
  mov->logicalLine = ret->logicalLine;

  gen::Expr from = this->GenExpr(ret->expr, OutputFile);

  if (!from.passable){
    alert("cannot return an lvalue reference to safe type " + from.type);
  };

  if (from.op != asmc::Float) {
    // move from.access to %rax
    auto mov2 = new asmc::Mov();
    mov2->logicalLine = ret->logicalLine;
    mov2->size = from.size;
    mov2->from = from.access;
    mov2->to = this->registers["%rax"]->get(from.size);
    mov2->op = from.op;
    OutputFile.text << mov2;
    auto push = new asmc::Push();
    push->logicalLine = ret->logicalLine;
    push->op = this->registers["%rax"]->get(asmc::QWord);
    OutputFile.text << push;
  };


  if(!this->canAssign(this->returnType, from.type, true)){
    auto imp = imply(ret->expr, this->returnType.typeName);
    from = this->GenExpr(imp, OutputFile);
  };

  scope::ScopeManager::getInstance()->softPop(this, OutputFile);

  if (from.op != asmc::Float){
    auto pop = new asmc::Pop();
    pop->logicalLine = ret->logicalLine;
    pop->op = this->registers["%rax"]->get(asmc::QWord);
    OutputFile.text << pop;
  };

  std::string move2 = (from.op == asmc::Float)
                          ? this->registers["%xmm0"]->get(from.size)
                          : this->registers["%rax"]->get(from.size);
  
  mov->from = from.access;
  mov->to = move2;
  mov->size = from.size;
  mov->op = from.op;
  OutputFile.text << mov;
  auto re = new asmc::Return();
  re->logicalLine = ret->logicalLine;
  OutputFile.text << re;
}

void gen::CodeGenerator::genAssign(ast::Assign* assign,
                                   asmc::File& OutputFile) {
  bool global = false;

  links::LinkedList<gen::Symbol>* Table = &this->SymbolTable;

  std::tuple<std::string, gen::Symbol, bool, asmc::File> resolved =
      this->resolveSymbol(assign->Ident, assign->modList, OutputFile,
                          assign->indices);

  if (!std::get<2>(resolved)) {
    alert("undefined variable:" + assign->Ident);
  }

  Symbol* symbol = &std::get<1>(resolved);

  Symbol* fin = symbol;

  // check if the symbol is a class
  gen::Type** t = this->typeList[symbol->type.typeName];
  if (t != nullptr && assign->modList.count == 0) {
    gen::Class* cl = dynamic_cast<gen::Class*>(*t);
    if (cl != nullptr) {
      // check if the class has an overloaded operator =
      ast::Function* func = cl->overloadTable[ast::Equ];
      if (func != nullptr) {
        // call the overloaded operator =
        ast::Var* v = new ast::Var();
        v->Ident = assign->Ident;
        v->modList = assign->modList;
        ast::Call* call = new ast::Call();
        call->ident = func->ident.ident;
        call->modList = assign->modList;
        call->Args.push(v);
        call->Args.push(assign->expr);
        call->publify = cl->Ident;
        ast::CallExpr* callExpr = new ast::CallExpr();
        callExpr->call = call;
        assign->expr = callExpr;
      };
    }
  }

  asmc::Mov* mov = new asmc::Mov();
  mov->logicalLine = assign->logicalLine;
  asmc::Mov* mov2 = new asmc::Mov();
  mov2->logicalLine = assign->logicalLine;
  gen::Expr expr = this->GenExpr(assign->expr, OutputFile, symbol->type.size);

  if(!assign->reference){
    if (!this->canAssign(symbol->type, expr.type)){
      expr = this->GenExpr(imply(assign->expr,symbol->type.typeName), OutputFile);
    }
  }

  mov->op = expr.op;
  mov2->op = expr.op;
  mov->size = expr.size;
  mov2->size = expr.size;
  mov2->from = expr.access;

  if (expr.op == asmc::Float)
    mov2->to = this->registers["%xmm0"]->get(expr.size);
  else
    mov2->to = this->registers["%rbx"]->get(expr.size);
  mov->from = mov2->to;

  assign->modList.invert();
  int tbyte = 0;

  asmc::Size size;
  std::string output = std::get<0>(resolved);
  if (assign->reference == true) {
    // 
    asmc::Mov* m1 = new asmc::Mov;
    m1->logicalLine = assign->logicalLine;
    m1->from = output;
    m1->size = asmc::QWord;
    m1->to = this->registers["%eax"]->get(asmc::QWord);
    mov->to = "(" + this->registers["%eax"]->get(asmc::QWord) + ")";
    OutputFile.text << m1;
  } else {
    mov->to = output;
  };

  if (!assign->reference && !fin->mutable_ && !assign->override) {
    alert("cannot assign to const " + fin->symbol);
  }

  mov2->logicalLine = assign->logicalLine;
  mov->logicalLine = assign->logicalLine;
  OutputFile.text << mov2;
  OutputFile.text << mov;
  OutputFile << std::get<3>(resolved);
  if (assign->modList.count == 0 && !assign->reference)
  scope::ScopeManager::getInstance()->addAssign(fin->symbol);
}

ast::Function gen::CodeGenerator::GenCall(ast::Call* call,
                                          asmc::File& OutputFile) {
  std::string mod = "";
  ast::Function* func;
  bool checkArgs = true;
  call->modList.invert();
  call->modList.reset();
  std::string ident = call->ident;

  std::string nsp = "";
  if (this->nameSpaceTable.contains(ident)) {
    nsp = this->nameSpaceTable.get(ident) + ".";
    ident = nsp + call->modList.shift();
  };

  links::LinkedList<gen::Symbol>* Table;
  Table = &this->SymbolTable;
  links::LinkedList<std::string> mods = links::LinkedList<std::string>();
  // first push rdx
  links::LinkedList<std::string> stack;


  this->intArgsCounter = 0;
  int argsCounter = 0;
  std::string allMods = "";
  if (call->modList.pos == nullptr) {
    func = this->nameTable[ident];
    if (func == nullptr) {
      gen::Symbol* smbl = gen::scope::ScopeManager::getInstance()->get(ident);
      if (smbl == nullptr)
        smbl = this->GlobalSymbolTable.search<std::string>(searchSymbol, ident);
      if (smbl != nullptr) {
        if (smbl->type.typeName == "adr") {
          ast::Var* var = new ast::Var();
          var->logicalLine = call->logicalLine;
          var->Ident = smbl->symbol;
          var->modList = links::LinkedList<std::string>();

          gen::Expr exp1 = this->GenExpr(var, OutputFile);

          asmc::Mov* mov = new asmc::Mov();
          mov->logicalLine = call->logicalLine;
          mov->size = exp1.size;
          mov->from = exp1.access;
          mov->to = this->registers["%r11"]->get(exp1.size);
          OutputFile.text << mov;

          func = new ast::Function();
          func->logicalLine = call->logicalLine;
          func->ident.ident = '*' + this->registers["%r11"]->get(exp1.size);
          func->type.arraySize = 0;
          func->type.size = asmc::QWord;
          func->type.typeName = "--std--flex--function";
          func->flex = true;
          checkArgs = false;
        } else {
            // find the type of the object
            auto type = *this->typeList[smbl->type.typeName];
            if (type == nullptr) alert("type not found " + smbl->type.typeName);
            auto cl = dynamic_cast<gen::Class*>(type);
            if (cl == nullptr) alert("type is not a class " + smbl->type.typeName);
            func = cl->nameTable["_call"];
            if (func == nullptr) alert("cannot preform call on type " + smbl->type.typeName + " because it does not implement the _call function");
            func->ident.ident = "pub_" + smbl->type.typeName + "__call";

            auto var = new ast::Var();
            var->logicalLine = call->logicalLine;
            var->Ident = smbl->symbol;

            auto exp1 = this->GenExpr(var, OutputFile);

            auto mov = new asmc::Mov();
            mov->logicalLine = call->logicalLine;
            mov->size = exp1.size;
            mov->from = exp1.access;
            mov->to = this->intArgs[intArgsCounter].get(exp1.size);
            OutputFile.text << mov;
            intArgsCounter++;
        }
      };
    }
  } else {
    std::string pubname;
    gen::Symbol* sym = gen::scope::ScopeManager::getInstance()->get(ident);
    if (sym == nullptr) alert("cannot find object: " + ident);
    allMods += sym->symbol + ".";

    ast::Type last = sym->type;
    std::string my = sym->symbol;
    // get the type of the original function
    gen::Type* type = *this->typeList[last.typeName];

    int modCount = 0;
    while (call->modList.pos != nullptr) {
      bool addPub = true;
      if (this->typeList[last.typeName] == nullptr)
        alert("type not found " + last.typeName);
      type = *this->typeList[last.typeName];
      gen::Class* cl = dynamic_cast<gen::Class*>(type);
      if (cl != nullptr) {
        pubname = cl->Ident;
        func = cl->nameTable[call->modList.touch()];
        gen::Class* parent = cl->parent;
        if (func == nullptr && parent) {
          func = parent->nameTable[call->modList.touch()];
          if (func != nullptr) pubname = parent->Ident;
        }
        bool shift = true;
        if (func == nullptr) {
          std::string id = call->modList.touch();
          mods.push(id);
          sym = cl->SymbolTable.search<std::string>(searchSymbol,
                                                    call->modList.touch());
          if (sym != nullptr && sym->type.typeName == "adr") {
            call->modList.touch();
            ast::Var* var = new ast::Var();
            var->logicalLine = call->logicalLine;
            var->Ident = ident;
            mods.invert();
            var->modList = mods;

            gen::Expr exp1 = this->GenExpr(var, OutputFile);

            asmc::Mov* mov = new asmc::Mov();
            mov->logicalLine = call->logicalLine;
            mov->size = exp1.size;
            mov->from = exp1.access;
            mov->to = this->registers["%r11"]->get(exp1.size);
            OutputFile.text << mov;

            //

            func = new ast::Function();
            func->logicalLine = call->logicalLine;
            func->ident.ident = '*' + this->registers["%r11"]->get(exp1.size);
            func->type = sym->type;
            func->type.typeName = "--std--flex--function";
            checkArgs = false;
            func->type.size = asmc::QWord;
            func->flex = true;
            addPub = false;
          } else if (sym == nullptr) {
            alert("cannot find function " + call->modList.touch());
          } else if (call->modList.pos->next == nullptr ) {
            // find the type of the object
            auto type = *this->typeList[sym->type.typeName];
            if (type == nullptr) alert("type not found " + sym->type.typeName);
            auto cl = dynamic_cast<gen::Class*>(type);
            if (cl == nullptr) alert("type is not a class " + sym->type.typeName);
            auto f = cl->nameTable["_call"];
            if (f == nullptr) alert("cannot preform call on type " + sym->type.typeName + " because it does not implement the _call function");
            func = new ast::Function();
            func->ident = f->ident;
            func->type = f->type;
            func->req = f->req;
            func->argTypes = f->argTypes;
            pubname = sym->type.typeName;
            shift = false;
          }
        }
        if (func != nullptr) {
          call->modList.shift();
          call->modList.invert();
          call->modList.reset();
          ast::Reference* ref = new ast::Reference();
          ref->logicalLine = call->logicalLine;
          ref->Ident = my;
          ref->modList = call->modList;
          if (shift) ref->internal = true;
          mod = "pub_" + pubname + "_";
          if (!addPub) mod = "";
          gen::Expr exp;
          exp = this->GenExpr(ref, OutputFile);
          asmc::Mov* mov = new asmc::Mov();
          mov->logicalLine = call->logicalLine;
          asmc::Mov* mov2 = new asmc::Mov();
          mov2->logicalLine = call->logicalLine;
          asmc::Push* push = new asmc::Push();
          push->logicalLine = call->logicalLine;

          mov->size = exp.size;
          mov2->size = exp.size;

          mov->from = '(' + exp.access + ')';
          mov->to = this->registers["%eax"]->get(exp.size);
          mov2->from = this->registers["%eax"]->get(exp.size);
          mov2->to = this->intArgs[argsCounter].get(exp.size);
          push->op = this->intArgs[argsCounter].get(asmc::QWord);
          stack << this->intArgs[argsCounter].get(asmc::QWord);

          argsCounter++;
          OutputFile.text << push;
          OutputFile.text << mov;
          OutputFile.text << mov2;

          if (!shift) {
            call->modList.shift();
            call->modList.invert();
            call->modList.reset();
            auto var = new ast::Var();
            var->logicalLine = call->logicalLine;
            var->Ident = ident;
            var->modList = call->modList;
            var->internal = true;
            call->Args.invert();
            call->Args.push(var);
            call->Args.invert();
          }
          break;
        }
      }
      mods.push(call->modList.shift());
      last = sym->type;
      allMods += sym->symbol + ".";
      modCount++;
    };
  };

  if (call->publify != "") {
    func = new ast::Function();
    func->ident.ident = "pub_" + call->publify + "_" + ident;
    func->type.typeName = call->publify;
    func->type.size = asmc::QWord;
    func->scopeName = call->publify;
    if (ident != "init" && ident != "del") {
      // find the function in the class
      gen::Class* cl =
          dynamic_cast<gen::Class*>(*this->typeList[call->publify]);
      if (cl != nullptr) {
        ast::Function* f = cl->nameTable[ident];
        if (f == nullptr) {
          alert("cannot find function: " + ident + " in class " +
                call->publify);
        };
        func->type = f->type;
        func->scope = f->scope;
        func->scopeName = f->scopeName;
        ast::Type t;
        t.typeName = cl->Ident;
        t.size = asmc::QWord;
        func->argTypes = f->argTypes;
        func->argTypes.push_back(t);
      };
    } else {
      argsCounter++;
      stack.push("%rdi");
      // find the class
      gen::Type* type = *this->typeList[call->publify];
      if (type == nullptr) alert("type not found " + call->publify);
      gen::Class* cl = dynamic_cast<gen::Class*>(type);
      if (cl == nullptr) alert("not a class: " + call->publify);
      ast::Function* f = cl->nameTable[ident];
      if (f == nullptr)
        alert("cannot find function: " + ident + " in " + cl->Ident);
      func->argTypes = f->argTypes;
      func->req = f->req;
    }
    mod = "";
  }

  asmc::Push* push = new asmc::Push();
  push->logicalLine = call->logicalLine;
  push->op = this->registers["%rdx"]->get(asmc::QWord);
  OutputFile.text << push;
  stack << push->op;

  if (func == nullptr) alert("Cannot Find Function: " + ident + allMods);

  if (func->scope == ast::Private && func->scopeName != "global") {
    if (this->scope == nullptr)
      alert("attempt to call private function: " + allMods + func->ident.ident +
            " from global scope");
    if (func->scopeName != this->scope->Ident)
      alert("Attempt to call private function " + allMods + func->ident.ident +
            " from wrong scope: " + this->scope->Ident +
            " != " + func->scopeName);
  }

  call->Args.invert();
  call->Args.reset();
  links::LinkedList<ast::Expr*> args;
  std::vector<asmc::Instruction*> overflowArgs;
  int i = 0;
  if (call->Args.trail() < func->req)
    alert("Too few arguments for function: " + ident +
          " expected: " + std::to_string(func->argTypes.size()) +
          " got: " + std::to_string(i + 1));

  while (call->Args.trail() > 0) {
    args.push(call->Args.touch());
    ast::Expr * rem = call->Args.touch();
    gen::Expr exp = this->GenExpr(call->Args.shift(), OutputFile);
    if (!exp.passable) this->alert("Cannot pass an lvalue of safe type " + exp.type + " to a function");
    bool run;
    if (checkArgs) {
      if (i >= func->argTypes.size()) {
        alert("Too many arguments for function: " + ident +
              " expected: " + std::to_string(func->argTypes.size()) +
              " got: " + std::to_string(i + 1));
      };
      if (!canAssign(func->argTypes.at(i), exp.type)) {
        ast::Expr * init = imply(rem, func->argTypes.at(i).typeName);
        exp = this->GenExpr(init, OutputFile);
      };
    };
    i++;
    if (exp.op == asmc::Float){
      ast::Type fl = ast::Type();
      fl.typeName = "float";
      fl.size = asmc::DWord;
      int bytemod = gen::scope::ScopeManager::getInstance()->assign("", fl, false, false);
      asmc::Mov* mov = new asmc::Mov();
      mov->op = asmc::Float;
      mov->logicalLine = call->logicalLine;
      mov->size = exp.size;
      mov->from = exp.access;
      mov->to = this->registers["%xmm0"]->get(exp.size);
      OutputFile.text << mov;

      asmc::Mov* mov2 = new asmc::Mov();
      mov2->op = asmc::Float;
      mov2->logicalLine = call->logicalLine;
      mov2->size = exp.size;
      mov2->from = this->registers["%xmm0"]->get(exp.size);
      mov2->to = "-" + std::to_string(bytemod) + "(%rbp)";
      OutputFile.text << mov2;
      // move to eax

      // clear out eax
      asmc::Xor* xory = new asmc::Xor();
      xory->logicalLine = call->logicalLine;
      xory->op1 = this->registers["%eax"]->get(asmc::QWord);
      xory->op2 = this->registers["%eax"]->get(asmc::QWord);
      OutputFile.text << xory;


      asmc::Mov* mov3 = new asmc::Mov();
      mov3->logicalLine = call->logicalLine;
      mov3->size = asmc::DWord;
      mov3->from = "-" + std::to_string(bytemod) + "(%rbp)";
      mov3->to = this->registers["%eax"]->get(asmc::DWord);
      OutputFile.text << mov3;

      asmc::Push* push = new asmc::Push();
      push->logicalLine = call->logicalLine;

      push->op = this->intArgs[argsCounter].get(asmc::QWord);

      OutputFile.text << push;

      asmc::Mov* mov4 = new asmc::Mov();
      mov4->logicalLine = call->logicalLine;
      mov4->size = asmc::DWord;
      mov4->from = this->registers["%eax"]->get(asmc::DWord);
      mov4->to = this->intArgs[argsCounter].get(asmc::DWord);
      OutputFile.text << mov4;
    } else {
      asmc::Mov* mov = new asmc::Mov();
      mov->logicalLine = call->logicalLine;
      asmc::Mov* mov2 = new asmc::Mov();
      mov2->logicalLine = call->logicalLine;
      asmc::Push* push = new asmc::Push();
      push->logicalLine = call->logicalLine;
      mov->size = exp.size;
      mov2->size = exp.size;

      mov->from = exp.access;
      mov->to = this->registers["%eax"]->get(exp.size);
      mov2->from = this->registers["%eax"]->get(exp.size);
      mov2->to = this->intArgs[argsCounter].get(exp.size);
      push->op = this->intArgs[argsCounter].get(asmc::QWord);
      stack << this->intArgs[argsCounter].get(asmc::QWord);

      OutputFile.text << push;
      OutputFile.text << mov;
      OutputFile.text << mov2;
    };
    argsCounter++;
  };

  int argsUsed = argsCounter;
  if (call->publify != "") argsUsed--;

  while (argsUsed < func->argTypes.size()) {
    // if the argument is a float, we need to push a float
    asmc::Mov* move = new asmc::Mov();
    move->logicalLine = call->logicalLine;
    move->size = asmc::QWord;
    move->from = "$0";
    move->to = this->intArgs[argsCounter].get(asmc::QWord);
    argsCounter++;
    argsUsed++;
    OutputFile.text << move;
  }

  // add the overflow arguments
  for (auto inst : overflowArgs) {
    OutputFile.text << inst;
  }

  // call->Args = args;
  call->Args.invert();
  asmc::Call* calls = new asmc::Call;
  calls->logicalLine = call->logicalLine;

  calls->function = mod + func->ident.ident;
  OutputFile.text << calls;
  // push everything back on the stack
  while (stack.count > 0) {
    asmc::Pop* pop = new asmc::Pop();
    pop->logicalLine = call->logicalLine;
    pop->op = stack.pop();
    OutputFile.text << pop;
  }
  intArgsCounter = 0;
  ast::Function ret = *func;
  return ret;
}
// Deprecated
void gen::CodeGenerator::genPush(ast::Push* push, asmc::File& OutputFile) {
  asmc::Mov* count = new asmc::Mov;
  count->logicalLine = push->logicalLine;

  count->size = asmc::QWord;
  count->to = this->registers["%rdx"]->get(count->size);
  count->from = "$1";
  asmc::Mov* pointer = new asmc::Mov;
  pointer->logicalLine = push->logicalLine;
  pointer->size = asmc::QWord;
  pointer->to = this->registers["%rsi"]->get(pointer->size);
  pointer->from = this->GenExpr(push->expr, OutputFile).access;

  asmc::Mov* callnum = new asmc::Mov;
  callnum->logicalLine = push->logicalLine;
  callnum->size = asmc::QWord;
  callnum->to = this->registers["%rax"]->get(callnum->size);
  callnum->from = "$1";

  asmc::Mov* rdi = new asmc::Mov;
  rdi->logicalLine = push->logicalLine;
  rdi->size = asmc::QWord;
  rdi->from = "$1";
  rdi->to = this->registers["%rdi"]->get(rdi->size);

  OutputFile.text << rdi;
  OutputFile.text << pointer;
  OutputFile.text << count;
  OutputFile.text << callnum;

  auto syscall = new asmc::SysCall;
  syscall->logicalLine = push->logicalLine;

  OutputFile.text << syscall;
};

// Deprecated
void gen::CodeGenerator::genPull(ast::Pull* pull, asmc::File& OutputFile) {
  asmc::Mov* count = new asmc::Mov;
  count->logicalLine = pull->logicalLine;
  count->size = asmc::QWord;
  count->to = this->registers["%rdx"]->get(count->size);
  count->from = "$1";
  asmc::Mov* pointer = new asmc::Mov;
  pointer->logicalLine = pull->logicalLine;
  pointer->size = asmc::QWord;
  pointer->to = this->registers["%rsi"]->get(pointer->size);
  pointer->from = this->GenExpr(pull->expr, OutputFile).access;
  asmc::Mov* callnum = new asmc::Mov;
  callnum->logicalLine = pull->logicalLine;
  callnum->size = asmc::QWord;
  callnum->to = this->registers["%rax"]->get(callnum->size);
  callnum->from = "$0";
  asmc::Mov* rdi = new asmc::Mov;
  rdi->logicalLine = pull->logicalLine;
  rdi->size = asmc::QWord;
  rdi->from = "$1";
  rdi->to = this->registers["%rdi"]->get(rdi->size);

  OutputFile.text << rdi;
  OutputFile.text << pointer;
  OutputFile.text << count;
  OutputFile.text << callnum;

  auto syscall = new asmc::SysCall;
  syscall->logicalLine = pull->logicalLine;

  OutputFile.text << syscall;
};

void gen::CodeGenerator::genIf(ast::If* ifStmt, asmc::File& OutputFile) {
  // push a new scope
  gen::scope::ScopeManager::getInstance()->pushScope(false);

  asmc::Label* label1 = new asmc::Label();
  label1->logicalLine = ifStmt->logicalLine;
  label1->label = ".L" + this->nameTable.head->data.ident.ident +
                  std::to_string(this->labelCount);
  this->labelCount++;

  gen::Expr expr = this->GenExpr(ifStmt->expr, OutputFile);

  ast::Type t = ast::Type();
  t.typeName = "bool";
  t.size = asmc::Byte;
  this->canAssign(t, expr.type);

  asmc::Mov* mov1 = new asmc::Mov();
  mov1->logicalLine = ifStmt->logicalLine;

  mov1->size = asmc::Byte;

  mov1->from = expr.access;

  mov1->to = this->registers["%rax"]->get(mov1->size);

  asmc::Cmp* cmp = new asmc::Cmp();
  cmp->logicalLine = ifStmt->logicalLine;
  asmc::Je* je = new asmc::Je();
  je->logicalLine = ifStmt->logicalLine;

  cmp->from = "$0";
  cmp->to = mov1->to;
  cmp->size = expr.size;

  je->to = label1->label;

  OutputFile.text << mov1;
  OutputFile.text << cmp;
  OutputFile.text << je;
  OutputFile << this->GenSTMT(ifStmt->statement);
  if (ifStmt->elseStatement != nullptr) {
    asmc::Label* end = new asmc::Label();
    end->logicalLine = ifStmt->logicalLine;
    end->label = ".L" + this->nameTable.head->data.ident.ident +
                 std::to_string(this->labelCount);
    this->labelCount++;
    asmc::Jmp* jmp = new asmc::Jmp();
    jmp->logicalLine = ifStmt->logicalLine;
    jmp->to = end->label;
    gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile);
    OutputFile.text << jmp;
    OutputFile.text << label1;

    gen::scope::ScopeManager::getInstance()->pushScope(true);
    OutputFile << this->GenSTMT(ifStmt->elseStatement);
    gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile);
    OutputFile.text << end;
  } else {
    gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile);
    OutputFile.text << label1;
  };
}

void gen::CodeGenerator::genWhile(ast::While* loop, asmc::File& OutputFile) {
  gen::scope::ScopeManager::getInstance()->pushScope(true);

  asmc::Label* label1 = new asmc::Label();
  label1->logicalLine = loop->logicalLine;
  label1->label = ".L" + this->nameTable.head->data.ident.ident +
                  std::to_string(this->labelCount);
  this->labelCount++;

  asmc::Label* label2 = new asmc::Label();
  label2->logicalLine = loop->logicalLine;
  label2->label = ".L" + this->nameTable.head->data.ident.ident +
                  std::to_string(this->labelCount);
  this->labelCount++;

  asmc::Label* breakLabel = new asmc::Label();
  breakLabel->logicalLine = loop->logicalLine;
  breakLabel->label = ".L" + this->nameTable.head->data.ident.ident +
                      std::to_string(this->labelCount);
  this->labelCount++;

  asmc::Jmp* jmp = new asmc::Jmp();
  jmp->logicalLine = loop->logicalLine;
  jmp->to = label2->label;
  OutputFile.text << jmp;

  OutputFile.text << label1;
  this->breakContext.push(breakLabel->label);
  this->continueContext.push(label2->label);
  OutputFile << this->GenSTMT(loop->stmt);
  this->breakContext.pop();
  this->continueContext.pop();
  gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile);

  OutputFile.text << label2;

  gen::Expr expr = this->GenExpr(loop->expr, OutputFile);

  ast::Type t = ast::Type();
  t.typeName = "bool";
  t.size = asmc::Byte;
  this->canAssign(t, expr.type);

  asmc::Mov* mov = new asmc::Mov();
  mov->logicalLine = loop->logicalLine;

  mov->size = expr.size;

  mov->from = expr.access;

  mov->to = this->registers["%eax"]->get(mov->size);

  asmc::Cmp* cmp = new asmc::Cmp();
  cmp->logicalLine = loop->logicalLine;
  asmc::Je* je = new asmc::Je();
  je->logicalLine = loop->logicalLine;

  cmp->from = "$1";
  cmp->to = mov->to;
  cmp->size = expr.size;
  je->to = label1->label;

  OutputFile.text << mov;
  OutputFile.text << cmp;
  OutputFile.text << je;
  OutputFile.text << breakLabel;
}

void gen::CodeGenerator::genFor(ast::For* loop, asmc::File& OutputFile) {
  gen::scope::ScopeManager::getInstance()->pushScope(true);

  asmc::Label* label1 = new asmc::Label();
  label1->logicalLine = loop->logicalLine;
  label1->label = ".L" + this->nameTable.head->data.ident.ident +
                  std::to_string(this->labelCount);
  this->labelCount++;

  asmc::Label* label2 = new asmc::Label();
  label2->logicalLine = loop->logicalLine;
  label2->label = ".L" + this->nameTable.head->data.ident.ident +
                  std::to_string(this->labelCount);
  this->labelCount++;

  asmc::Label* breakLabel = new asmc::Label();
  breakLabel->logicalLine = loop->logicalLine;
  breakLabel->label = ".L" + this->nameTable.head->data.ident.ident +
                      std::to_string(this->labelCount);
  this->labelCount++;

  asmc::Label* continueLabel = new asmc::Label();
  continueLabel->logicalLine = loop->logicalLine;
  continueLabel->label = ".L" + this->nameTable.head->data.ident.ident +
                         std::to_string(this->labelCount);
  this->labelCount++;
  OutputFile << this->GenSTMT(loop->declare);
  asmc::Jmp* jmp = new asmc::Jmp();
  jmp->to = label2->label;
  OutputFile.text << jmp;

  OutputFile.text << label1;

  this->breakContext.push(breakLabel->label);
  this->continueContext.push(continueLabel->label);

  gen::scope::ScopeManager::getInstance()->pushScope(true);
  OutputFile << this->GenSTMT(loop->Run);
  OutputFile.text << continueLabel;
  OutputFile << this->GenSTMT(loop->increment);
  gen::scope::ScopeManager::getInstance()->popScope(this, OutputFile);
  OutputFile.text << label2;
  this->breakContext.pop();
  this->continueContext.pop();

  gen::Expr expr = this->GenExpr(loop->expr, OutputFile);
  

  ast::Type t = ast::Type();
  t.typeName = "bool";
  t.size = asmc::Byte;
  this->canAssign(t, expr.type);

  asmc::Mov* mov = new asmc::Mov();
  mov->logicalLine = loop->logicalLine;

  mov->size = expr.size;

  mov->from = expr.access;

  mov->to = this->registers["%eax"]->get(mov->size);

  asmc::Cmp* cmp = new asmc::Cmp();
  cmp->logicalLine = loop->logicalLine;
  asmc::Je* je = new asmc::Je();
  je->logicalLine = loop->logicalLine;
  je->to = label1->label;

  cmp->from = "$1";
  cmp->to = mov->to;
  cmp->size = expr.size;

  OutputFile.text << mov;
  OutputFile.text << cmp;
  OutputFile.text << je;
  OutputFile.text << breakLabel;
  scope::ScopeManager::getInstance()->popScope(this, OutputFile);
}

void gen::CodeGenerator::genUDef(ast::UDeffType* udef, asmc::File& OutputFile) {
  gen::Type* type = new gen::Type();
  bool saveScope = this->globalScope;
  this->globalScope = false;
  type->Ident = udef->ident.ident;
  type->SymbolTable = this->GenTable(udef->statement, type->SymbolTable);
  this->typeList.push(type);
  this->globalScope = saveScope;
}

void gen::CodeGenerator::genClass(ast::Class* deff, asmc::File& OutputFile) {
  gen::Class* type = new gen::Class();
  bool saveScope = this->globalScope;
  this->globalScope = false;
  type->Ident = deff->ident.ident;
  type->nameTable.foo = compareFunc;
  type->safeType = deff->safeType;
  type->dynamic = deff->dynamic;
  this->scope = type;
  type->overloadTable.foo = [](ast::Function func, ast::Op op) {
    if (func.op == op) {
      return true;
    }
    return false;
  };
  type->SymbolTable;
  this->typeList.push(type);
  // write any signed contracts
  if (deff->base != "") {
    gen::Type** T = this->typeList[deff->base];
    if (T != nullptr) {
      gen::Class* base = dynamic_cast<gen::Class*>(*T);
      asmc::File contractFile;
      if (base != nullptr) {
        // check if the base class has a contract
        if (base->contract == nullptr)
          err::Exception("Base class does not have a contract");
        // if my contact is not nullptr stitch it with the base
        if (deff->contract != nullptr) {
          ast::Sequence* seq = new ast::Sequence();
          seq->Statement1 = deff->contract;
          seq->Statement2 = base->contract;
          type->contract = seq;
          type->parent = base;
          contractFile = this->GenSTMT(seq);
          OutputFile << contractFile;
        } else {
          type->contract = base->contract;
          type->parent = base;
          contractFile = this->GenSTMT(base->contract);
          OutputFile << contractFile;
          type->contract = deff->contract;
        }
      } else
        alert("Base class is not a class");
    } else
      alert("Base class not found");
  } else if (deff->contract != nullptr) {
    asmc::File contractFile = this->GenSTMT(deff->contract);
    OutputFile << contractFile;
    type->contract = deff->contract;
  }
  asmc::File file = this->GenSTMT(deff->statement);
  if (extract("init", deff->statement) == nullptr &&
      this->scope->defaultValues.size() > 0) {
    ast::Function* func = new ast::Function();
    func->logicalLine = deff->logicalLine;
    ast::Return* ret = new ast::Return();
    ret->logicalLine = deff->logicalLine;
    ast::Var* var = new ast::Var();
    var->logicalLine = deff->logicalLine;
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
    file << this->GenSTMT(func);
  }
  OutputFile << file;
  this->globalScope = saveScope;
  this->scope = nullptr;
}

void gen::CodeGenerator::genEnum(ast::Enum* deff, asmc::File& OutputFile) {
  gen::Enum* type = new gen::Enum();
  type->Ident = deff->Ident;
  int i = 0;
  for ( std::string s : deff->values)
    type->values << gen::Enum::EnumValue(s, i++);
  
  this->typeList.push(type);
};

void gen::CodeGenerator::genInc(ast::Inc* inc, asmc::File& OutputFile) {
  gen::Symbol* sym = gen::scope::ScopeManager::getInstance()->get(inc->ident);
  if (sym == nullptr) this->alert("Identifier not found to increment");
  this->canAssign(sym->type, "int");

  asmc::Add* add = new asmc::Add();
  add->logicalLine = inc->logicalLine;
  add->op1 = "$1";
  add->op2 = "-" + std::to_string(sym->byteMod) + "(%rbp)";
  OutputFile.text << add;
  gen::scope::ScopeManager::getInstance()->addAssign(inc->ident);
}

void gen::CodeGenerator::genDec(ast::Dec* inc, asmc::File& OutputFile) {
  gen::Symbol* sym = gen::scope::ScopeManager::getInstance()->get(inc->ident);
  if (sym == nullptr) this->alert("Identifier not found to increment");
  this->canAssign(sym->type, "int");

  asmc::Sub* sub = new asmc::Sub();
  sub->logicalLine = inc->logicalLine;
  sub->op1 = "$-1";
  sub->op2 = "-" + std::to_string(sym->byteMod) + "(%rbp)";
  OutputFile.text << sub;
  gen::scope::ScopeManager::getInstance()->addAssign(inc->ident);
}

void gen::CodeGenerator::genImport(ast::Import* imp, asmc::File& OutputFile) {
  bool standard = false;
    if (imp->path.find("./") == std::string::npos) {
      imp->path = getLibPath("src") + imp->path;
    };

    if (imp->path.substr(imp->path.length() - 3, 3) != ".af") {
      imp->path = imp->path + ".af";
    };

    std::string id = imp->path.substr(imp->path.find_last_of("/") + 1);
    // remove the .af extension
    id = id.substr(0, id.find_last_of("."));
    ast::Statement* added;
    if (this->includedMemo.contains(imp->path))
      added = this->includedMemo.get(imp->path);
    else {
      // scan the file
      std::ifstream file(imp->path);
      std::string text = std::string((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());
      lex::Lexer l = lex::Lexer();
      PreProcessor pp = PreProcessor();
      auto tokens = l.Scan(pp.PreProcess(text, getLibPath("head")));
      tokens.invert();
      // parse the file
      parse::Parser p = parse::Parser();
      ast::Statement* statement = p.parseStmt(tokens);
      auto low = parse::lower::Lowerer(statement);
      added = statement;
    }
    for (std::string ident : imp->imports) {
      ast::Statement* statement = extract(ident, added, id);
      if (statement == nullptr)
        this->alert("Identifier " + ident + " not found to import");
      OutputFile << this->GenSTMT(statement);
    }
    this->nameSpaceTable.insert(imp->nameSpace, id);
}

void gen::CodeGenerator::genDelete(ast::Delete* del, asmc::File& OutputFile) {
  std::tuple<std::string, gen::Symbol, bool, asmc::File> resolved =
        this->resolveSymbol(del->ident, del->modList, OutputFile,
                            links::LinkedList<ast::Expr*>());
    if (!std::get<2>(resolved))
      this->alert("Identifier " + del->ident + " not found to delete");

    gen::Symbol* sym = &std::get<1>(resolved);

    ast::Function* free = this->nameTable["free"];
    if (free == nullptr)
      alert(
          "Please import std library in order to use delete operator.\n\n -> "
          ".needs <std> \n\n");

    gen::Type** type = this->typeList[sym->type.typeName];
    if (type != nullptr) {
      gen::Class* classType = dynamic_cast<gen::Class*>(*type);
      if (classType != nullptr) {
        // check if the class has a destructor
        ast::Function* destructor = classType->nameTable["del"];

        if (destructor != nullptr) {
          ast::Call* call = new ast::Call();
          call->ident = del->ident;
          call->modList = LinkedList<std::string>();
          call->modList.push("del");
          call->Args = LinkedList<ast::Expr*>();

          OutputFile << this->GenSTMT(call);
        };
      }
    };
    // call free
    ast::Var* var = new ast::Var();
    var->logicalLine = del->logicalLine;
    var->Ident = del->ident;
    var->modList = LinkedList<std::string>();

    ast::Call* freeCall = new ast::Call();
    freeCall->logicalLine = del->logicalLine;
    freeCall->ident = "free";
    freeCall->modList = LinkedList<std::string>();
    freeCall->Args = LinkedList<ast::Expr*>();
    freeCall->Args.push(var);
    OutputFile << this->GenSTMT(freeCall);
    OutputFile << std::get<3>(resolved);
}

void gen::CodeGenerator::genContinue(ast::Continue* cont, asmc::File& OutputFile) {
  if (this->continueContext.size() == 0)
    this->alert("Attempted to continue outside of a loop");
  if (this->continueContext.size() < cont->level)
    this->alert("Attempted to continue deeper than the current loop");
  
  int index = cont->level - 1;
  asmc::Jmp* jmp = new asmc::Jmp();
  jmp->logicalLine = cont->logicalLine;
  jmp->to = this->continueContext.get(index);
  OutputFile.text << jmp;
}

void gen::CodeGenerator::genBreak(ast::Break* brk, asmc::File& OutputFile) {
  if (this->breakContext.size() == 0)
    this->alert("Attempted to break outside of a loop");
  if (this->breakContext.size() < brk->level)
    this->alert("Attempted to break deeper than the current loop");
  
  int index = brk->level - 1;
  asmc::Jmp* jmp = new asmc::Jmp();
  jmp->logicalLine = brk->logicalLine;
  jmp->to = this->breakContext.get(index); 
  OutputFile.text << jmp;
}
#pragma endregion