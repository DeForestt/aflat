#include "Parser/AST/Statements/DecAssign.hpp"

#include "CodeGenerator/CodeGenerator.hpp"
#include "CodeGenerator/Conversions.hpp"
#include "CodeGenerator/Scope/ScopeManager.hpp"
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
  if (!generator.globalScope()) {
    if (generator.scope() == nullptr || generator.inFunction()) {
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
            generator.alert("Cannot create a mutable reference to a immutable "
                            "variable " +
                            var->Ident);
          }

          ref->Ident = var->Ident;
          ref->modList = var->modList;
          ref->logicalLine = var->logicalLine;
          this->expr = ref;
        } else {
          if (!var)
            allowAdr;
        }
      }

      if (dec->type.typeName != "let" &&
          parse::PRIMITIVE_TYPES.find(dec->type.typeName) ==
              parse::PRIMITIVE_TYPES.end() &&
          dec->type.typeName != "any" &&
          !(dec->type.typeName.find("~") != std::string::npos &&
            dec->type.typeName.find("<") == std::string::npos)) {
        generator.getType(dec->type.typeName, file);
      }

      auto mov = new asmc::Mov();
      mov->logicalLine = this->logicalLine;
      const bool inferredDeclaration = dec->TypeName.empty() ||
                                       dec->TypeName == "let" ||
                                       dec->type.typeName == "let";
      gen::Expr expr = generator.GenExpr(this->expr, file, dec->type.size,
                                         dec->type.typeName, dec->local);
      if (dec->local && expr.storageOrigin != gen::StorageOrigin::Stack)
        generator.alert("a local binding requires a stack-local value", true,
                        __FILE__, __LINE__);

      // Integer declarations route through the conversion classifier:
      // a literal source is judged by value, a typed integer source by
      // range. Lossless conversions emit a real extension in the store
      // sequence below; checked-required conversions are diagnosed
      // here instead of falling through to canAssign.
      const gen::utils::IntTypeInfo *decIntInfo =
          gen::utils::intTypeInfo(dec->type.typeName);
      const gen::utils::IntTypeInfo *exprIntInfo = nullptr;
      gen::utils::ConversionKind intConversion =
          gen::utils::ConversionKind::NotConvertible;
      if (decIntInfo != nullptr && dec->type.typeName != "let") {
        std::int64_t literalValue = 0;
        if (gen::utils::integerLiteralValue(this->expr, literalValue)) {
          const bool literalFitsDest =
              gen::utils::literalFits(literalValue, *decIntInfo) ||
              (decIntInfo->widthBytes == 8 &&
               gen::utils::longLiteralSource(this->expr));
          intConversion = literalFitsDest
                              ? gen::utils::ConversionKind::LiteralFits
                              : gen::utils::ConversionKind::CheckedRequired;
          if (intConversion == gen::utils::ConversionKind::CheckedRequired &&
              !dec->trust)
            generator.alert(gen::utils::literalRangeDiagnostic(
                                literalValue, dec->type.typeName),
                            true, __FILE__, __LINE__);
        } else if ((exprIntInfo = gen::utils::intTypeInfo(expr.type)) !=
                   nullptr) {
          intConversion =
              gen::utils::classifyConversion(*exprIntInfo, *decIntInfo);
          if (intConversion == gen::utils::ConversionKind::CheckedRequired &&
              !dec->trust)
            generator.alert(gen::utils::implicitConversionDiagnostic(
                                expr.type, dec->type.typeName),
                            true, __FILE__, __LINE__);
        } else if (expr.type == "number" && expr.op != asmc::Float) {
          // A computed value of literal provenance has i32 provenance:
          // classify it as i32 so a widening declaration sign-extends
          // it (the literal-led-negative class) instead of relying on
          // the zeroing store. Checked-required destinations keep the
          // legacy path for now.
          exprIntInfo = gen::utils::intTypeInfo("i32");
          const gen::utils::ConversionKind asI32 =
              gen::utils::classifyConversion(*exprIntInfo, *decIntInfo);
          if (asI32 == gen::utils::ConversionKind::Lossless ||
              asI32 == gen::utils::ConversionKind::Identity)
            intConversion = asI32;
          else
            exprIntInfo = nullptr;
        }
      }
      const bool intConversionHandled =
          intConversion == gen::utils::ConversionKind::Identity ||
          intConversion == gen::utils::ConversionKind::Lossless ||
          intConversion == gen::utils::ConversionKind::LiteralFits;

      const auto testType =
          allowAdr ? ast::Type("adr", asmc::QWord) : dec->type;
      if (!intConversionHandled && !dec->trust && dec->type.typeName != "let" &&
          !generator.canAssign(testType, expr.type,
                               "type {} cannot be assigned to type {}")) {
        auto prev = expr;
        expr = generator.GenExpr(generator.imply(this->expr, testType.typeName),
                                 file, testType.size, testType.typeName);
        expr.adoptImmutableRequirement(prev);
      }

      if (dec->type.typeName == "let") {
        if (expr.type == "generic" || expr.type == "any" ||
            expr.type == "let") {
          generator.alert("Cannot infer type of variable because the "
                          "expression is of type " +
                          expr.type);
        }
        if (expr.type == "number") {
          // A literal bound without a destination keeps the i32
          // default; one outside that range is diagnosed instead of
          // silently changing width.
          std::int64_t literalValue = 0;
          if (gen::utils::integerLiteralValue(this->expr, literalValue) &&
              !gen::utils::literalFits(literalValue,
                                       *gen::utils::intTypeInfo("i32")))
            generator.alert(
                gen::utils::literalRangeDiagnostic(literalValue, "i32"), true,
                __FILE__, __LINE__);
        }
        dec->type.typeName = expr.type;
        dec->type.size = expr.size;
        dec->type.opType = expr.op;
        if (expr.type.find("~") != std::string::npos &&
            expr.type.find("<") == std::string::npos) {
          // find the type in TypeList
          auto t = generator.TypeList()[expr.type];
          if (t == nullptr)
            generator.alert("Function Pointer type not found " + expr.type +
                            " on line " + std::to_string(this->logicalLine));
          dec->type = *t;
        }
      }
      if (inferredDeclaration)
        generator.reportInferredType(dec->ident, dec->type, this->logicalLine);

      const bool inferredPrimitive = parse::PRIMITIVE_TYPES.find(expr.type) !=
                                     parse::PRIMITIVE_TYPES.end();
      if (expr.requiresImmutableBinding && !inferredPrimitive &&
          !this->declare->readOnly) {
        auto source = expr.immutableBindingSource.empty()
                          ? std::string("this expression")
                          : expr.immutableBindingSource;
        generator.alert("value produced by `" + source +
                            "` must be bound to an immutable symbol",
                        true, __FILE__, __LINE__);
      }

      int byteMod = gen::scope::ScopeManager::getInstance()->assign(
          dec->ident, dec->type, this->declare->mask, this->mute,
          this->declare->readOnly);
      auto s = gen::scope::ScopeManager::getInstance()->get(dec->ident);
      s->usable = false;
      s->refCount--;

      auto mov2 = new asmc::Mov();
      mov2->logicalLine = this->logicalLine;
      auto isConcreteSize = [](asmc::Size size) {
        return size == asmc::Byte || size == asmc::Word ||
               size == asmc::DWord || size == asmc::QWord;
      };
      auto isDWordRegisterOrImmediate = [](const std::string &access) {
        return access.rfind("$", 0) == 0 || access.rfind("%e", 0) == 0 ||
               (access.size() > 2 && access[0] == '%' && access.back() == 'd');
      };
      const bool widenInteger =
          expr.op != asmc::Float && isConcreteSize(expr.size) &&
          isConcreteSize(dec->type.size) && expr.size < dec->type.size;
      asmc::Size loadSize =
          isConcreteSize(expr.size) ? expr.size : dec->type.size;
      if (widenInteger && loadSize < asmc::DWord &&
          isDWordRegisterOrImmediate(expr.access)) {
        loadSize = asmc::DWord;
      }

      mov2->size = loadSize;
      mov2->from = expr.access;
      if (expr.op == asmc::Float)
        mov2->to = generator.registers()["%xmm0"]->get(expr.size);
      else
        mov2->to = generator.registers()["%rbx"]->get(loadSize);

      mov->op = expr.op;

      mov2->op = expr.op;
      mov->size = dec->type.size;
      mov->from = generator.registers()["%rbx"]->get(dec->type.size);
      mov->to = "-" + std::to_string(byteMod) + "(%rbp)";

      if (!widenInteger) {
        if (expr.op == asmc::Float) {
          mov->from = mov2->to;
        } else {
          mov->from = generator.registers()["%rbx"]->get(dec->type.size);
        }
      }

      if (widenInteger) {
        auto zero = new asmc::Xor();
        zero->logicalLine = this->logicalLine;
        zero->op1 = generator.registers()["%rbx"]->get(asmc::QWord);
        zero->op2 = generator.registers()["%rbx"]->get(asmc::QWord);
        zero->size = asmc::QWord;
        file.text << zero;
      }
      file.text << mov2;
      if (intConversion == gen::utils::ConversionKind::Lossless &&
          exprIntInfo != nullptr && decIntInfo != nullptr &&
          exprIntInfo->isSigned &&
          exprIntInfo->widthBytes < decIntInfo->widthBytes)
        generator.emitIntegerExtension(file, "%rbx", *exprIntInfo, *decIntInfo);
      file.text << mov;
      s->owned = expr.owned && !dec->type.isLoan;
      s->type.isLocal = dec->local;
      s->needsDrop = expr.needsDrop && !dec->type.isLoan;
      s->storageOrigin = expr.storageOrigin;
      s->storageScope = expr.storageScope;
      s->stackObjectOffset = expr.stackObjectOffset;
      s->stackCleanupNodeOffset = expr.stackCleanupNodeOffset;
      if (expr.loanProvenance != gen::LoanProvenance::None) {
        s->loanProvenance = expr.loanProvenance;
        s->loanScope = expr.loanScope;
      }
      s->usable = true;
    } else {
      if (this->annotations.size() > 0) {
        for (auto &annotation : this->annotations) {
          if (generator.transforms().find(annotation.name) ==
              generator.transforms().end()) {
            generator.alert("Template " + annotation.name + " not found");
          }
          auto expr_str = this->expr->toString();
          auto transform = generator.transforms()[annotation.name];
          std::string mut = this->mute ? "mutable" : "const";
          std::string scope =
              this->declare->scope == ast::Public ? "public" : "private";

          std::unordered_map<std::string, std::string> args;
          for (auto &arg : annotation.args) {
            std::istringstream iss(arg);
            std::string key;
            std::string value;
            std::getline(iss, key, '_');
            std::getline(iss, value, '_');
            args[key] = value;
          }
          for (auto &arg : annotation.namedArgs) {
            args[arg.first] = arg.second;
          }

          auto result = transform.parse(declare->ident, declare->type.typeName,
                                        expr_str, scope, mut, args, generator);
          if (!result) {
            generator.alert("Template " + annotation.name + " failed to parse");
          }
          if (generator.scope() != nullptr &&
              generator.scope()->declarationOnly) {
            ast::Statement *declarations =
                gen::utils::extractAllDeclarations(result);
            ast::Statement *functionShells =
                gen::utils::copyAllFunctionShells(result);

            ast::Statement *shellOnly = nullptr;
            if (declarations != nullptr && functionShells != nullptr) {
              auto *seq = new ast::Sequence();
              seq->Statement1 = declarations;
              seq->Statement2 = functionShells;
              shellOnly = seq;
            } else if (declarations != nullptr) {
              shellOnly = declarations;
            } else {
              shellOnly = functionShells;
            }

            if (shellOnly != nullptr) {
              file << shellOnly->generate(generator).file;
            }
          } else {
            file << result->generate(generator).file;
          }
        }
      } else {
        // add the this to the class default list
        generator.scope()->defaultValues.push_back(*this);
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
    Symbol.readOnly = this->declare->readOnly;
    auto Table = &generator.GlobalSymbolTable();

    auto var = new asmc::LinkTask();
    auto label = new asmc::Label();

    label->label = dec->ident;
    if (dec->type.size = asmc::QWord) {
      var->command = "quad";
    }
    gen::Expr exp = generator.GenExpr(this->expr, file);
    // Global integer initializers are constants: literals are judged
    // by value and checked-required conversions are always diagnosed
    // at compile time.
    const gen::utils::IntTypeInfo *globalIntInfo =
        gen::utils::intTypeInfo(dec->type.typeName);
    bool globalIntHandled = false;
    if (globalIntInfo != nullptr) {
      std::int64_t literalValue = 0;
      if (gen::utils::integerLiteralValue(this->expr, literalValue)) {
        if (!gen::utils::literalFits(literalValue, *globalIntInfo) &&
            !(globalIntInfo->widthBytes == 8 &&
              gen::utils::longLiteralSource(this->expr)))
          generator.alert(gen::utils::literalRangeDiagnostic(
                              literalValue, dec->type.typeName),
                          true, __FILE__, __LINE__);
        globalIntHandled = true;
      } else if (const gen::utils::IntTypeInfo *expIntInfo =
                     gen::utils::intTypeInfo(exp.type)) {
        const gen::utils::ConversionKind kind =
            gen::utils::classifyConversion(*expIntInfo, *globalIntInfo);
        if (kind == gen::utils::ConversionKind::CheckedRequired)
          generator.alert(gen::utils::implicitConversionDiagnostic(
                              exp.type, dec->type.typeName),
                          true, __FILE__, __LINE__);
        globalIntHandled = kind == gen::utils::ConversionKind::Identity;
      }
    }
    if (!globalIntHandled &&
        !generator.canAssign(
            dec->type, exp.type,
            "cannot assign type {} cannot be assigned to type {}")) {
      auto prev = exp;
      exp = generator.GenExpr(generator.imply(this->expr, dec->type.typeName),
                              file);
      exp.adoptImmutableRequirement(prev);
    };
    const bool inferredPrimitive =
        parse::PRIMITIVE_TYPES.find(exp.type) != parse::PRIMITIVE_TYPES.end();
    if (exp.requiresImmutableBinding && !inferredPrimitive &&
        !this->declare->readOnly) {
      auto source = exp.immutableBindingSource.empty()
                        ? std::string("this expression")
                        : exp.immutableBindingSource;
      generator.alert("value produced by `" + source +
                          "` must be bound to an immutable symbol",
                      true, __FILE__, __LINE__);
    }
    var->operand = exp.access.erase(0, 1);
    Symbol.type.opType = exp.op;
    Symbol.owned = exp.owned && !dec->type.isLoan;

    generator.validateLoanAssignment(exp, Symbol);

    file.data << label;
    file.data << var;
    if (Table->search<std::string>(gen::utils::searchSymbol, dec->ident) !=
        nullptr)
      generator.alert("redefined variable:" + dec->ident);
    Table->push(Symbol);
  };

  if (dec->type.fPointerArgs.isFPointer &&
      generator.typeList()[dec->TypeName] == nullptr &&
      this->annotations.size() == 0) {
    auto newType = new ast::Type();
    newType->typeName = dec->TypeName;
    newType->size = asmc::QWord;
    newType->fPointerArgs = dec->type.fPointerArgs;
    generator.TypeList().push(*newType);
  }
  return {file, std::nullopt};
}

} // namespace ast
