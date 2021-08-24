#include <AST.hpp>
#include <string>

std::string AST::Statment::toString(){
    return "unimpimented statment";
}

std::string AST::Sequence::toString(){

    std::string output = "";

    if (this->Statment1 != nullptr)
    {
        try
        {
            output += this->Statment1->toString();
        }
        catch(const std::exception& e)
        { }
    }
    output += "\n";
    if (this->Statment2 != nullptr){
        try
        {
            output += this->Statment2->toString();
        }
        catch(const std::exception& e)
        { 
            output += "caught it";
        }
    }
    return output;
}

std::string AST::Declare::toString(){
    std::string type = "Unknown type";
    switch (this->type){
        case AST::Byte :
            type = "Byte";
            break;
        case AST::String :
            type = "String";
            break;
        case AST::Int :
            type = "Int";
            break;
    }
    return "[Declare " + type + ": " + this->Ident + "]"; 
};

std::string AST::Function::toString(){
    std::string type = "Unknown type";
    switch (this->type){
        case AST::Byte :
            type = "Byte";
            break;
        case AST::String :
            type = "String";
            break;
        case AST::Int :
            type = "Int";
            break;
    }
    return "[function " + type + ": " + this->ident.ident + " { \n " + this->statment->toString() +  " \n}]";
}

std::string AST::CWrite::toString(){
    return "CWrite";
}