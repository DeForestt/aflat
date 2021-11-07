#include "Parser.hpp"
#include "Exceptions.hpp"
#include "AST.hpp"

parse::Parser::Parser(){
    this->typeList.foo = AST::Type::compair;

    // Int Type
    AST::Type Int = AST::Type();
    Int.typeName = "int";
    Int.opType = ASMC::Hard;
    Int.size = ASMC::DWord;

    // Char Type
    AST::Type Char = AST::Type();
    Char.typeName = "char";
    Char.opType = ASMC::Hard;
    Char.size = ASMC::Byte;
    
    // Pointer Type
    AST::Type Adr = AST::Type();
    Adr.typeName = "adr";
    Adr.opType = ASMC::Hard; 
    Adr.size = ASMC::QWord;

    // Byte type
    AST::Type Byte = AST::Type();
    Byte.typeName = "byte";
    Byte.opType = ASMC::Hard;
    Byte.size = ASMC::Byte;

    // Float Type
    AST::Type Float = AST::Type();
    Float.typeName = "float";
    Float.opType = ASMC::Float;
    Float.size = ASMC::DWord;

    // Add Types to TypeList
    this->typeList << Int;
    this->typeList << Char;
    this->typeList << Byte;
    this->typeList << Adr;
    this->typeList << Float; 
}

/*
 * function name: Parser::parseStmt
 * description: Recursive function that parses a statement and returns the AST as a Statment
 * parameters: LinkedList<Token> &tokens - the list of tokens to parse
 * return: AST::Statement - the parsed statement
 */
