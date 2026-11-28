#include "CodeGenerator/CodeGenerator.hpp"

#include "CodeGenerator/Conversions.hpp"
#include "CodeGenerator/Scope/ScopeManager.hpp"
#include "CodeGenerator/Utils.hpp"
#include "Parser/AST.hpp"

// Explicit conversions: `expr as Type` and `expr as wrap Type`
// (design section 4).
//
// - On an adr operand, `as` keeps its dereference meaning: a read
//   through the pointer at the target type's width.
// - On an integer operand, the conversion is classified like every
//   other boundary. Identity and lossless conversions produce the
//   plain converted value; a checked-required conversion produces a
//   result::<T> -- Ok with the converted value, or Err with an Error
//   naming the value, both types, and the destination range -- via
//   the runtime range test emitted below. A constant operand is
//   judged at compile time instead: out of range is a diagnostic.
// - `as wrap` always produces the plain value at the destination
//   width: widened by the source's signedness, narrowed by taking
//   the low bits. It never produces a result and never fails.

namespace {

void stageToRax(gen::CodeGenerator &generator, asmc::File &file,
                const gen::Expr &value) {
  auto *mov = new asmc::Mov();
  mov->logicalLine = generator.logicalLine();
  mov->size = value.size;
  mov->op = value.op;
  mov->from = value.access;
  mov->to = generator.registers()["%rax"]->get(value.size);
  file.text << mov;
}

} // namespace

