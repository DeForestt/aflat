#include "CodeGenerator.hpp"
#include "Exceptions.hpp"

bool searchSymbol(gen::Symbol sym, std::string str){
    if (sym.symbol == str) return true; else return false;
}

std::string gen::CodeGenerator::GenExpr(AST::Expr * expr, ASMC::File &OutputFile){
    std::string output = "";
    if(dynamic_cast<AST::IntLiteral *>(expr) != nullptr){
        AST::IntLiteral * intlit = dynamic_cast<AST::IntLiteral *>(expr);
        output = '$' + std::to_string(intlit->val);
    } else if (dynamic_cast<AST::Var *>(expr) != nullptr)
    {
        AST::Var var = *dynamic_cast<AST::Var *>(expr);
        output = '-' + std::to_string(this->SymbolTable.search<std::string>(searchSymbol, var.Ident)->byteMod) + "(%rbp)";
    } else if (dynamic_cast<AST::Compound *>(expr) != nullptr)
    {
        AST::Compound comp = *dynamic_cast<AST::Compound *>(expr);
        switch (comp.op)
        {
            case AST::Plus:{
                ASMC::Mov * mov1 = new ASMC::Mov();
                ASMC::Mov * mov2 = new ASMC::Mov();
                ASMC::Add * add = new ASMC::Add();
                mov1->size = ASMC::DWord;
                mov2->size = ASMC::QWord;
                mov1->to = "%edx";
                mov2->to = "%eax";
                mov1->from = this->GenExpr(comp.expr1, OutputFile);
                mov2->from = this->GenExpr(comp.expr2, OutputFile);
                add->op2 = "%eax";
                add->op1 = "%edx";
                OutputFile.text << mov1;
                OutputFile.text << mov2;
                OutputFile.text << add;
                output = "%eax";
                break;
            }
            default:{
                throw err::Exception("Unhandled oporator");
                break;

            }
        }   
    }
    
    
    else{
        throw err::Exception("cannot gen expr");
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
        mov->size = ASMC::QWord;
        mov->from =  "%rsp";
        mov->to = "%rbp";
        OutputFile.text.push(lable);
        OutputFile.text.push(push);
        OutputFile.text.push(mov);
        
        this->intArgsCounter = 0;
        
        this->intArgsCounter = 0;
        OutputFile << this->GenSTMT(func->args);

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
            case AST::Char:
                offset = 1;
                break;
        }

        if(this->SymbolTable.search<std::string>(searchSymbol, dec->Ident) != nullptr) throw err::Exception("redefined veriable:" + dec->Ident);

        gen::Symbol Symbol;
        if (this->SymbolTable.head == nullptr){
            Symbol.byteMod = offset;
        }else{
            Symbol.byteMod = this->SymbolTable.head->data.byteMod + offset;
        }
        Symbol.type = dec->type;
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
            case AST::Char:
                offset = 1;
                break;
        }

        if(this->SymbolTable.search<std::string>(searchSymbol, dec->Ident) != nullptr) throw err::Exception("redefined veriable:" + dec->Ident);

        gen::Symbol symbol;
        if (this->SymbolTable.head == nullptr){
            symbol.byteMod = offset;
        }else{
            symbol.byteMod = this->SymbolTable.head->data.byteMod + offset;
        }
        symbol.type = dec->type;
        symbol.symbol = dec->Ident;
        this->SymbolTable.push(symbol);

        if(dynamic_cast<AST::IntLiteral *>(decAssign->expr)){
            ASMC::Mov * mov = new ASMC::Mov();
            mov->size = ASMC::DWord;
            mov->from = this->GenExpr(decAssign->expr, OutputFile);
            mov->to = "-" + std::to_string(symbol.byteMod) + "(%rbp)";
            OutputFile.text.push(mov);
        }else{
            ASMC::Mov * mov = new ASMC::Mov();
            mov->from = this->GenExpr(decAssign->expr, OutputFile);
            mov->to = "-" + std::to_string(symbol.byteMod) + "(%rbp)";
            OutputFile.text.push(mov);
        }

    }else if (dynamic_cast<AST::Return *>(STMT) != nullptr)
    {
        /*
            mov [this.GenExpr(ret.value)]
            pop rbp
            ret
        */

        AST::Return * ret = dynamic_cast<AST::Return *>(STMT);

        if (dynamic_cast<AST::IntLiteral *>(ret->expr))
        {
            ASMC::Mov * mov = new ASMC::Mov();
            mov->size = ASMC::DWord;
            mov->from = this->GenExpr(ret->expr, OutputFile);
            mov->to = "%eax";
            OutputFile.text.push(mov);   
        }else{
            ASMC::Mov * mov = new ASMC::Mov();
            mov->from = this->GenExpr(ret->expr, OutputFile);
            mov->to = "%eax";
            OutputFile.text.push(mov);
        }
        

        
        ASMC::Pop * pop = new ASMC::Pop();
        pop->op = "%rbp";
        ASMC::Return * re = new ASMC::Return();
        
        OutputFile.text.push(pop);
        OutputFile.text.push(re);
    }else if (dynamic_cast<AST::Assign *>(STMT) != nullptr)
    {
        AST::Assign * assign = dynamic_cast<AST::Assign *>(STMT);
        Symbol * symbol = this->SymbolTable.search<std::string>(searchSymbol, assign->Ident);
        if(symbol == nullptr) throw err::Exception("unknown name: " + assign->Ident);
        ASMC::Movl * mov = new ASMC::Movl();
        mov->from = this->GenExpr(assign->expr, OutputFile);
        mov->to = "-" + std::to_string(symbol->byteMod) + "(%rbp)";
        OutputFile.text.push(mov);
    }else if (dynamic_cast<AST::Argument *>(STMT) != nullptr)
    {
        /*
            movl $0x0, -[SymbolT + size](rdp)
            **also needs to be added to symbol table**
        */

        if (intArgsCounter > 6) throw err::Exception("AFlat compiler cannot handle more than 6 int arguments.");
        AST::Argument * arg =  dynamic_cast<AST::Argument *>(STMT);
        int offset = 0;
        switch(arg->type){
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

        if(this->SymbolTable.search<std::string>(searchSymbol, arg->Ident) != nullptr) throw err::Exception("redefined veriable:" + arg->Ident);

        gen::Symbol symbol;
        if (this->SymbolTable.head == nullptr){
            symbol.byteMod = offset;
        }else{
            symbol.byteMod = this->SymbolTable.peek().byteMod + offset;
        }
        symbol.symbol = arg->Ident;

        this->SymbolTable.push(symbol);

        ASMC::Mov * mov = new ASMC::Mov();
        switch (symbol.type)
        {
        case AST::Int:
            mov->size = ASMC::DWord;
            break;
        default:
            break;
        }
        mov->from = this->intArgs[intArgsCounter];
        mov->to = "-" + std::to_string(symbol.byteMod) + + "(%rbp)";
        OutputFile.text.push(mov);
        intArgsCounter++;
    }else if (dynamic_cast<AST::Call *>(STMT) != nullptr)
    {
        AST::Call * call = dynamic_cast<AST::Call *>(STMT);

        this->intArgsCounter = 0;
        while (call->Args.count > 0)
        {
            
            ASMC::Mov * mov = new ASMC::Mov();
            mov->size = ASMC::DWord;
            mov->from = this->GenExpr(call->Args.pop(), OutputFile);
            mov->to = this->intArgs[intArgsCounter];
            intArgsCounter++;
            OutputFile.text.push(mov);
        }

        ASMC::Call * calls = new ASMC::Call;
        calls->function = call->ident;
        OutputFile.text.push(calls);
    }
    
    else{
        OutputFile.text.push(new ASMC::Instruction());
    }

    return OutputFile;
}