AST::Statment* parse::Parser::parseStmt(links::LinkedList<lex::Token*> &tokens){
    AST::Statment* output = new AST::Statment;
    if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
        bool mask = false;
        // the default access modifier is public
        AST::ScopeMod scope = AST::Public;
        tokens.pop();
        
        // check if the next token is a scope modifier
        if(obj.meta == "public"){
            // set the scope modifier to public
            scope = AST::Public;
            if(dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr) throw err::Exception("Expected statent after public");
            obj = *dynamic_cast<lex::LObj *>(tokens.peek());
            tokens.pop();
        } else if(obj.meta == "private"){
            // set the scope modifier to private
            scope = AST::Private;
            if(dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr) throw err::Exception("Expected statent after mask");
            obj = *dynamic_cast<lex::LObj *>(tokens.peek());
            tokens.pop();
        }

        //Declare a variable
        if(typeList[obj.meta] != nullptr){
            AST::Declare * dec = new AST::Declare();
            //ensures the the current token is an Ident
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj Ident = *dynamic_cast<lex::LObj *>(tokens.peek());
                tokens.pop();
                dec->Ident = Ident.meta;
                dec->type = *this->typeList[obj.meta];
                dec->mask = mask;
                dec->scope = scope;
                output = dec;
                if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                    lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                    std::string scopeName = "global";
                    if (sym.Sym == '@'){
                        tokens.pop();
                        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                            lex::LObj scopeObj = *dynamic_cast<lex::LObj *>(tokens.pop());
                            scopeName = scopeObj.meta;
                        } else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + "Expected Scope Name");
                        if(dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +" Can Only Scope a Function");
                        sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                        if (sym.Sym != '(')  throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Can Only Scope a Function");
                    }
                    //Checking for Perenth to see if it is a function
                    if (sym.Sym == '('){
                        tokens.pop();
                        AST::Function * func = new AST::Function();
                        func->ident.ident = dec->Ident;
                        func->type = dec->type;
                        func->scopeName = scopeName;
                        func->mask = mask;
                        func->scope = scope;
                        func->args = this->parseArgs(tokens, ',', ')');
                        if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                            sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                            if (sym.Sym == '{'){
                                tokens.pop();
                                func->statment = this->parseStmt(tokens);
                                output = func;
                                delete(dec);
                            }else{
                                func->statment = nullptr;
                                output = func;
                                delete(dec);
                            };
                        }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Need terminating symble or open symble");
                    }else if (sym.Sym == '='){
                        tokens.pop();
                        AST::DecAssign * assign = new AST::DecAssign;
                        assign->declare = dec;
                        assign->expr = this->parseExpr(tokens);
                        output = assign;
                    }
                    
                }
                
            } else if(dynamic_cast<lex::INT *>(tokens.peek()) != nullptr){
                lex::INT Int = *dynamic_cast<lex::INT *>(tokens.pop());
                if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                    lex::LObj Ident = *dynamic_cast<lex::LObj *>(tokens.pop());
                    AST::DecArr * decA = new AST::DecArr();
                    decA->count = std::stoi(Int.value);
                    decA->ident = Ident.meta;
                    if(this->typeList[obj.meta] == nullptr) throw err::Exception("Line: " + std::to_string(Ident.lineCount) +" Unknown type " + obj.meta);
                    decA->type = *this->typeList[obj.meta];
                    output = decA;
                }
            }
            else{
                throw err::Exception("Line: " + std::to_string(obj.lineCount) + "Unparsable token found" );
            }
        }
        else if (obj.meta == "return")
        {
            AST::Return * ret = new AST::Return;
            ret->expr = this->parseExpr(tokens);
            output = ret;
        } else if (obj.meta == "push")
        {
            AST::Push * push = new AST::Push;
            push->expr = this->parseExpr(tokens);
            output = push;
        } else if(obj.meta == "pull"){
            AST::Pull * pull = new AST::Pull;
            pull->expr = this->parseExpr(tokens);
            output = pull;
        }else if (obj.meta == "if")
        {
            AST::If * ifstmt = new AST::If;
            ifstmt ->Condition = this->parseCondition(tokens);
            if(dynamic_cast<lex::OpSym * >(tokens.peek()) != nullptr){
                lex::OpSym sym = *dynamic_cast<lex::OpSym * >(tokens.pop());
                if(sym.Sym == '{'){
                    ifstmt->statment = this->parseStmt(tokens);
                    output = ifstmt;
                }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened If");
            }
        }else if(obj.meta == "while"){
            AST::While * loop = new AST::While;
            
            loop->condition = this->parseCondition(tokens);

            if(dynamic_cast<lex::OpSym * >(tokens.peek()) != nullptr){
                lex::OpSym sym = *dynamic_cast<lex::OpSym * >(tokens.pop());
                if(sym.Sym == '{'){
                        loop->stmt = this->parseStmt(tokens);
                        output = loop;
                    }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened loop");
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened loop");
            
        }
        else if(obj.meta == "struct"){
            AST::UDeffType * stc = new AST::UDeffType();
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj ident = *dynamic_cast<lex::LObj *>(tokens.pop());
                stc->ident.ident = ident.meta;
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " struct needs Ident");
            if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                lex::OpSym op = *dynamic_cast<lex::OpSym *>(tokens.pop());
                if(op.Sym != '{')throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened UDeffType");
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened UDeffType");
            stc->statment = this->parseStmt(tokens);
            AST::Type t = AST::Type();
            t.size = ASMC::QWord;
            t.typeName = stc->ident.ident;
            this->typeList << t;
            output = stc;
        }else if(obj.meta == "class"){
            AST::Class * item = new AST::Class();
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj ident = *dynamic_cast<lex::LObj *>(tokens.pop());
                item->ident.ident = ident.meta;
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " struct needs Ident");
            if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                lex::OpSym op = *dynamic_cast<lex::OpSym *>(tokens.pop());
                if(op.Sym != '{')throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened UDeffType");
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened UDeffType");
            item->statment = this->parseStmt(tokens);
            AST::Type t = AST::Type();
            t.size = ASMC::QWord;
            t.typeName = item->ident.ident;
            this->typeList << t;
            output = item;
        }
        else{
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                lex::OpSym sym = *dynamic_cast<lex::OpSym *> (tokens.pop());
                links::LinkedList<std::string> modList;
                while (sym.Sym == '.')
                {
                    if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                        lex::LObj mod = *dynamic_cast<lex::LObj *>(tokens.pop());
                        modList << mod.meta;
                    }else throw err::Exception("Expected, Ident after dot.");
                    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                        sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
                    }else throw err::Exception("expected assignment oporator got " + sym.Sym);
                }
                if(sym.Sym == '='){
                    AST::Assign * assign = new AST::Assign();
                    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                        lex::OpSym * s2 = dynamic_cast<lex::OpSym *>(tokens.peek());
                        if (s2->Sym == ':'){
                            assign->refrence = true;
                            tokens.pop();
                        };
                    };
                    
                    assign->Ident = obj.meta;
                    assign->modList = modList;
                    assign->expr = this->parseExpr(tokens);
                    output = assign;
                }else if (sym.Sym == '(')
                {
                    AST::Call * call = new AST::Call();
                    call->ident = obj.meta;
                    call->modList = modList;
                    bool pop = false;
                    if(dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr){
                        do{
                            if (pop) tokens.pop();
                            call->Args.push(this->parseExpr(tokens));
                            pop = true;
                        }while(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr
                        && dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');
                        if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                            lex::OpSym * symp = dynamic_cast<lex::OpSym *> (tokens.pop());
                            if (symp->Sym != ')') throw err::Exception("Expected closed perenth got " + symp->Sym);
                        }
                    }else{
                        lex::OpSym * symp = dynamic_cast<lex::OpSym *> (tokens.pop());
                        if (symp->Sym != ')') throw err::Exception("Expected closed perenth got " + symp->Sym);
                    }
                    output = call;
                }
                else throw err::Exception("expected assignment oporator after " + obj.meta);
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " expected Asignment oporator after " + obj.meta);
        }
    }
    
    if (tokens.head == nullptr){ 
        this->Output = *output;
        return output;
    } else if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr & tokens.head->next != nullptr)
    {
        lex::OpSym obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
        tokens.pop();

        if(obj.Sym == ';'){
        AST::Sequence * s = new AST::Sequence; 
        s->Statment1 = output;
        s->Statment2 = this->parseStmt(tokens);
        this->Output = *s;
        return s;
        }
        else if(obj.Sym == '}'){
            this->Output = *output;
            return output;
        }
    } 
    else{ 
        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr & tokens.head->next == nullptr){
            lex::OpSym obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
            tokens.pop();
            if(obj.Sym == '}'){
                this->Output = *output;
                return nullptr;
            }else if (obj.Sym == ';')
            {
                this->Output = *output;
                return output;
            }
            
        }
    }
    return output;
}

