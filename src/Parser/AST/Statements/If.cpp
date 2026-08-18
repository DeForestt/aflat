#include "Parser/AST/Statements/If.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "Parser/AST/Statements/Return.hpp"
#include "Parser/AST/Statements/Sequence.hpp"

namespace {
bool definitelyReturns(const ast::Statement *statement) {
  if (statement == nullptr)
    return false;
  if (dynamic_cast<const ast::Return *>(statement) != nullptr)
    return true;
  if (const auto *sequence = dynamic_cast<const ast::Sequence *>(statement))
    return definitelyReturns(sequence->Statement1) ||
           definitelyReturns(sequence->Statement2);
  if (const auto *branch = dynamic_cast<const ast::If *>(statement))
    return branch->elseStatement != nullptr &&
           definitelyReturns(branch->statement) &&
           definitelyReturns(branch->elseStatement);
  return false;
}
} // namespace

namespace ast {
/*
 * @brief This will parse an if statement
 * @param tokens The tokens to parse\
 * @param parser The parser to use
 */
If::If(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  this->elseIf = nullptr;
  this->elseStatement = nullptr;
  this->expr = parser.parseExpr(tokens);
  this->logicalLine = this->expr->logicalLine;
  auto sym = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (sym != nullptr) {
    if (sym->Sym == '{') {
      tokens.pop();
      this->statement = parser.parseStmt(tokens);
    } else
      throw err::Exception(
          "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
          " Unopened If");
  } else if (dynamic_cast<lex::LObj *>(tokens.peek())) {
    this->statement = parser.parseStmt(tokens, true);
  } else
    throw err::Exception(
        "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
        " Unopened If");
  if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
    auto elseObj = *dynamic_cast<lex::LObj *>(tokens.peek());
    if (elseObj.meta == "else") {
      tokens.pop();
      if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
        sym = dynamic_cast<lex::OpSym *>(tokens.pop());
        if (sym->Sym == '{') {
          this->elseStatement = parser.parseStmt(tokens);
        } else
          throw err::Exception(
              "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
              " Unopened Else");
      } else if (dynamic_cast<lex::LObj *>(tokens.peek())) {
        this->elseStatement = parser.parseStmt(tokens, true);
      } else
        throw err::Exception(
            "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
            " Unopened Else");
    } else
      throw err::Exception(
          "Line: " + std::to_string(lex::tokenLine(tokens.peek())) +
          " Unclosed if");
  }
}

gen::GenerationResult const If::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  auto *scope = gen::scope::ScopeManager::getInstance();
  scope->pushScope(false);

  asmc::Label *label1 = new asmc::Label();
  label1->logicalLine = this->logicalLine;
  label1->label = ".L" + generator.nameTable().head->data.ident.ident +
                  std::to_string(generator.labelCount());
  generator.labelCount()++;

  gen::Expr expr = generator.GenExpr(this->expr, file);
  const auto incomingOwnership = scope->captureOwnershipState();

  ast::Type t = ast::Type();
  t.typeName = "bool";
  t.size = asmc::Byte;
  generator.canAssign(t, expr.type,
                      "if conditions must be of type {}, {} was given.");

  asmc::Mov *mov1 = new asmc::Mov();
  mov1->logicalLine = this->logicalLine;

  mov1->size = asmc::Byte;

  mov1->from = expr.access;

  mov1->to = generator.registers()["%rax"]->get(mov1->size);

  asmc::Cmp *cmp = new asmc::Cmp();
  cmp->logicalLine = this->logicalLine;
  asmc::Je *je = new asmc::Je();
  je->logicalLine = this->logicalLine;

  cmp->from = "$0";
  cmp->to = mov1->to;
  cmp->size = expr.size;

  je->to = label1->label;

  file.text << mov1;
  file.text << cmp;
  file.text << je;
  file << generator.GenSTMT(this->statement);
  if (this->elseStatement != nullptr) {
    asmc::Label *end = new asmc::Label();
    end->logicalLine = this->logicalLine;
    end->label = ".L" + generator.nameTable().head->data.ident.ident +
                 std::to_string(generator.labelCount());
    generator.labelCount()++;
    asmc::Jmp *jmp = new asmc::Jmp();
    jmp->logicalLine = this->logicalLine;
    jmp->to = end->label;
    scope->popScope(&generator, file);
    const auto thenOwnership = scope->captureOwnershipState();
    scope->restoreOwnershipState(incomingOwnership);
    file.text << jmp;
    file.text << label1;

    scope->pushScope(true);
    file << generator.GenSTMT(this->elseStatement);
    scope->popScope(&generator, file);
    const auto elseOwnership = scope->captureOwnershipState();
    const bool thenReturns = definitelyReturns(this->statement);
    const bool elseReturns = definitelyReturns(this->elseStatement);
    if (thenReturns && elseReturns)
      scope->restoreOwnershipState(incomingOwnership);
    else if (thenReturns)
      scope->restoreOwnershipState(elseOwnership);
    else if (elseReturns)
      scope->restoreOwnershipState(thenOwnership);
    else
      scope->mergeOwnershipStates(thenOwnership, elseOwnership);
    file.text << end;
  } else {
    scope->popScope(&generator, file);
    const auto thenOwnership = scope->captureOwnershipState();
    if (definitelyReturns(this->statement))
      scope->restoreOwnershipState(incomingOwnership);
    else
      scope->mergeOwnershipStates(thenOwnership, incomingOwnership);
    file.text << label1;
  };
  generator.currentFunction()->has_return = false;
  return {file, std::nullopt};
};

} // namespace ast
