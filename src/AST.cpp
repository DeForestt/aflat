#include <AST.hpp>
#include <string>

std::string AST::Sequence::toString(){
    std::string output = this->Statment1->toString() + this->Statment2->toString();
    return output;
}

std::string AST::Declare::toString(){
    return "Declare" + this->Ident; 
};