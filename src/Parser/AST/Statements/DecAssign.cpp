#include "Parser/AST/Statements/DecAssign.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/Parser.hpp"

namespace ast {
void DecAssign::debug() {
  std::cout << "DecAssign: " << this->declare->ident << " " << this->mute << " "
            << this->locked << std::endl;
  std::cout << "Expr: " << this->expr->toString() << std::endl;
}
DecAssign::DecAssign(Declare *declare, const bool mute,
                     links::LinkedList<lex::Token *> &tokens,
                     parse::Parser &parser,
                     const std::vector<parse::Annotation> &annotations)
    : declare(declare), mute(mute), annotations(annotations) {
  this->expr = parser.parseExpr(tokens);
  this->logicalLine = this->expr->logicalLine;
}

gen::GenerationResult const DecAssign::generate(gen::CodeGenerator &generator) {
  asmc::File file;
  ast::Declare *dec = this->declare;
  bool allowAdr = false;
  if (!generator.globalScope) {
    if (generator.scope == nullptr || generator.inFunction) {
      if (dec->type.isReference) {
        auto var = dynamic_cast<ast::Var *>(this->expr);
        if (var && !var->clean) {
          ast::Reference *ref = new ast::Reference();
          asmc::File file;
          auto resolved = generator.resolveSymbol(
              var->Ident, var->modList, file, links::LinkedList<ast::Expr *>());
          if (!std::get<2>(resolved)) {
            generator.alert("Variable " + var->Ident + " not found");
          }

          if (dec->mut && !std::get<1>(resolved).mutable_) {
            generator.alert(
                "Cannot create a mutable reference to a immutable "
                "variable " +
                var->Ident);
          }

          ref->Ident = var->Ident;
          ref->modList = var->modList;
          ref->logicalLine = var->logicalLine;
          this->expr = ref;
        } else {
          if (!var) allowAdr;
        }
      }

      auto mov = new asmc::Mov();
      mov->logicalLine = this->logicalLine;
      gen::Expr expr = generator.GenExpr(this->expr, file, dec->type.size);

      const auto testType =
          allowAdr ? ast::Type("adr", asmc::QWord) : dec->type;
      if (!dec->trust) {
        if (dec->type.typeName != "let" &&
                !generator.canAssign(testType, expr.type,
                                     "type {} cannot be assigned to type {}"),
            dec->trust) {
          expr = generator.GenExpr(
              generator.imply(this->expr, testType.typeName), file);
        };
      };

      if (dec->type.typeName == "let") {
        if (expr.type == "generic" || expr.type == "any" ||
            expr.type == "let") {
          generator.alert(
              "Cannot infer type of variable because the "
              "expression is of type " +
              expr.type);
        }
        dec->type.typeName = expr.type;
        dec->type.size = expr.size;
        dec->type.opType = expr.op;
        if (expr.type.find("~") != std::string::npos) {
          // find the type in TypeList
          auto t = generator.TypeList[expr.type];
          if (t == nullptr)
            generator.alert("Function Pointer type not found " + expr.type +
                            " on line " + std::to_string(this->logicalLine));
          dec->type = *t;
        }
      }

      int byteMod = gen::scope::ScopeManager::getInstance()->assign(
          dec->ident, dec->type, this->declare->mask, this->mute);
      auto s = gen::scope::ScopeManager::getInstance()->get(dec->ident);
      s->usable = false;
      s->refCount--;

      auto mov2 = new asmc::Mov();
      mov2->logicalLine = this->logicalLine;
      mov2->size = dec->type.size;
      mov2->from = expr.access;
      if (expr.op == asmc::Float)
        mov2->to = generator.registers["%xmm0"]->get(expr.size);
      else
        mov2->to = generator.registers["%rbx"]->get(dec->type.size);

      mov->op = expr.op;

      mov2->op = expr.op;
      mov->size = dec->type.size;
      mov->from = generator.registers["%rbx"]->get(dec->type.size);
      mov->to = "-" + std::to_string(byteMod) + "(%rbp)";

      mov->from = mov2->to;

      file.text << mov2;
      file.text << mov;
      s->usable = true;
    } else {
      if (this->annotations.size() > 0) {
        for (auto &annotation : this->annotations) {
          if (generator.transforms.find(annotation.name) ==
              generator.transforms.end()) {
            generator.alert("Template " + annotation.name + " not found");
          }
          auto expr_str = this->expr->toString();
          auto transform = generator.transforms[annotation.name];
          std::string mut = this->mute ? "mutable" : "const";
          std::string scope =
              this->declare->scope == ast::Public ? "public" : "private";
          auto result = transform.parse(declare->ident, declare->type.typeName,
                                        expr_str, scope, mut, generator);
          if (!result) {
            generator.alert("Template " + annotation.name + " failed to parse");
          }
          file << result->generate(generator).file;
        }
      } else {
        // add the this to the class default list
        generator.scope->defaultValues.push_back(*this);
        // generate the declare
        dec->mut = this->mute;
        file << generator.GenSTMT(dec);
      };
    }
  } else {
    gen::Symbol Symbol;

    Symbol.type = dec->type;
    Symbol.symbol = dec->ident;
    Symbol.mutable_ = this->mute;
    auto Table = &generator.GlobalSymbolTable;

    auto var = new asmc::LinkTask();
    auto label = new asmc::Label();

    label->label = dec->ident;
    if (dec->type.size = asmc::QWord) {
      var->command = "quad";
    }
    gen::Expr exp = generator.GenExpr(this->expr, file);
    if (!generator.canAssign(
            dec->type, exp.type,
            "cannot assign type {} cannot be assigned to type {}")) {
      exp = generator.GenExpr(generator.imply(this->expr, dec->type.typeName),
                              file);
    };
    var->operand = exp.access.erase(0, 1);
    Symbol.type.opType = exp.op;
    file.data << label;
    file.data << var;
    if (Table->search<std::string>(gen::utils::searchSymbol, dec->ident) !=
        nullptr)
      generator.alert("redefined variable:" + dec->ident);
    Table->push(Symbol);
  };

  if (dec->type.fPointerArgs.isFPointer &&
      generator.typeList[dec->TypeName] == nullptr &&
      this->annotations.size() == 0) {
    auto newType = new ast::Type();
    newType->typeName = dec->TypeName;
    newType->size = asmc::QWord;
    newType->fPointerArgs = dec->type.fPointerArgs;
    generator.TypeList.push(*newType);
  }
  return {file, std::nullopt};
}

}  // namespace ast