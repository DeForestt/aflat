#include "ASM.hpp"

std::string asmc::Instruction::toString(){
    return("");
}

std::string asmc::SysCall::toString(){
    return "\tsyscall\t\n";
}

std::string asmc::Lable::toString(){
    return this->lable + ":\n";
}

std::string asmc::LinkTask::toString(){
    return  "." + this->command + "\t" + this->operand + "\n";
}

std::string asmc::Call::toString(){
    return "\tcall\t" + this->function + "\n";
}

std::string asmc::Mov::toString(){
    std::string size = "";
    switch (this->size)
    {
    case asmc::Byte:
        size = "b";
        break;
    case asmc::Word:
        size = "s";
        break;
    case asmc::DWord:
        size = "l";
        break;
    case asmc::QWord:
        size = "q";
        break;
    case asmc::AUTO:
        size = "";
        break;
    default:
        size = "";
        break;
    }
    if(this->op == asmc::Float) return "\tmovss" "\t" + this->from + ", " + this->to + "\n";
    return "\tmov" + size + "\t" + this->from + ", " + this->to + "\n";
}

std::string asmc::Cmp::toString(){
    std::string size = "";
    switch (this->size)
    {
    case asmc::Byte:
        size = "b";
        break;
    case asmc::Word:
        size = "s";
        break;
    case asmc::DWord:
        size = "l";
        break;
    case asmc::QWord:
        size = "q";
        break;
    case asmc::AUTO:
        size = "";
        break;
    default:
        size = "";
        break;
    }
    return "\tcmp" + size + "\t" + this->from + ", " + this->to + "\n";
}

std::string asmc::Lea::toString(){
    return "\tlea\t" + this->from + ", " + this->to + "\n"; 
}

std::string asmc::Movq::toString(){
    return "\tmovq\t" + this->from + ", " + this->to + "\n";
}

std::string asmc::Jmp::toString(){
    return "\tjmp\t" + this->to + "\n";
}

std::string asmc::Jne::toString(){
    return "\tjne\t" + this->to + "\n";
}

std::string asmc::Je::toString(){
    return "\tje\t" + this->to + "\n";
}

std::string asmc::Jl::toString(){
    return "\tjl\t" + this->to + "\n";
}

std::string asmc::Jle::toString(){
    return "\tjle\t" + this->to + "\n";
}

std::string asmc::Jg::toString(){
    return "\tjg\t" + this->to + "\n";
}

std::string asmc::Jge::toString(){
    return "\tjge\t" + this->to + "\n";
}

std::string asmc::Movl::toString(){
    return "\tmovl\t" + this->from + ", " + this->to + "\n";
}

std::string asmc::StringLiteral::toString(){
    return "\t.asciz\t \"" + this->value + "\"\n";
}

std::string asmc::FloatLiteral::toString(){
    return "\t.float\t" + this->value + "\n";
}

std::string asmc::Add::toString(){
    if (this->opType == asmc::Float) return "\taddss\t" + this->op1 + ", " + this->op2 + "\n";
    return "\tadd\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string asmc::Sub::toString(){
    if (this->opType == asmc::Float) return "\tsubss\t" + this->op1 + ", " + this->op2 + "\n";
    return "\tsub\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string asmc::Mul::toString(){
    if (this->opType == asmc::Float) return "\tmulss\t" + this->op1 + ", " + this->op2 + "\n";
    return "\timul\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string asmc::Xor::toString(){
    return "\txor\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string asmc::And::toString(){
    return "\tandl\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string asmc::Or::toString(){
    return "\torl\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string asmc::Sal::toString(){
    return "\tsall\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string asmc::Sar::toString(){
    return "\tsarl\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string asmc::Div::toString(){
    if (this->opType == asmc::Float) return "\tdivss\t" + this->op1 + ", " + this->op2 + "\n";
    return "\tcltd\n\tidivl\t" + this->op1 + "\n";
}

std::string asmc::Subq::toString(){
    return "\tsubq\t" + this->op1 + ", " + this->op2 + "\n";
}

std::string asmc::Push::toString(){
    return "\tpushq\t" + this->op + "\n";
}

std::string asmc::Pop::toString(){
    return "\tpopq\t" + this->op + "\n";
}

std::string asmc::Return::toString(){
    return "\tleave\n\tret\n";
}

void asmc::File::operator<<(asmc::File file){
    this->linker.stitch(file.linker);
    this->text.istitch(file.text);
    this->bss.stitch(file.bss);
    this->data.stitch(file.data);
    if (!this->hasLambda && file.hasLambda) {
        this->hasLambda = true;
        this->lambdas = new asmc::File;
    }
    if (file.hasLambda) {
        this->hasLambda = true;
        *this->lambdas << *file.lambdas;
    }
}

void asmc::File::cstitch(asmc::File file){
    this->linker.stitch(file.linker);
    this->text.istitch(file.text);
    this->bss.stitch(file.bss);
    this->data.stitch(file.data);
}

void asmc::File::operator>>(asmc::File file){
    this->linker.stitch(file.linker);
    this->text.stitch(file.text);
    this->bss.stitch(file.bss);
    this->data.stitch(file.data);
    if (!this->hasLambda && file.hasLambda) {
        this->hasLambda = true;
        this->lambdas = new asmc::File();
    }
    if (file.hasLambda && file.lambdas != nullptr) {
        this->hasLambda = true;
        this->lambdas->operator<<(*file.lambdas);
    }
}

void asmc::File::collect(){
    if(this->hasLambda && this->lambdas != nullptr){
        this->lambdas->collect();
        this->cstitch(*this->lambdas);
        delete this->lambdas;
    }
}

asmc::File::File(){
    this->hasLambda = false;
    this->lambdas = nullptr;
    this->linker = links::LinkedList<asmc::Instruction *>();
    this->text = links::LinkedList<asmc::Instruction *>();
    this->bss = links::LinkedList<asmc::Instruction *>();
};

asmc::Register::Register(std::string _qWord, std::string _dWord, std::string _word, std::string _byte){
    this->qWord = '%' + _qWord;
    this->dWord = '%' + _dWord;
    this->word = '%' + _word;
    this->byte = '%' + _byte;
}

std::string asmc::Register::get(asmc::Size wants){
    switch(wants){
        case asmc::QWord:
            return this->qWord;
            break;
        case asmc::DWord:
            return this->dWord;
            break;
        case asmc::Word:
            return this->word;
            break;
        case asmc::Byte:
            return this->byte;
            break;
        default:
            throw err::Exception("Unknown register size");
    };
} 