#include "ASM.hpp"

std::string ASMC::Instruction::toString(){
    return("");
}

std::string ASMC::Lable::toString(){
    return this->lable + ":\n";
}

std::string ASMC::LinkTask::toString(){
    return  this->command + "\t" + this->operand + "\n";
}

std::string ASMC::Call::toString(){
    return "\tcall\t" + this->function + "\n";
}

std::string ASMC::Mov::toString(){
    return "\tmov\t" + this->from + ", " + this->to + "\n";
}

std::string ASMC::Push::toString(){
    return "\tpush\t" + this->op + "\n";
}

std::string ASMC::Pop::toString(){
    return "\tpop\t" + this->op + "\n";
}

void ASMC::File::operator<<(ASMC::File file){
    //this->data.stitch(file.data);
    this->text.istitch(file.text);
}