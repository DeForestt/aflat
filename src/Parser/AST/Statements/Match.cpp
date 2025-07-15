#include "Parser/AST/Statements/Match.hpp"

#include <algorithm>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "ASM.hpp"
#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Types.hpp"
#include "Parser/Parser.hpp"

namespace ast {

Match::Pattern::Pattern(links::LinkedList<lex::Token *> &tokens) {
  auto name = dynamic_cast<lex::LObj *>(tokens.pop());
  if (name == nullptr) {
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Match pattern must have an alias name.");
  }
  aliasName = name->meta;
  veriableName = std::nullopt;
  if (auto openParen = dynamic_cast<lex::OpSym *>(tokens.peek());
      openParen != nullptr && openParen->Sym == '(') {
    tokens.pop();
    auto veriableToken = dynamic_cast<lex::LObj *>(tokens.pop());
    if (veriableToken == nullptr) {
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Match pattern must have a variable name.");
    }
    veriableName = veriableToken->meta;
    auto closeParen = dynamic_cast<lex::OpSym *>(tokens.pop());
    if (closeParen == nullptr || closeParen->Sym != ')') {
      throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                           " Match pattern must have a closing parenthesis.");
    }
  }
}

Match::Case::Case(links::LinkedList<lex::Token *> &tokens,
                  parse::Parser &parser) {
  pattern = Match::Pattern(tokens);
  auto token = tokens.pop();
  if (token == nullptr) {
    throw err::Exception("Unexpected end of tokens while parsing match case.");
  }
  const auto equalsSym = dynamic_cast<lex::OpSym *>(token);
  if (equalsSym == nullptr || equalsSym->Sym != '=') {
    throw err::Exception("Match case must have a `=>` symbol.");
  }
  const auto carrot = dynamic_cast<lex::Symbol *>(tokens.pop());
  if (carrot == nullptr || carrot->meta != ">") {
    throw err::Exception("Match case must have a `=>` symbol.");
  }

  auto openBrace = dynamic_cast<lex::OpSym *>(tokens.peek());
  if (openBrace && openBrace->Sym == '{') {
    statement = parser.parseStmt(tokens);
  } else {
    statement = parser.parseStmt(tokens, true);
  }
}

std::vector<Match::Case> const Match::parseCases(
    links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  std::vector<Match::Case> cases;
  bool firstCase = true;

  do {
    if (!firstCase) {
      tokens.pop();
    } else {
      firstCase = false;
    }
    // if we have a close brace, there is a trailing comma (Which we want to
    // allow so we can break)
    auto closeBrace = dynamic_cast<lex::OpSym *>(tokens.peek());
    if (closeBrace && closeBrace->Sym == '}') {
      break;
    }
    // Now we can parse the case
    cases.push_back(Match::Case(tokens, parser));
  } while (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
           dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
  return cases;
}

Match::Match(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  expr = parser.parseExpr(tokens);
  auto openBrace = dynamic_cast<lex::OpSym *>(tokens.pop());
  if (openBrace == nullptr || openBrace->Sym != '{') {
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Match statement must start with an opening brace.");
  }
  cases = parseCases(tokens, parser);
  auto closeBrace = dynamic_cast<lex::OpSym *>(tokens.pop());
  if (closeBrace == nullptr || closeBrace->Sym != '}') {
    throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +
                         " Match statement must end with a closing brace.");
  }
}

