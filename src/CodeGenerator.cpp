#include "CodeGenerator.hpp"

ASMC::File gen::CodeGenerator::GenSTMT(AST::Statment * STMT){

    ASMC::File OutputFile;

    if(dynamic_cast<AST::Sequence *>(STMT) != nullptr){
        AST::Sequence * sequence = dynamic_cast<AST::Sequence *>(STMT);
        OutputFile << this->GenSTMT(sequence->Statment1);
        OutputFile << this->GenSTMT(sequence->Statment2);

    }else if(dynamic_cast<AST::Function *>(STMT)){
        /*
            ident:
                push rbp
                mov  rsp, rbp
                this->GenSTMT()
                
        */
        AST::Function * func = dynamic_cast<AST::Function *>(STMT);
        ASMC::Lable lable;
        lable.lable == func->ident.ident;
        ASMC::Push push;
        push.op = "%rbp";
        ASMC::Mov mov;
        mov.from =  "%rsp";
        mov.to = "%rbp";
        OutputFile.text.push(lable);
        OutputFile.text.push(push);
        OutputFile.text.push(mov);
        OutputFile << this->GenSTMT(func->statment);
        delete(func);
    }else if (dynamic_cast<AST::Declare *>(STMT) != nullptr)
    {
        /*
            movl $0x0, -[SymbolT + size](rdp)
            **also needs to be added to symbol table**
        */
        AST::Declare * dec =  dynamic_cast<AST::Declare *>(STMT);
        int offset = 0;
        switch(dec->type){
            case AST::Int:
                offset = 4;
                break;
            case AST::Byte:
                offset = 1;
                break;
            case AST::String:
                offset = 4;
                break;
        }

        gen::Symbol Symbol;
        if (this->SymbolTable.head == nullptr){
            Symbol.byteMod = offset;
        }else{
            Symbol.byteMod = this->SymbolTable.head->data.byteMod + offset;
        }
        Symbol.symbol = dec->Ident;
        this->SymbolTable.push(Symbol);

        ASMC::Mov mov;
        mov.from = "$0x0";
        mov.to = "-0x" + std::to_string(Symbol.byteMod) + "(rdp)";
    }
    

    return OutputFile;
}