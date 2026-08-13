#include "Parser/AST/Statements/PointerAssign.hpp"

#include "CodeGenerator/CodeGenerator.hpp"

namespace ast {
gen::GenerationResult const
PointerAssign::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  if (target == nullptr || expr == nullptr)
    generator.alert("pointer assignment is missing an operand", true, __FILE__,
                    __LINE__);

  auto pointer = generator.GenExpr(target, file, asmc::QWord, "adr");
  if (pointer.type != "adr")
    generator.alert("left side of =: is not an assignable pointer expression",
                    true, __FILE__, __LINE__);

  auto savePointer = new asmc::Mov();
  savePointer->logicalLine = logicalLine;
  savePointer->size = asmc::QWord;
  savePointer->from = pointer.access;
  savePointer->to = generator.registers()["%r11"]->get(asmc::QWord);
  file.text << savePointer;

  auto pushPointer = new asmc::Push();
  pushPointer->logicalLine = logicalLine;
  pushPointer->op = generator.registers()["%r11"]->get(asmc::QWord);
  file.text << pushPointer;

  auto value = generator.GenExpr(expr, file);
  const auto valueRegister =
      value.op == asmc::Float ? generator.registers()["%xmm0"]->get(value.size)
                              : generator.registers()["%rax"]->get(value.size);

  auto saveValue = new asmc::Mov();
  saveValue->logicalLine = logicalLine;
  saveValue->size = value.size;
  saveValue->op = value.op;
  saveValue->from = value.access;
  saveValue->to = valueRegister;
  file.text << saveValue;

  auto restorePointer = new asmc::Pop();
  restorePointer->logicalLine = logicalLine;
  restorePointer->op = generator.registers()["%r11"]->get(asmc::QWord);
  file.text << restorePointer;

  auto store = new asmc::Mov();
  store->logicalLine = logicalLine;
  store->size = value.size;
  store->op = value.op;
  store->from = valueRegister;
  store->to = "(" + generator.registers()["%r11"]->get(asmc::QWord) + ")";
  file.text << store;

  return {file, std::nullopt};
}
} // namespace ast
