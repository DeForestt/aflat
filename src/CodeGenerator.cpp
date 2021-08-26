#include "CodeGenerator.hpp"

std::string gen::CodeGenerator::GenExpr(AST::Expr * expr){
    std::string output = "";
    if(dynamic_cast<AST::IntLiteral *>(expr) != nullptr){
        AST::IntLiteral * intlit = dynamic_cast<AST::IntLiteral *>(expr);
        output = '$' + std::to_string(intlit->val);
    }else{
        throw ("cannot gen statment");
    }
    return output;
};

ASMC::File gen::CodeGenerator::GenSTMT(AST::Statment * STMT){

    ASMC::File OutputFile;

    if(dynamic_cast<AST::Sequence *>(STMT) != nullptr){
        AST::Sequence * sequence = dynamic_cast<AST::Sequence *>(STMT);
        OutputFile << this->GenSTMT(sequence->Statment1);
        OutputFile << this->GenSTMT(sequence->Statment2);;

    }else if(dynamic_cast<AST::Function *>(STMT)){
        /*
            ident:
                push rbp
                mov  rsp, rbp
                this->GenSTMT()
                
        */

       this->SymbolTable.clear();

        AST::Function * func = dynamic_cast<AST::Function *>(STMT);
        ASMC::Lable * lable = new ASMC::Lable;
        lable->lable = func->ident.ident;
        ASMC::Push * push = new ASMC::Push();
        push->op = "%rbp";
        ASMC::Mov * mov = new ASMC::Mov();
        mov->from =  "%rsp";
        mov->to = "%rbp";
        OutputFile.text.push(lable);
        OutputFile.text.push(push);
        OutputFile.text.push(mov);
        ASMC::LinkTask * link = new ASMC::LinkTask();
        link->command = "global";
        link->operand = func->ident.ident;
        OutputFile.linker.push(link);
        ASMC::File file = this->GenSTMT(func->statment);
        OutputFile << file;
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

    }else if (dynamic_cast<AST::DecAssign *>(STMT) != nullptr)
    {
        /*
            movl $0x0, -[SymbolT + size](rdp)
            **also needs to be added to symbol table**
        */
        AST::DecAssign * decAssign =  dynamic_cast<AST::DecAssign *>(STMT);
        AST::Declare * dec = decAssign->declare;
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

        ASMC::Movq * mov = new ASMC::Movq();
        mov->from = "$0x0";
        mov->to = "-0x" + std::to_string(Symbol.byteMod) + "(%rbp)";
        OutputFile.text.push(mov);

    }else if (dynamic_cast<AST::Return *>(STMT) != nullptr)
    {
        /*
            mov [this.GenExpr(ret.value)]
            pop rbp
            ret
        */

        AST::Return * ret = dynamic_cast<AST::Return *>(STMT);
        ASMC::Mov * mov = new ASMC::Mov();
        mov->from = this->GenExpr(ret->expr);
        mov->to = "%rax";
        
        ASMC::Pop * pop = new ASMC::Pop();
        pop->op = "%rbp";
        ASMC::Return * re = new ASMC::Return();
        OutputFile.text.push(mov);
        OutputFile.text.push(pop);
        OutputFile.text.push(re);
    }
     else{
        OutputFile.text.push(new ASMC::Instruction());
    }
    

    return OutputFile;
}