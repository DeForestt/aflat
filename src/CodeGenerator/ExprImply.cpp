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
ast::Expr *gen::CodeGenerator::imply(ast::Expr *expr, std::string typeName) {
  // find the type
  // find the type of the expression
  auto exprResult = this->GenExpr(expr, *(new asmc::File()), asmc::AUTO, "");
  std::string exprType = exprResult.type;

  auto type = this->typeList[typeName];
  if (type != nullptr) {
    auto cl = dynamic_cast<gen::Class *>(*type);
    if (cl != nullptr) {
      // check if the class has a function called "__from__<exprType>"
      std::string fromFuncName = "__from__" + exprType;
      ast::Function *fromFunc = cl->nameTable[fromFuncName];
      if (cl->dynamic || cl->uniqueType) {
        ast::NewExpr *newExpr = new ast::NewExpr();
        if (fromFunc)
          newExpr->initFuncName = fromFuncName;
        newExpr->logicalLine = this->logicalLine;
        newExpr->type.typeName = typeName;
        newExpr->type.safeType = cl->safeType;
        newExpr->type.opType = asmc::Hard;
        newExpr->type.size = asmc::QWord;
        newExpr->args.push(expr);
        return newExpr;
      } else {
        ast::Call *init = new ast::Call();
        init->logicalLine = this->logicalLine;
        init->ident = typeName;
        init->Args.push(expr);
        ast::CallExpr *call = new ast::CallExpr;
        call->logicalLine = this->logicalLine;
        call->call = init;
        return call;
      }
    }
  }
  this->alert("Cannot imply type " + typeName +
                  " from expression of type at line " +
                  std::to_string(expr->logicalLine),
              true, __FILE__, __LINE__);
  return nullptr;
}
} // namespace gen
