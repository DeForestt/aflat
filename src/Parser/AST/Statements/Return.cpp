#include "Parser/AST/Statements/Return.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/Parser.hpp"

namespace ast {
/*
 * @brief This will parse a return statement
 * @param tokens The tokens to parse
 * @param pa
 */
Return::Return(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  if (!(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
        dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ';')) {
    this->expr = parser.parseExpr(tokens);
    this->logicalLine = this->expr->logicalLine;
  } else {
    auto nu = new ast::Var();
    nu->Ident = "**void_type**";
    nu->logicalLine = tokens.peek()->lineCount;
    this->expr = nu;
  };
}

gen::GenerationResult const Return::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  auto mov = new asmc::Mov();
  mov->logicalLine = this->logicalLine;

  gen::Expr from = generator.GenExpr(this->expr, file);

  if (generator.currentFunction->isLambda &&
      generator.lambdaReturns == "void") {
    // adopt the new return type
    generator.lambdaReturns = from.type;
    generator.lambdaSize = from.size;
    generator.returnType = ast::Type(from.type, from.size);
  }

  if (!from.passable) {
    generator.alert("cannot return an lvalue reference to safe type " +
                    from.type);
  };

  if (from.op != asmc::Float) {
    // move from.access to %rax
    auto mov2 = new asmc::Mov();
    mov2->logicalLine = this->logicalLine;
    mov2->size = from.size;
    mov2->from = from.access;
    mov2->to = generator.registers["%rax"]->get(from.size);
    mov2->op = from.op;
    file.text << mov2;
    auto push = new asmc::Push();
    push->logicalLine = this->logicalLine;
    push->op = generator.registers["%rax"]->get(asmc::QWord);
    file.text << push;
  };

  if (!generator.canAssign(generator.returnType, from.type,
                           "the return type of this function is {} but the "
                           "expression returns {}")) {
    auto imp = generator.imply(this->expr, generator.returnType.typeName);
    from = generator.GenExpr(imp, file);
  };

  if (from.op != asmc::Float) {
    auto pop = new asmc::Pop();
    pop->logicalLine = this->logicalLine;
    pop->op = generator.registers["%rax"]->get(asmc::QWord);
    file.text << pop;
  };

  std::string move2 = (from.op == asmc::Float)
                          ? generator.registers["%xmm0"]->get(from.size)
                          : generator.registers["%rax"]->get(from.size);

  mov->from = from.access;
  mov->to = move2;
  mov->size = from.size;
  mov->op = from.op;
  file.text << mov;

  gen::scope::ScopeManager::getInstance()->softPop(&generator, file);

  auto re = new asmc::Return();
  re->logicalLine = this->logicalLine;
  file.text << re;
  return {file, std::nullopt};
}
}  // namespace ast