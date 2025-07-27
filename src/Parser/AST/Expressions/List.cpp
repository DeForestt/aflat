#include "Parser/AST/Expressions/List.hpp"

#include "ASM.hpp"
#include "CodeGenerator/CodeGenerator.hpp"
#include "Parser/AST.hpp"

namespace ast {
List::List(links::LinkedList<lex::Token *> &tokens, parse::Parser &parser) {
  tokens.pop();  // pop the opening '['
  do {
    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
        dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ']') {
      tokens.pop();  // pop the closing ']'
      break;
    }
    auto item = parser.parseExpr(tokens);
    this->items.push_back(item);
    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr &&
        dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',') {
      tokens.pop();  // pop the comma
    }
  } while (true);
}

gen::GenerationResult const List::generateExpression(
    gen::CodeGenerator &generator, asmc::Size size, std::string typeHint) {
  asmc::File file;

  // get the type of the first item This will be the type of the list
  if (!typeHint.empty() && typeHint != "let") {
    // if typeHint is provided, we will use it as the type of the List
    // The typehint will be vector.typeName we just want the typeName part
    //
    if (typeHint.find('.') == std::string::npos) {
      generator.alert("Vector type hint given: `" + typeHint +
                      "` is unintelligible for a vector type.");
    }
    typeHint = typeHint.substr(typeHint.find('.') + 1);
  }
  std::string type = typeHint;

  if (typeHint.empty() || typeHint == "let") {
    if (this->items.empty()) {
      generator.alert(
          "List is empty, cannot determine the type of an empty list, please "
          "provide an explicit type.");
    }
    auto firstItem = dynamic_cast<ast::Expr *>(ast::deepCopy(this->items[0]));
    asmc::File junckFile;
    auto typeCheck = generator.GenExpr(firstItem, junckFile, size, typeHint);
    type = typeCheck.type;
  };

  auto newExpr = new ast::NewExpr();
  newExpr->type.typeName = "vector";
  newExpr->type.size = asmc::QWord;
  newExpr->templateTypes.push_back(type);

  ast::Expr *builder = newExpr;

  for (auto &item : this->items) {
    auto callExpr = new ast::CallExpr();
    callExpr->logicalLine = this->logicalLine;
    callExpr->call = new ast::Call();
    callExpr->call->ident = "push_back";
    callExpr->call->Args = links::LinkedList<ast::Expr *>();
    callExpr->call->Args.push(item);
    callExpr->call->modList = links::LinkedList<std::string>();
    builder->extention = callExpr;
    builder = callExpr;
  }

  auto res = generator.GenExpr(newExpr, file, size, typeHint);
  return gen::GenerationResult{
      .file = file,
      .expr = res,
  };
}
}  // namespace ast
