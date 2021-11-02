#include <AST.hpp>
#include <string>


bool AST::Type::compair(AST::Type t, std::string name){
    if(t.typeName == name) return true;
    return false;
}  