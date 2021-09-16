#include "CodeGenerator.hpp"
#include "Exceptions.hpp"

bool searchSymbol(gen::Symbol sym, std::string str){
    if (sym.symbol == str) return true; else return false;
}

gen::CodeGenerator::CodeGenerator(){
    this->registers << ASMC::Register("rax", "eax", "ax", "al");
    this->registers << ASMC::Register("rcx", "ecx", "cx", "cl");
    this->registers << ASMC::Register("rdx", "edx", "dx", "dl");
    this->registers << ASMC::Register("rbx", "ebx", "bx", "bl");
    this->registers << ASMC::Register("rsi", "esi", "si", "sil");
    this->registers << ASMC::Register("rdi", "edi", "di", "dil");   
    this->registers << ASMC::Register("rsp", "esp", "sp", "spl");
    this->registers << ASMC::Register("rbp", "ebp", "bp", "bpl");
    this->registers.foo = ASMC::Register::compair;
}

gen::Expr gen::CodeGenerator::GenExpr(AST::Expr * expr, ASMC::File &OutputFile){
    gen::Expr output;
    if(dynamic_cast<AST::IntLiteral *>(expr) != nullptr){
        AST::IntLiteral * intlit = dynamic_cast<AST::IntLiteral *>(expr);
        output.access = '$' + std::to_string(intlit->val);
        output.size = ASMC::DWord;
    } else if (dynamic_cast<AST::Var *>(expr) != nullptr){
        AST::Var var = *dynamic_cast<AST::Var *>(expr);
        gen::Symbol sym = *this->SymbolTable.search<std::string>(searchSymbol, var.Ident);
        if(sym.type == AST::Int) output.size = ASMC::DWord;
            else if (sym.type == AST::Char)  output.size = ASMC::Byte;
            else if (sym.type == AST::IntPtr) output.size = ASMC::QWord;
            else if (sym.type == AST::CharPtr) output.size = ASMC::QWord;

        output.access = '-' + std::to_string(sym.byteMod) + "(%rbp)";
    }else if (dynamic_cast<AST::CharLiteral *>(expr) != nullptr){
        AST::CharLiteral charlit = *dynamic_cast<AST::CharLiteral *>(expr);
        output.access = "$" + std::to_string(charlit.value);
        output.size = ASMC::Byte;
    }else if (dynamic_cast<AST::Refrence *>(expr) != nullptr)
    {
        AST::Refrence ref = *dynamic_cast<AST::Refrence *>(expr);
        gen::Symbol sym = *this->SymbolTable.search<std::string>(searchSymbol, ref.Ident);
        ASMC::Lea * lea = new ASMC::Lea();
        lea->to = this->registers["%rax"]->qWord;
        lea->from = '-' + std::to_string(sym.byteMod) + "(%rbp)";
        //ASMC::Mov * mov = new ASMC::Mov();
        OutputFile.text << lea;
        output.access = registers["%rax"]->qWord;
        output.size = ASMC::QWord;
    }else if(dynamic_cast<AST::DeRefence *>(expr)){
        AST::DeRefence deRef = *dynamic_cast<AST::DeRefence *>(expr);
        gen::Symbol sym = *this->SymbolTable.search<std::string>(searchSymbol, deRef.Ident);
        ASMC::Mov * mov = new ASMC::Mov();
        ASMC::Mov * mov2 = new ASMC::Mov();
        mov->size = ASMC::QWord;
        mov->from = '-' + std::to_string(sym.byteMod) + "(%rbp)";
        mov->to = this->registers["%rax"]->qWord;
        mov2->from = "(" + this->registers["%rax"]->qWord + ")";
        switch (deRef.type)
        {
        case AST::Int:
            mov2->size = ASMC::DWord;
            mov2->to = this->registers["%rax"]->dWord;
            output.size = ASMC::DWord;
            output.access = this->registers["%rax"]->dWord;
            break;
        case AST::Char:
            mov2->size = ASMC::Byte;
            mov2->to = this->registers["%rax"]->byte;
            output.size = ASMC::Byte;
            output.access = this->registers["%rax"]->byte;
            break;
        case AST::IntPtr:
            mov2->size = ASMC::QWord;
            mov2->to = this->registers["%rax"]->qWord;
            output.size = ASMC::QWord;
            output.access = this->registers["%rax"]->qWord;
            break;
        default:
            throw err::Exception("Cannot DeRefrence to this type");
            break;
        }

        OutputFile.text << mov;
        OutputFile.text << mov2;

        
    }else if (dynamic_cast<AST::Compound *>(expr) != nullptr)
    {
        AST::Compound comp = *dynamic_cast<AST::Compound *>(expr);
        switch (comp.op)
        {
            case AST::Plus:{
                ASMC::Mov * mov1 = new ASMC::Mov();
                ASMC::Mov * mov2 = new ASMC::Mov();
                ASMC::Add * add = new ASMC::Add();
                gen::Expr expr1 = this->GenExpr(comp.expr1, OutputFile);
                gen::Expr expr2 = this->GenExpr(comp.expr2, OutputFile);
                mov1->size = expr1.size;
                mov2->size = expr1.size;
                switch (expr1.size)
                {
                case ASMC::Byte:
                    mov1->to = this->registers["%edx"]->byte;
                    break;
                case ASMC::Word:
                    mov1->to = this->registers["%edx"]->word;
                    break;
                case ASMC::DWord:
                    mov1->to = this->registers["%edx"]->dWord;
                    break;
                case ASMC::QWord:
                    mov1->to = this->registers["%edx"]->qWord;
                    break;
                default:
                    mov1->to = this->registers["%edx"]->qWord;
                    break;
                }
                
                switch (expr1.size)
                {
                case ASMC::Byte:
                    mov2->to = this->registers["%eax"]->byte;
                    break;
                case ASMC::Word:
                    mov2->to = this->registers["%eax"]->word;
                    break;
                case ASMC::DWord:
                    mov2->to = this->registers["%eax"]->dWord;
                    break;
                case ASMC::QWord:
                    mov2->to = this->registers["%eax"]->qWord;
                    break;
                default:
                    mov2->to = this->registers["%eax"]->qWord;
                    break;
                }
                
                mov2->to = "%eax";
                mov1->from = expr1.access;
                mov2->from = expr2.access;
                add->op2 = "%eax";
                add->op1 = "%edx";
                OutputFile.text << mov1;
                OutputFile.text << mov2;
                OutputFile.text << add;
                output.access = "%eax";
                output.size = ASMC::DWord;
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

void gen::CodeGenerator::GenArgs(AST::Statment * STMT, ASMC::File &OutputFile){
     if(dynamic_cast<AST::Sequence *>(STMT) != nullptr){
        AST::Sequence * sequence = dynamic_cast<AST::Sequence *>(STMT);
        this->GenArgs(sequence->Statment1, OutputFile);
        this->GenArgs(sequence->Statment2, OutputFile);
    }else if (dynamic_cast<AST::Declare *>(STMT) != nullptr)
    {
        /*
            movl $0x0, -[SymbolT + size](rdp)
            **also needs to be added to symbol table**
        */

        if (intArgsCounter > 6) throw err::Exception("AFlat compiler cannot handle more than 6 int arguments.");
        AST::Declare * arg =  dynamic_cast<AST::Declare *>(STMT);
        int offset = 0;
        switch(arg->type){
            case AST::Int:
                offset = 4;
                break;
            case AST::IntPtr:
                offset = 8;
                break;
            case AST::CharPtr:
                offset = 8;
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
        mov->from = this->intArgs[intArgsCounter].dWord;
        mov->size = ASMC::DWord;
        mov->to = "-" + std::to_string(symbol.byteMod) + + "(%rbp)";
        OutputFile.text << mov;
        intArgsCounter++;
    }
}

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
        int AlignmentLoc = OutputFile.text.count;
        
        this->intArgsCounter = 0;
        

        ASMC::LinkTask * link = new ASMC::LinkTask();
        link->command = "global";
        link->operand = func->ident.ident;
        this->GenArgs(func->args, OutputFile);
        OutputFile.linker.push(link);
        ASMC::File file = this->GenSTMT(func->statment);
        OutputFile << file;
        
        int alligne = (this->SymbolTable.peek().byteMod / 16) * 16;
        ASMC::Sub * sub = new ASMC::Sub;
        sub->op1 = "$" + std::to_string(alligne);
        sub->op2 = this->registers["%rsp"]->qWord;
        OutputFile.text.insert(sub, AlignmentLoc);
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
            case AST::IntPtr:
                offset = 8;
                break;
            case AST::CharPtr:
                offset = 8;
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
            case AST::IntPtr:
                offset = 8;
                break;
            case AST::CharPtr:
                offset = 8;
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

        ASMC::Mov * mov = new ASMC::Mov();
        gen::Expr expr = this->GenExpr(decAssign->expr, OutputFile);
        mov->size = expr.size;
        mov->from = expr.access;
        mov->to = "-" + std::to_string(symbol.byteMod) + "(%rbp)";
        OutputFile.text << mov;

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
            mov->size = ASMC::AUTO;
            mov->from = this->GenExpr(ret->expr, OutputFile).access;
            mov->to = "%rax";
            OutputFile.text.push(mov);   
        }else{
            ASMC::Mov * mov = new ASMC::Mov();
            mov->size = ASMC::AUTO;
            mov->from = this->GenExpr(ret->expr, OutputFile).access;
            mov->to = "%rax";
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
        ASMC::Mov * mov = new ASMC::Mov();
        gen::Expr expr = this->GenExpr(assign->expr, OutputFile);
        mov->size = expr.size;
        mov->from = expr.access;
        mov->to = "-" + std::to_string(symbol->byteMod) + "(%rbp)";
        OutputFile.text << mov;
    }else if (dynamic_cast<AST::Call *>(STMT) != nullptr)
    {
        AST::Call * call = dynamic_cast<AST::Call *>(STMT);

        this->intArgsCounter = 0;
        call->Args.invert();
        while (call->Args.count > 0)
        {
            
            ASMC::Mov * mov = new ASMC::Mov();
            mov->size = ASMC::DWord;
            mov->from = this->GenExpr(call->Args.pop(), OutputFile).access;
            mov->to = "%eax";

            ASMC::Mov * mov2 = new ASMC::Mov();
            mov2->from = "%eax";
            mov2->size = ASMC::DWord;
            mov2->to = this->intArgs[intArgsCounter].dWord;
            intArgsCounter++;
            OutputFile.text << mov;
            OutputFile.text << mov2;
        }

        ASMC::Call * calls = new ASMC::Call;
        calls->function = call->ident;
        OutputFile.text << calls;
    }
    else if (dynamic_cast<AST::Push *>(STMT) != nullptr)
    {
        AST::Push * push = dynamic_cast<AST::Push *>(STMT);
        ASMC::Mov * count = new ASMC::Mov;
        count->size = ASMC::QWord;
        count->to = this->registers["%rdx"]->qWord;
        count->from = "$1";
        ASMC::Mov * pointer = new ASMC::Mov;
        pointer->size = ASMC::QWord;
        pointer->to = this->registers["%rsi"]->qWord;
        pointer->from = this->GenExpr(push->expr, OutputFile).access;
        ASMC::Mov * callnum = new ASMC::Mov;
        callnum->size = ASMC::QWord;
        callnum->to = this->registers["%rax"]->qWord;
        callnum->from = "$1";
        ASMC::Mov * rdi = new ASMC::Mov;
        rdi->size = ASMC::QWord;
        rdi->from = "$1";
        rdi->to = this->registers["%rdi"]->qWord;

        OutputFile.text << rdi;
        OutputFile.text << pointer;
        OutputFile.text << count;
        OutputFile.text << callnum;
        
        OutputFile.text << new ASMC::SysCall;
    }else if (dynamic_cast<AST::Pull *>(STMT) != nullptr)
    {
        AST::Pull * pull = dynamic_cast<AST::Pull *>(STMT);
        ASMC::Mov * count = new ASMC::Mov;
        count->size = ASMC::QWord;
        count->to = this->registers["%rdx"]->qWord;
        count->from = "$1";
        ASMC::Mov * pointer = new ASMC::Mov;
        pointer->size = ASMC::QWord;
        pointer->to = this->registers["%rsi"]->qWord;
        pointer->from = this->GenExpr(pull->expr, OutputFile).access;
        ASMC::Mov * callnum = new ASMC::Mov;
        callnum->size = ASMC::QWord;
        callnum->to = this->registers["%rax"]->qWord;
        callnum->from = "$0";
        ASMC::Mov * rdi = new ASMC::Mov;
        rdi->size = ASMC::QWord;
        rdi->from = "$1";
        rdi->to = this->registers["%rdi"]->qWord;

        OutputFile.text << rdi;
        OutputFile.text << pointer;
        OutputFile.text << count;
        OutputFile.text << callnum;
        
        OutputFile.text << new ASMC::SysCall;
    }
    else if (dynamic_cast<AST::If *>(STMT) != nullptr)
    {
        AST::If ifStmt = *dynamic_cast<AST::If *>(STMT);

        ASMC::Lable * lable1 = new ASMC::Lable();
        lable1->lable = ".L" + std::to_string(this->lablecount);
        this->lablecount ++;

        gen::Expr expr1 =this->GenExpr(ifStmt.Condition->expr1, OutputFile);
        gen::Expr expr2 =this->GenExpr(ifStmt.Condition->expr2, OutputFile);
    
        ASMC::Mov * mov1 = new ASMC::Mov();
        ASMC::Mov * mov2 = new ASMC::Mov();

        mov1->size = expr1.size;
        mov2->size = expr2.size;

        mov1->from = expr1.access;
        mov2->from = expr2.access;

        switch (mov1->size)
        {
        case ASMC::Byte :
            mov1->to = this->registers["%eax"]->byte;
            break;
        case ASMC::Word :
            mov1->to = this->registers["%eax"]->word;
            break;
        case ASMC::DWord:
            mov1->to = this->registers["%eax"]->dWord;
            break;
        case ASMC::QWord :
            mov1->to = this->registers["%eax"]->qWord;
            break;
        default:
            break;
        }
        
        switch (mov2->size)
        {
        case ASMC::Byte :
            mov2->to = this->registers["%ecx"]->byte;
            break;
        case ASMC::Word :
            mov2->to = this->registers["%ecx"]->word;
            break;
        case ASMC::DWord:
            mov2->to = this->registers["%ecx"]->dWord;
            break;
        case ASMC::QWord :
            mov2->to = this->registers["%ecx"]->qWord;
            break;
        default:
            break;
        }

        ASMC::Cmp * cmp = new ASMC::Cmp();
        ASMC::Jne * jne = new ASMC::Jne();

        cmp->from = mov2->to;
        cmp->to = mov1->to;
        cmp->size = expr1.size;

        
        OutputFile.text << mov1;
        OutputFile.text << mov2;
        OutputFile.text << cmp;

        switch (ifStmt.Condition->op)
        {
        case AST::Equ:
        {
            jne->to = lable1->lable;

            
            OutputFile.text << jne;
            OutputFile << this->GenSTMT(ifStmt.statment);
            OutputFile.text << lable1;
            break;
        }
        case AST::NotEqu :
        {
            ASMC::Je * je = new ASMC::Je();

            je->to = lable1->lable;

            OutputFile.text << je;
            OutputFile << this->GenSTMT(ifStmt.statment);
            OutputFile.text << lable1;
            break;
        }
        case AST::Great :
        {
            ASMC::Jl * jl = new ASMC::Jl();

            jl->to = lable1->lable;

            OutputFile.text << jl;
            OutputFile << this->GenSTMT(ifStmt.statment);
            OutputFile.text << lable1;
            break;
        }
        case AST::Less :
        {
            ASMC::Jg * jg = new ASMC::Jg();

            jg->to = lable1->lable;


            OutputFile.text << jg;
            OutputFile << this->GenSTMT(ifStmt.statment);
            OutputFile.text << lable1;
            break;
        }
        
        default:
            break;
        }
    }
    else if(dynamic_cast<AST::While *>(STMT) != nullptr){
        AST::While * loop = dynamic_cast<AST::While *>(STMT);

        ASMC::Lable * lable1 = new ASMC::Lable();
        lable1->lable = ".L" + std::to_string(this->lablecount);
        this->lablecount++;

        ASMC::Lable * lable2 = new ASMC::Lable();
        lable2->lable = ".L" + std::to_string(this->lablecount);
        this->lablecount++;

        ASMC::Jmp * jmp = new ASMC::Jmp();
        jmp->to = lable2->lable;
        OutputFile.text << jmp;

        OutputFile.text << lable1;

        OutputFile << this->GenSTMT(loop->stmt);

        OutputFile.text << lable2;

        gen::Expr expr1 =this->GenExpr(loop->condition->expr1, OutputFile);
        gen::Expr expr2 =this->GenExpr(loop->condition->expr2, OutputFile);
    
        ASMC::Mov * mov1 = new ASMC::Mov();
        ASMC::Mov * mov2 = new ASMC::Mov();

        mov1->size = expr1.size;
        mov2->size = expr2.size;

        mov1->from = expr1.access;
        mov2->from = expr2.access;


        switch (mov1->size)
        {
        case ASMC::Byte :
            mov1->to = this->registers["%eax"]->byte;
            break;
        case ASMC::Word :
            mov1->to = this->registers["%eax"]->word;
            break;
        case ASMC::DWord:
            mov1->to = this->registers["%eax"]->dWord;
            break;
        case ASMC::QWord :
            mov1->to = this->registers["%eax"]->qWord;
            break;
        default:
            break;
        }
        
        switch (mov2->size)
        {
        case ASMC::Byte :
            mov2->to = this->registers["%ecx"]->byte;
            break;
        case ASMC::Word :
            mov2->to = this->registers["%ecx"]->word;
            break;
        case ASMC::DWord:
            mov2->to = this->registers["%ecx"]->dWord;
            break;
        case ASMC::QWord :
            mov2->to = this->registers["%ecx"]->qWord;
            break;
        default:
            break;
        }

        ASMC::Cmp * cmp = new ASMC::Cmp();
        ASMC::Jne * jne = new ASMC::Jne();

        cmp->from = mov2->to;
        cmp->to = mov1->to;
        cmp->size = expr1.size;

        
        OutputFile.text << mov1;
        OutputFile.text << mov2;
        OutputFile.text << cmp;

        switch (loop->condition->op)
        {
        case AST::Equ:
        {
            ASMC::Je * je = new ASMC::Je();
            jne->to = lable1->lable;
            OutputFile.text << je;
            break;
        }
        case AST::NotEqu :
        {
            ASMC::Jne * jne = new ASMC::Jne();

            jne->to = lable1->lable;
            OutputFile.text << jne;
            break;
        }
        case AST::Great :
        {
            ASMC::Jg * jg = new ASMC::Jg();

            jg->to = lable1->lable;

            OutputFile.text << jg;
            break;
        }
        case AST::Less :
        {
            ASMC::Jl * jl = new ASMC::Jl();

            jl->to = lable1->lable;
            OutputFile.text << jl;
            break;
        }
        }
        
    }
    else{
        OutputFile.text.push(new ASMC::Instruction());
    }

    return OutputFile;
}