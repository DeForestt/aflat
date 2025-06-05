#include "ASM.hpp"
#include "catch.hpp"

TEST_CASE("ASM Mov toString", "[asm]") {
  asmc::Mov m;
  m.from = "%rbx";
  m.to = "%rax";
  m.size = asmc::QWord;
  REQUIRE(m.toString() == "\tmovq\t%rbx, %rax\n");
}

TEST_CASE("ASM Add int", "[asm]") {
  asmc::Add a;
  a.op1 = "%rax";
  a.op2 = "%rbx";
  a.size = asmc::DWord;
  REQUIRE(a.toString() == "\taddl\t%rax, %rbx\n");
}

TEST_CASE("Register get sizes", "[asm]") {
  asmc::Register r("rax", "eax", "ax", "al");
  REQUIRE(r.get(asmc::QWord) == "%rax");
  REQUIRE(r.get(asmc::DWord) == "%eax");
  REQUIRE(r.get(asmc::Word) == "%ax");
  REQUIRE(r.get(asmc::Byte) == "%al");
}

TEST_CASE("ASM File optimize removes redundant moves and nops", "[asm]") {
  asmc::File f;
  auto *movSame = new asmc::Mov();
  movSame->from = "%rax";
  movSame->to = "%rax";
  movSame->size = asmc::QWord;
  f.text.push(movSame);

  auto *n = new asmc::nop();
  f.text.push(n);

  auto *mov = new asmc::Mov();
  mov->from = "%rax";
  mov->to = "%rbx";
  mov->size = asmc::QWord;
  f.text.push(mov);

  f.optimize();

  REQUIRE(f.text.size() == 1);
  auto *inst = f.text.peek();
  auto *m = dynamic_cast<asmc::Mov *>(inst);
  REQUIRE(m != nullptr);
  REQUIRE(m->to == "%rbx");
}