AST::Statment* parse::Parser::parseArgs(links::LinkedList<lex::Token*> &tokens, char delimn, char close){
    AST::Statment* output = new AST::Statment();
    if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        if(typeList[obj.meta] != nullptr){
            AST::Declare * dec = new AST::Declare();
            //ensures the the current token is an Ident
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj Ident = *dynamic_cast<lex::LObj *>(tokens.peek());
                tokens.pop();
                dec->Ident = Ident.meta;
                dec->type = *typeList[obj.meta];
                output = dec;
            }
        }
    }

    if (tokens.head == nullptr){
        throw err::Exception("unterminated functioncall");
    } else if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr & tokens.head->next != nullptr)
    {
        lex::OpSym obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
        tokens.pop();

        if(obj.Sym == delimn){
        AST::Sequence * s = new AST::Sequence;
        s->Statment1 = output;
        s->Statment2 = this->parseArgs(tokens, delimn, close);
        return s;
        }
        else if(obj.Sym == close){
            return output;
        }
    }
    return output;
}

AST::ConditionalExpr* parse::Parser::parseCondition(links::LinkedList<lex::Token*> &tokens){

    AST::ConditionalExpr * output = new AST::ConditionalExpr();

    if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
        lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
        if(sym.Sym != '(') throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + "unOpened Condition.  Please open with: (");
    }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " unOpened Condition.  Please open with: (");

    output->expr1 = this->parseExpr(tokens);

    if(dynamic_cast<lex::Symbol *>(tokens.peek()) != nullptr){
        lex::Symbol sym = *dynamic_cast<lex::Symbol *>(tokens.pop());
        if(sym.meta == "=="){
            output->op = AST::Equ;
        }else if(sym.meta == "!="){
            output->op = AST::NotEqu;
        }else if(sym.meta == "<"){
            output->op = AST::Less;
        }else if(sym.meta == ">"){
            output->op = AST::Great;
        }
    }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Condition with now conditional Oporator");

    output->expr2 = this->parseExpr(tokens);

    if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
        lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
        if(sym.Sym != ')') throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " unTerminated Condition.  Please terminate with: )");
    }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " unTerminated Condition.  Please terminate with: )");

    return output;
}

