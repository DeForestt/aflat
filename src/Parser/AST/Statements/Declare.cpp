#include "Parser/AST/Statements/Declare.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"

namespace ast {
gen::GenerationResult const Declare::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  int offset = generator.getBytes(this->type.size);
  links::LinkedList<gen::Symbol> *Table;

  if (!generator.globalScope) {
    // if the there  is no scope use the scope manager otherwise use the
    // scope
    if (generator.scope == nullptr || generator.inFunction) {
      auto mod = gen::scope::ScopeManager::getInstance()->assign(
          this->ident, this->type, false, this->mut);
      auto def = new asmc::Define();
      def->logicalLine = this->logicalLine;
      def->name = this->ident;
      def->type = this->type.size;
      def->value = "-" + std::to_string(mod) + "(%rbp)";
      file.text << def;
    } else {
      // add the symbol to the class symbol table
      Table = &generator.scope->SymbolTable;

      if (Table->search<std::string>(gen::utils::searchSymbol, this->ident) !=
          nullptr)
        generator.alert("redefined variable: " + this->ident);
      gen::Symbol Symbol;
      if (Table->head == nullptr) {
        Symbol.byteMod = offset;
      } else {
        Symbol.byteMod = Table->head->data.byteMod + offset;
      }
      Symbol.type = this->type;
      Symbol.symbol = this->ident;
      Symbol.mutable_ = this->mut;
      Table->push(Symbol);
      // if the symbol is public add it to the public symbol table
      if (this->scope == ast::Public && generator.scope != nullptr)
        generator.scope->publicSymbols.push(Symbol);
    };
  } else {
    Table = &generator.GlobalSymbolTable;
    auto var = new asmc::LinkTask();
    auto label = new asmc::Label();
    if (Table->search<std::string>(gen::utils::searchSymbol, this->ident) !=
        nullptr)
      generator.alert("redefined global variable: " + this->ident);

    label->label = this->ident;
    if (this->type.size = asmc::QWord) {
      var->command = "quad";
    };
    gen::Symbol Symbol;

    Symbol.type = this->type;
    Symbol.symbol = this->ident;
    Symbol.mutable_ = this->mut;
    file.bss << label;
    file.bss << var;
    Table->push(Symbol);
  }

  if (this->type.fPointerArgs.isFPointer) {
    auto newType = new ast::Type();
    newType->typeName = this->TypeName;
    newType->size = asmc::QWord;
    newType->fPointerArgs = this->type.fPointerArgs;
    generator.TypeList.push(*newType);
  }
  return {file, std::nullopt};
}
}  // namespace ast