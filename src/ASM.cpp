#include "ASM.hpp"

std::string ASMC::Instruction::toString(){
    return("noop");
}

std::string ASMC::ExIn::toString(){
    std::string output = this->mnumonic + "\t" + this->operand1;
    if (this->operand2 != ""){
        output += ", " + this->operand2;
    }
    return output;
}

std::string ASMC::Lable::toString(){
    return this->lable + ":"
}

std::string ASMC::SectionHeader::toString(){
    return this-> "." + this->section;
}g