gen::GenerationResult const Match::generate(gen::CodeGenerator &generator) {
  asmc::File file;

  auto exprResult = generator.GenExpr(expr, file);
  auto t = generator.getType(exprResult.type, file);

  if (!t) {
    generator.alert("Match expression type not found: " + exprResult.type, true,
                    __FILE__, __LINE__);
    return {.file = file, .expr = std::nullopt};
  }

  auto type = *t;
  auto unionType = dynamic_cast<gen::Union *>(type);
  if (!unionType) {
    generator.alert("Match expression type is not a union: " + exprResult.type,
                    true, __FILE__, __LINE__);
    return {.file = file, .expr = std::nullopt};
  }

  auto matchID = boost::uuids::to_string(boost::uuids::random_generator()());
  matchID.erase(std::remove(matchID.begin(), matchID.end(), '-'),
                matchID.end());

  std::vector<std::string> jmpLabels;
  std::transform(unionType->aliases.begin(), unionType->aliases.end(),
                 std::back_inserter(jmpLabels), [&](const auto &alias) {
                   return ".match_arm_" + matchID + "_" + alias.name;
                 });
  std::string matchEndLabel = ".match_end_" + matchID;

  // push rdx
  auto pushRdx = new asmc::Push();
  pushRdx->logicalLine = expr->logicalLine;
  pushRdx->size = asmc::QWord;
  pushRdx->op = generator.registers["%rdx"]->get(asmc::QWord);
  file.text << pushRdx;

  // move the output to eax;
  auto move = new asmc::Mov();
  move->logicalLine = expr->logicalLine;
  move->size = asmc::QWord;
  move->to = generator.registers["%rdx"]->get(asmc::QWord);
  move->from = exprResult.access;
  file.text << move;

  // move to int at the address of the union + offset into eax
  auto moveTypeTag = new asmc::Mov();
  moveTypeTag->logicalLine = expr->logicalLine;
  moveTypeTag->size = asmc::DWord;
  moveTypeTag->to = generator.registers["%rax"]->get(asmc::DWord);
  moveTypeTag->from = std::to_string(unionType->largestSize) + "(" +
                      generator.registers["%rdx"]->get(asmc::QWord) + ")";
  file.text << moveTypeTag;

  for (auto i = 0; i < jmpLabels.size(); i++) {
    auto &alias = unionType->aliases[i];
    auto cmp = new asmc::Cmp();
    cmp->logicalLine = expr->logicalLine;
    cmp->to = generator.registers["%rax"]->get(asmc::DWord);
    cmp->size = asmc::DWord;
    cmp->from = "$" + std::to_string(i);
    file.text << cmp;
    auto jne = new asmc::Jne();
    jne->to = jmpLabels[i];
    jne->logicalLine = expr->logicalLine;
    file.text << jne;

    // find the case for this alias type
    auto caseIt =
        std::find_if(cases.begin(), cases.end(), [&](const Match::Case &c) {
          return c.pattern.aliasName == alias.name;
        });
    if (caseIt == cases.end()) {
      generator.alert("Match case not found for alias: " + alias.name, true,
                      __FILE__, __LINE__);
      continue;
    }

    // push the scope for the case
    gen::scope::ScopeManager::getInstance()->pushScope(true);
    auto &_case = *caseIt;
    if (_case.pattern.veriableName.has_value()) {
      if (std::holds_alternative<ast::Type *>(alias.value)) {
        // if the alias value is a type, we need to declare a variable of that
        // type

        auto &type = std::get<ast::Type *>(alias.value);
        auto byteMod = gen::scope::ScopeManager::getInstance()->assign(
            *_case.pattern.veriableName, *type, false, false);

        if (parse::PRIMITIVE_TYPES.find(type->typeName) !=
            parse::PRIMITIVE_TYPES.end()) {
          // primitives will need to be dereferenced
          auto mov = new asmc::Mov();
          mov->logicalLine = expr->logicalLine;
          mov->size = type->size;
          mov->from = "(" + generator.registers["%rdx"]->get(asmc::QWord) + ")";
          mov->to = generator.registers["%eax"]->get(type->size);
          file.text << mov;

          auto mov2 = new asmc::Mov();
          mov2->logicalLine = expr->logicalLine;
          mov2->size = type->size;
          mov2->from = generator.registers["%eax"]->get(type->size);
          mov2->to = "-" + std::to_string(byteMod) + "(%rbp)";
          file.text << mov2;

        } else {
          auto mov = new asmc::Mov();
          mov->logicalLine = expr->logicalLine;
          mov->size = type->size;
          mov->from = generator.registers["%rdx"]->get(type->size);
          mov->to = "-" + std::to_string(byteMod) + "(%rbp)";
          file.text << mov;
        };

      } else if (std::holds_alternative<ast::Expr *>(alias.value)) {
        auto &expr = std::get<ast::Expr *>(alias.value);
        // if the alias value is an expression, we need to declare a variable of
        // that Type
        auto decAssign = new ast::DecAssign();
        decAssign->logicalLine = expr->logicalLine;
        decAssign->declare = new ast::Declare();
        decAssign->declare->type = ast::Type("let", asmc::AUTO);
        decAssign->declare->ident = _case.pattern.veriableName.value();
        decAssign->declare->mut = false;
        decAssign->expr = expr;
        file << decAssign->generate(generator).file;
      }
    } else
      std::cout << "Alias has no variable name." << std::endl;
    file << _case.statement->generate(generator).file;
    auto jmp = new asmc::Jmp();
    jmp->logicalLine = expr->logicalLine;
    jmp->to = matchEndLabel;
    file.text << jmp;
    // pop the scope for the cases
    gen::scope::ScopeManager::getInstance()->popScope(&generator, file);
    auto lable = new asmc::Label();
    lable->logicalLine = expr->logicalLine;
    lable->label = jmpLabels[i];
    file.text << lable;
  }

  // add the end Label
  auto endLabel = new asmc::Label();
  endLabel->logicalLine = expr->logicalLine;
  endLabel->label = matchEndLabel;
  file.text << endLabel;

  // pop popRdx
  auto popRdx = new asmc::Pop();
  popRdx->logicalLine = expr->logicalLine;
  popRdx->size = asmc::QWord;
  popRdx->op = generator.registers["%rdx"]->get(asmc::QWord);
  file.text << popRdx;

  return {file, std::nullopt};
}

}  // namespace ast
