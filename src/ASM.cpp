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
    if(this->op == ASMC::Float) return "\tmovss" "\t" + this->from + ", " + this->to + "\n";
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

std::string ASMC::Jle::toString(){
    return "\tjle\t" + this->to + "\n";
}

std::string ASMC::Jg::toString(){
    return "\tjg\t" + this->to + "\n";
}

std::string ASMC::Jge::toString(){
    return "\tjge\t" + this->to + "\n";
}

std::string ASMC::Movl::toString(){
    return "\tmovl\t" + this->from + ", " + this->to + "\n";
}

std::string ASMC::StringLiteral::toString(){
    return "\t.asciz\t \"" + this->value + "\"\n";
}

std::string ASMC::FloatLiteral::toString(){
    return "\t.float\t" + this->value + "\n";
}

std::string ASMC::Add::toString(){
    if (this->opType == ASMC::Float) return "\taddss\t" + this->op1 + ", " + this->op2 + "\n";
    return "\tadd\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string ASMC::Sub::toString(){
    if (this->opType == ASMC::Float) return "\tsubss\t" + this->op1 + ", " + this->op2 + "\n";
    return "\tsub\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string ASMC::Mul::toString(){
    if (this->opType == ASMC::Float) return "\tmulss\t" + this->op1 + ", " + this->op2 + "\n";
    return "\timul\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string ASMC::Div::toString(){
    if (this->opType == ASMC::Float) return "\tdivss\t" + this->op1 + ", " + this->op2 + "\n";
    return "\tcltd\n\tidivl\t" + this->op1 + "\n";
}

std::string ASMC::Subq::toString(){
    return "\tsubq\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string ASMC::Push::toString(){
    return "\tpushq\t" + this->op + "\n";
}

std::string ASMC::Pop::toString(){
    return "\tpop\t" + this->op + "\n";
}

std::string ASMC::Return::toString(){
    return "\tleave\n\tret\n";
}

void ASMC::File::operator<<(ASMC::File file){
    this->linker.stitch(file.linker);
    this->text.istitch(file.text);
    this->bss.stitch(file.bss);
    this->data.stitch(file.data);
}

ASMC::Register::Register(std::string _qWord, std::string _dWord, std::string _word, std::string _byte){
    this->qWord = '%' + _qWord;
    this->dWord = '%' + _dWord;
    this->word = '%' + _word;
    this->byte = '%' + _byte;
}

std::string ASMC::Register::get(ASMC::Size wants){
    switch(wants){
        case ASMC::QWord:
            return this->qWord;
            break;
        case ASMC::DWord:
            return this->dWord;
            break;
        case ASMC::Word:
            return this->word;
            break;
        case ASMC::Byte:
            return this->byte;
            break;
        default:
            throw err::Exception("Unknown register size");
    };
}