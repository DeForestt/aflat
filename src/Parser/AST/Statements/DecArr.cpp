#include "Parser/AST/Statements/DecArr.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"

namespace ast {
asmc::File const DecArr::generate(gen::CodeGenerator &generator) {
  asmc::File OutputFile;
  int offset = generator.getBytes(this->type.size);
  int index = 1;
  this->indices.reset();
  links::LinkedList<int> typeHolder;
  while (this->indices.pos != nullptr) {
    ast::IntLiteral *lit =
        dynamic_cast<ast::IntLiteral *>(this->indices.shift());
    if (lit == nullptr) generator.alert("array index must be an integer");
    index *= lit->val;
    typeHolder.push(lit->val);
  }
  typeHolder.invert();

  offset *= index;

  ast::Type type = this->type;
  type.arraySize = index;
  this->type.arraySize = index;
  links::LinkedList<gen::Symbol> *Table;
  if (generator.scope == nullptr || generator.inFunction) {
    gen::scope::ScopeManager::getInstance()->assign("." + this->ident, type,
                                                    false, false);
    // create a pointer to the array
    ast::Type adr;
    adr.arraySize = 1;
    adr.opType = asmc::Hard;
    adr.typeName = "adr";
    adr.typeHint = &this->type;
    adr.size = asmc::QWord;
    adr.indices = typeHolder;
    ast::Reference *ref = new ast::Reference();
    ref->Ident = "." + this->ident;

    ast::DecAssign *assign = new ast::DecAssign();
    auto _dec = ast::Declare();
    assign->declare = &_dec;
    assign->declare->ident = this->ident;
    assign->declare->type = adr;
    assign->expr = ref;
    assign->mute = this->mut;
    assign->declare->scope = this->scope;

    OutputFile << generator.GenSTMT(assign);
  } else {
    Table = &generator.scope->SymbolTable;

    if (Table->search<std::string>(gen::utils::searchSymbol, this->ident) !=
        nullptr)
      generator.alert("redefined variable:" + this->ident);

    gen::Symbol Symbol;

    if (Table->head == nullptr) {
      Symbol.byteMod = offset;
    } else {
      Symbol.byteMod = Table->head->data.byteMod + offset;
    }
    Symbol.type = this->type;
    Symbol.symbol = "." + this->ident;
    Table->push(Symbol);

    // create a pointer to the array
    ast::Type adr;
    adr.arraySize = 1;
    adr.opType = asmc::Hard;
    adr.typeName = "adr";
    adr.typeHint = &this->type;
    adr.indices = typeHolder;
    adr.size = asmc::QWord;
    ast::Reference *ref = new ast::Reference();
    ref->Ident = "my";
    ref->modList.push("." + this->ident);

    ast::DecAssign *assign = new ast::DecAssign();
    auto __dec = ast::Declare();
    assign->declare = &__dec;
    assign->declare->ident = this->ident;
    assign->declare->type = adr;
    assign->expr = ref;
    assign->mute = this->mut;
    assign->declare->scope = this->scope;
    OutputFile << generator.GenSTMT(assign);
  }
  return OutputFile;
}
};  // namespace ast