AST::Expr* parse::Parser::parseExpr(links::LinkedList<lex::Token*> &tokens){
    AST::Expr * output = new AST::Expr();
    if (dynamic_cast<lex::StringObj *>(tokens.peek()) != nullptr){
        lex::StringObj stringObj = *dynamic_cast<lex::StringObj *>(tokens.peek());
        tokens.pop();
        AST::StringLiteral * slit = new AST::StringLiteral();
        slit->val = stringObj.value;
        output = slit;
    } else if(dynamic_cast<lex::INT *>(tokens.peek()) != nullptr){
        lex::INT intObj = *dynamic_cast<lex::INT *>(tokens.pop());
        AST::IntLiteral * ilit = new AST::IntLiteral();
        ilit->val = std::stoi(intObj.value);
        output = ilit;
    }
    else if(dynamic_cast<lex::FloatLit *>(tokens.peek()) != nullptr){
        lex::FloatLit floatObj = *dynamic_cast<lex::FloatLit *>(tokens.pop());
        AST::FloatLiteral * flit = new AST::FloatLiteral();
        flit->val = floatObj.value;
        output = flit;
    }
    else if(dynamic_cast<lex::Long *>(tokens.peek()) != nullptr){
        lex::Long intObj = *dynamic_cast<lex::Long *>(tokens.pop());
        AST::LongLiteral * ilit = new AST::LongLiteral();
        ilit->val = std::stoi(intObj.value);
        output = ilit;
    }
    else if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.pop());
        links::LinkedList<std::string> modList;
        if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
            lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
            while(sym.Sym == '.'){
                tokens.pop();
                if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                    lex::LObj mod = *dynamic_cast<lex::LObj *>(tokens.pop());
                    modList << mod.meta;
                }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Expected, Ident after dot.");
                if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                    sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                }else break;
            }
        }
        
        if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
            lex::LObj aobj = *dynamic_cast<lex::LObj *>(tokens.peek());
            if(aobj.meta == "as"){
                AST::DeRefence * deRef = new AST::DeRefence();
                tokens.pop();
                if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                    lex::LObj view = * dynamic_cast<lex::LObj *>(tokens.pop());
                    deRef->Ident = obj.meta;
                    deRef->modList = modList;
                    if(this->typeList[view.meta] == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unknown Type " + view.meta);
                    deRef->type = *this->typeList[view.meta];
                    output = deRef;
                }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " No dereffrens type given with as");
            }
        }else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr)
        {
            lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
            if (sym.Sym == '('){
                tokens.pop();
                AST::Call * call = new AST::Call();
                call->ident = obj.meta;
                call->modList = modList;
                bool pop = false;
                if(dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr){
                    do{
                        if (pop) tokens.pop();
                        call->Args.push(this->parseExpr(tokens));
                        pop = true;
                    }while(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr
                    && dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');

                    if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                        lex::OpSym * symp = dynamic_cast<lex::OpSym *> (tokens.pop());
                        if (symp->Sym != ')') throw err::Exception("Expected closed perenth got " + symp->Sym);
                    }
                }else{
                    lex::OpSym * symp = dynamic_cast<lex::OpSym *> (tokens.pop());
                    if (symp->Sym != ')') throw err::Exception("Expected closed perenth got " + symp->Sym);
                }

                AST::CallExpr * callExpr = new AST::CallExpr;
                callExpr->call = call;

                output = callExpr;
            }else{
                AST::Var * var = new AST::Var();
                var->Ident = obj.meta;
                var->modList = modList;
                output = var;
            }
        }
         else {
            AST::Var * var = new AST::Var();
            var->Ident = obj.meta;
            var->modList = modList;
            output = var;
        }
    }
    else if (dynamic_cast<lex::CharObj *>(tokens.peek()) != nullptr){
        lex::CharObj obj = *dynamic_cast<lex::CharObj *>(tokens.pop());
        AST::CharLiteral * charlit = new AST::CharLiteral();
        charlit->value = obj.value;
        output = charlit;
    }
    else if (dynamic_cast<lex::Ref *>(tokens.peek()) != nullptr){
        tokens.pop();
        AST::Refrence * ref = new AST::Refrence();
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
            lex::LObj obj = * dynamic_cast<lex::LObj *>(tokens.pop());
            ref->Ident = obj.meta;
            output = ref;
        }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " No object given to refrece");
    }
    else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unknown Expr");

    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){

        AST::Compound  * compound = new AST::Compound();

        lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
        if (sym.Sym == '+')
        {
            tokens.pop();
            compound->op = AST::Plus;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '-')
        {
            tokens.pop();
            compound->op = AST::Minus;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '*')
        {
            tokens.pop();
            compound->op = AST::Mul;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '/')
        {
            tokens.pop();
            compound->op = AST::Div;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '%')
        {
            tokens.pop();
            compound->op = AST::Mod;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '&')
        {
            tokens.pop();
            compound->op = AST::AndBit;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '>'){
            tokens.pop();
            compound->op = AST::Great;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '<'){
            tokens.pop();
            compound->op = AST::Less;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
    }
    return output;
}