#include "ASM.hpp"

std::string ASMC::Instruction::toString(){
    return("\tnoop");
}

std::string ASMC::Lable::toString(){
    return this->lable + ":";
}

std::string ASMC::LinkTask::toString(){
    return  this->command + "\t" + this->operand;
}

std::string ASMC::Call::toString(){
    return "\tcall\t" + this->function;
}

std::string ASMC::Mov::toString(){
    return "\tmov\t" + this->from + ", " + this->to;
}

std::string ASMC::Push::toString(){
    return "\tpush\t" + this->op;
}

std::string ASMC::Pop::toString(){
    return "\tpop\t" + this->op;
}

void ASMC::File::operator<<(ASMC::File file){
    //this->data.stitch(file.data);
    this->text.stitch(file.text);
}