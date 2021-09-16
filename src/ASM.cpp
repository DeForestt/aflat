#include "ASM.hpp"

std::string ASMC::Instruction::toString(){
    return("");
}

std::string ASMC::SysCall::toString(){
    return "\tsyscall\t\n";
}

std::string ASMC::Lable::toString(){
    return this->lable + ":\n";
}

std::string ASMC::LinkTask::toString(){
    return  "." + this->command + "\t" + this->operand + "\n";
}

std::string ASMC::Call::toString(){
    return "\tcall\t" + this->function + "\n";
}

std::string ASMC::Mov::toString(){
    std::string size = "";
    switch (this->size)
    {
    case ASMC::Byte:
        size = "b";
        break;
    case ASMC::Word:
        size = "s";
        break;
    case ASMC::DWord:
        size = "l";
        break;
    case ASMC::QWord:
        size = "q";
        break;
    case ASMC::AUTO:
        size = "";
        break;
    default:
        size = "";
        break;
    }
    return "\tmov" + size + "\t" + this->from + ", " + this->to + "\n";
}

std::string ASMC::Cmp::toString(){
    std::string size = "";
    switch (this->size)
    {
    case ASMC::Byte:
        size = "b";
        break;
    case ASMC::Word:
        size = "s";
        break;
    case ASMC::DWord:
        size = "l";
        break;
    case ASMC::QWord:
        size = "q";
        break;
    case ASMC::AUTO:
        size = "";
        break;
    default:
        size = "";
        break;
    }
    return "\tcmp" + size + "\t" + this->from + ", " + this->to + "\n";
}

std::string ASMC::Lea::toString(){
    return "\tlea\t" + this->from + ", " + this->to + "\n"; 
}

std::string ASMC::Movq::toString(){
    return "\tmovq\t" + this->from + ", " + this->to + "\n";
}

std::string ASMC::Jmp::toString(){
    return "\tjmp\t" + this->to + "\n";
}

std::string ASMC::Jne::toString(){
    return "\tjne\t" + this->to + "\n";
}

std::string ASMC::Je::toString(){
    return "\tje\t" + this->to + "\n";
}

std::string ASMC::Jl::toString(){
    return "\tjl\t" + this->to + "\n";
}

std::string ASMC::Jg::toString(){
    return "\tjg\t" + this->to + "\n";
}

std::string ASMC::Movl::toString(){
    return "\tmovl\t" + this->from + ", " + this->to + "\n";
}

std::string ASMC::Add::toString(){
    return "\tadd\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string ASMC::Push::toString(){
    return "\tpush\t" + this->op + "\n";
}

std::string ASMC::Pop::toString(){
    return "\tpop\t" + this->op + "\n";
}

std::string ASMC::Return::toString(){
    return "\tret\n";
}

void ASMC::File::operator<<(ASMC::File file){
    this->linker.stitch(file.linker);
    this->text.istitch(file.text);
}

ASMC::Register::Register(std::string _qWord, std::string _dWord, std::string _word, std::string _byte){
    this->qWord = '%' + _qWord;
    this->dWord = '%' + _dWord;
    this->word = '%' + _word;
    this->byte = '%' + _byte;
}