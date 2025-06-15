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

gen::Expr gen::CodeGenerator::prepareCompound(ast::Compound compound,
                                              asmc::File &OutputFile,
                                              bool isDiv) {
  asmc::Mov *mov1 = new asmc::Mov();
  asmc::Mov *mov2 = new asmc::Mov();
  std::string r1 = "%edx", r2 = "%rdi";
  // if expr1 op is Float set to the float registers

  gen::Expr expr2 = this->GenExpr(compound.expr2, OutputFile);

  if (expr2.op == asmc::Float) {
    r1 = "%xmm1";
    r2 = "%xmm0";
  }

  mov1->op = expr2.op;
  mov1->to = this->registers[r1]->get(expr2.size);
  mov1->from = expr2.access;
  mov1->size = expr2.size;
  mov1->logicalLine = this->logicalLine;
  if (!isDiv) OutputFile.text << mov1;

  gen::Expr expr1 = this->GenExpr(compound.expr1, OutputFile);
  mov2->op = expr1.op;
  mov2->to = this->registers[r2]->get(expr1.size);
  mov2->from = expr1.access;
  mov2->size = expr1.size;
  mov2->logicalLine = this->logicalLine;
  OutputFile.text << mov2;

  return expr1;
}

gen::Expr gen::CodeGenerator::genArithmetic(asmc::ArithInst *inst,
                                            ast::Compound compound,
                                            asmc::File &OutputFile) {
  gen::Expr expr = this->prepareCompound(compound, OutputFile);
  gen::Expr output;

  inst->opType = expr.op;
  inst->size = expr.size;

  std::string to1 = this->registers["%rdx"]->get(expr.size);
  std::string to2 = this->registers["%rdi"]->get(expr.size);

  output.access = this->registers["%rax"]->get(expr.size);

  if (expr.op == asmc::Float) {
    to1 = this->registers["%xmm1"]->get(asmc::DWord);
    to2 = this->registers["%xmm0"]->get(asmc::DWord);
    output.access = "%xmm2";
    output.op = asmc::Float;
  }

  asmc::Mov *mov = new asmc::Mov();
  mov->from = to2;
  mov->to = output.access;
  mov->op = output.op;
  mov->size = expr.size;
  mov->logicalLine = this->logicalLine;

  asmc::Pop *pop1 = new asmc::Pop();
  pop1->op = this->registers[to1]->get(asmc::QWord);
  pop1->logicalLine = this->logicalLine;

  asmc::Pop *pop2 = new asmc::Pop();
  pop2->op = this->registers[to2]->get(asmc::QWord);
  pop2->logicalLine = this->logicalLine;

  inst->op2 = to2;
  inst->op1 = to1;
  inst->logicalLine = this->logicalLine;
  OutputFile.text << inst;
  OutputFile.text << mov;
  output.size = expr.size;
  output.type = expr.type;
  return output;
}
}  // namespace gen
