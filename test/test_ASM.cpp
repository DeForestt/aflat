#include "catch.hpp"
#include "ASM.hpp"

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
    asmc::Register r("rax","eax","ax","al");
    REQUIRE(r.get(asmc::QWord) == "%rax");
    REQUIRE(r.get(asmc::DWord) == "%eax");
    REQUIRE(r.get(asmc::Word) == "%ax");
    REQUIRE(r.get(asmc::Byte) == "%al");
}