gen::Expr gen::CodeGenerator::genExplicitCast(ast::ExplicitCast *cast,
                                              asmc::File &OutputFile) {
  const std::string dstName = cast->targetType;
  const gen::utils::IntTypeInfo *dstInfo =
      gen::utils::classifiableIntType(dstName);

  // Constant operands are judged at compile time (design sections
  // 4.2 and 8): in range they are plain constants of the destination
  // type; out of range is a diagnostic, or a wrapping
  // materialization under `as wrap`.
  std::int64_t literalValue = 0;
  if (gen::utils::integerLiteralValue(cast->expr, literalValue)) {
    if (dstInfo == nullptr)
      alert("`as` conversion to " + dstName + " is not an integer conversion",
            true, __FILE__, __LINE__);
    const bool fits =
        gen::utils::literalFits(literalValue, *dstInfo) ||
        (dstInfo->widthBytes == 8 && gen::utils::longLiteralSource(cast->expr));
    if (!fits && !cast->wrap)
      alert(gen::utils::checkedConstantDiagnostic(
                literalValue,
                gen::utils::longLiteralSource(cast->expr) ? "i64" : "i32",
                dstName),
            true, __FILE__, __LINE__);
    gen::Expr lit =
        this->GenExpr(cast->expr, OutputFile,
                      gen::utils::toSize(dstInfo->widthBytes), dstName);
    lit.type = dstName;
    lit.size = gen::utils::toSize(dstInfo->widthBytes);
    return lit;
  }

  gen::Expr value = this->GenExpr(cast->expr, OutputFile);

  // An adr operand keeps the dereference meaning of `as` (design
  // section 2.2): read through the pointer at the target's width.
  if (value.type == "adr") {
    if (cast->wrap)
      alert("`as wrap` applies to integer values, not to an adr dereference",
            true, __FILE__, __LINE__);
    asmc::Size dstSize = asmc::QWord;
    if (auto prim = parse::PRIMITIVE_TYPES.find(dstName);
        prim != parse::PRIMITIVE_TYPES.end())
      dstSize = gen::utils::toSize(prim->second);
    stageToRax(*this, OutputFile, value);
    auto *deref = new asmc::Mov();
    deref->logicalLine = logicalLine();
    deref->size = dstSize;
    deref->from = "(" + registers()["%rax"]->get(asmc::QWord) + ")";
    deref->to = registers()["%rax"]->get(dstSize);
    OutputFile.text << deref;
    gen::Expr out = value;
    out.type = dstName;
    out.size = dstSize;
    out.access = deref->to;
    out.owned = false;
    out.needsDrop = false;
    return out;
  }

  const gen::utils::IntTypeInfo *srcInfo =
      gen::utils::classifiableIntType(value.type);
  const std::string srcName = value.type == "number" ? "i32" : value.type;
  if (value.type == "number" && srcInfo == nullptr)
    srcInfo = gen::utils::intTypeInfo("i32"); // literal provenance
  if (srcInfo == nullptr || dstInfo == nullptr)
    alert("`as` conversion from " + value.type + " to " + dstName +
              " is not an integer conversion",
          true, __FILE__, __LINE__);

  const gen::utils::ConversionKind kind =
      gen::utils::classifyConversion(*srcInfo, *dstInfo);
  const asmc::Size dstSize = gen::utils::toSize(dstInfo->widthBytes);

  if (kind == gen::utils::ConversionKind::Identity && !cast->wrap) {
    value.type = dstName;
    return value;
  }

  if (cast->wrap || kind == gen::utils::ConversionKind::Lossless) {
    stageToRax(*this, OutputFile, value);
    if (srcInfo->widthBytes < dstInfo->widthBytes)
      emitIntegerExtension(OutputFile, "%rax", *srcInfo, *dstInfo);
    gen::Expr out = value;
    out.type = dstName;
    out.size = dstSize;
    out.access = registers()["%rax"]->get(dstSize);
    return out;
  }

  // CheckedRequired: save the operand for the error message, extend
  // it to 64 bits in %rax, test the bits the destination cannot
  // represent, and construct Ok or Err. All bound tests are signed
  // jumps on the extended value: an unsigned destination's bound
  // fits the same comparison, a u64 destination needs no upper test,
  // and a u64 -> i64 conversion is exactly the sign test.
  // The canonical internal spelling of the instantiated union type
  // (the source spelling result::<T> is not a type-list name).
  const std::string resultTypeName = "result<" + dstName + ">";
  const std::string srcTempName =
      "$" + std::to_string(tempCount()++) + "_cast_src";
  const std::string dstTempName =
      "$" + std::to_string(tempCount()++) + "_cast_dst";
  ast::Type srcTempType(srcName, value.size);
  srcTempType.opType = value.op;
  const int srcSlot = gen::scope::ScopeManager::getInstance()->assign(
      srcTempName, srcTempType, false, false);
  const int dstSlot = gen::scope::ScopeManager::getInstance()->assign(
      dstTempName, ast::Type(dstName, dstSize), false, false);

  stageToRax(*this, OutputFile, value);
  auto *saveSrc = new asmc::Mov();
  saveSrc->logicalLine = logicalLine();
  saveSrc->size = value.size;
  saveSrc->op = value.op;
  saveSrc->from = registers()["%rax"]->get(value.size);
  saveSrc->to = "-" + std::to_string(srcSlot) + "(%rbp)";
  OutputFile.text << saveSrc;
  const gen::utils::IntTypeInfo *wideInfo =
      gen::utils::intTypeInfo(srcInfo->isSigned ? "i64" : "u64");
  if (srcInfo->widthBytes < 8)
    emitIntegerExtension(OutputFile, "%rax", *srcInfo, *wideInfo);

  const std::string errLabel = ".cast_err_" + std::to_string(labelCount()++);
  const std::string endLabel = ".cast_end_" + std::to_string(labelCount()++);
  auto emitCmpImm = [&](std::int64_t bound) {
    auto *cmp = new asmc::Cmp();
    cmp->logicalLine = logicalLine();
    cmp->size = asmc::QWord;
    cmp->from = "$" + std::to_string(bound);
    cmp->to = registers()["%rax"]->get(asmc::QWord);
    OutputFile.text << cmp;
  };
  auto emitCmpBound = [&](std::uint64_t bound) {
    if (bound <= 0x7fffffffULL) {
      emitCmpImm(static_cast<std::int64_t>(bound));
      return;
    }
    auto *load = new asmc::Mov();
    load->logicalLine = logicalLine();
    load->size = asmc::QWord;
    load->from = "$" + std::to_string(bound);
    load->to = registers()["%rcx"]->get(asmc::QWord);
    OutputFile.text << load;
    auto *cmp = new asmc::Cmp();
    cmp->logicalLine = logicalLine();
    cmp->size = asmc::QWord;
    cmp->from = registers()["%rcx"]->get(asmc::QWord);
    cmp->to = registers()["%rax"]->get(asmc::QWord);
    OutputFile.text << cmp;
  };
  auto jumpIfLess = [&]() {
    auto *jmp = new asmc::Jl();
    jmp->logicalLine = logicalLine();
    jmp->to = errLabel;
    OutputFile.text << jmp;
  };
  auto jumpIfGreater = [&]() {
    auto *jmp = new asmc::Jg();
    jmp->logicalLine = logicalLine();
    jmp->to = errLabel;
    OutputFile.text << jmp;
  };

  if (srcInfo->isSigned && !dstInfo->isSigned) {
    emitCmpImm(0);
    jumpIfLess();
  }
  const bool upperIsSignTest = dstInfo->isSigned && !srcInfo->isSigned &&
                               srcInfo->widthBytes == 8 &&
                               dstInfo->widthBytes == 8;
  if (upperIsSignTest) {
    emitCmpImm(0);
    jumpIfLess();
  } else if (!(dstInfo->widthBytes == 8 && !dstInfo->isSigned)) {
    emitCmpBound(gen::utils::intTypeMax(*dstInfo));
    jumpIfGreater();
  }
  if (dstInfo->isSigned && srcInfo->isSigned &&
      dstInfo->widthBytes < srcInfo->widthBytes) {
    emitCmpImm(gen::utils::intTypeMin(*dstInfo));
    jumpIfLess();
  }

  // In range: the destination-width bits of the extended value are
  // the converted value.
  auto *saveDst = new asmc::Mov();
  saveDst->logicalLine = logicalLine();
  saveDst->size = dstSize;
  saveDst->from = registers()["%rax"]->get(dstSize);
  saveDst->to = "-" + std::to_string(dstSlot) + "(%rbp)";
  OutputFile.text << saveDst;

  auto *okVar = new ast::Var();
  okVar->Ident = dstTempName;
  okVar->logicalLine = logicalLine();
  auto *okCtor = new ast::UnionConstructor(
      ast::Type(resultTypeName, asmc::QWord), "Ok", okVar, true, {});
  okCtor->logicalLine = logicalLine();
  gen::Expr result = this->GenExpr(okCtor, OutputFile);
  auto *jumpEnd = new asmc::Jmp();
  jumpEnd->logicalLine = logicalLine();
  jumpEnd->to = endLabel;
  OutputFile.text << jumpEnd;

  auto *errMark = new asmc::Label();
  errMark->logicalLine = logicalLine();
  errMark->label = errLabel;
  OutputFile.text << errMark;

  auto *srcVar = new ast::Var();
  srcVar->Ident = srcTempName;
  srcVar->logicalLine = logicalLine();
  auto *message = new ast::FStringLiteral();
  message->logicalLine = logicalLine();
  message->original = "checked conversion failed";
  message->val = "checked conversion failed: value %%% of type " + srcName +
                 " is out of range for type " + dstName + " (range " +
                 gen::utils::intTypeRange(*dstInfo) + ")";
  message->args.push_back(srcVar);
  gen::Expr messageValue =
      this->GenExpr(message, OutputFile, asmc::AUTO, "adr");
  const std::string msgTempName =
      "$" + std::to_string(tempCount()++) + "_cast_msg";
  const int msgSlot = gen::scope::ScopeManager::getInstance()->assign(
      msgTempName, ast::Type("adr", asmc::QWord), false, false);
  auto *saveMsg = new asmc::Mov();
  saveMsg->logicalLine = logicalLine();
  saveMsg->size = asmc::QWord;
  saveMsg->from = messageValue.access;
  saveMsg->to = "-" + std::to_string(msgSlot) + "(%rbp)";
  OutputFile.text << saveMsg;

  auto *msgVar = new ast::Var();
  msgVar->Ident = msgTempName;
  msgVar->logicalLine = logicalLine();
  auto *errorNew = new ast::NewExpr();
  errorNew->logicalLine = logicalLine();
  errorNew->type = ast::Type("Error", asmc::QWord);
  errorNew->args.push(msgVar);
  auto *errCtor = new ast::UnionConstructor(
      ast::Type(resultTypeName, asmc::QWord), "Err", errorNew, true, {});
  errCtor->logicalLine = logicalLine();
  this->GenExpr(errCtor, OutputFile);

  auto *endMark = new asmc::Label();
  endMark->logicalLine = logicalLine();
  endMark->label = endLabel;
  OutputFile.text << endMark;

  result.type = resultTypeName;
  result.size = asmc::QWord;
  result.access = registers()["%rax"]->get(asmc::QWord);
  return result;
}
