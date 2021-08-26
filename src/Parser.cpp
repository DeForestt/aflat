#include "Parser.hpp"
#include "Exceptions.hpp"


AST::Statment* parse::Parser::parseStmt(links::LinkedList<lex::Token*> &tokens){
    AST::Statment* output = new AST::Statment;
    if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        //Declare a byte;
        if(obj.meta == "byte" | obj.meta == "int" | obj.meta == "string"){
            AST::Declare * dec = new AST::Declare();
            //ensures the the current token is an Ident
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
                tokens.pop();
                dec->Ident = obj.meta;
                if(obj.meta == "byte") dec->type = AST::Byte; else if (obj.meta == "int") dec->type = AST::Int; else if (obj.meta == "string")dec->type = AST::String;
                output = dec;
                
                
                if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                    lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                    //Checking for Perenth to see if it is a function
                    if (sym.Sym == '('){
                        tokens.pop();
                        AST::Function * func = new AST::Function();
                        func->ident.ident = dec->Ident;
                        func->type = dec->type;
                        func->args = this->parseArgs(tokens);
                        if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                            sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                            if (sym.Sym == '{'){
                                tokens.pop();
                                func->statment = this->parseStmt(tokens);
                                output = func;
                                delete(dec);
                            }
                        }
                    }else if (sym.Sym == '=')
                    {
                        tokens.pop();
                        AST::DecAssign * assign = new AST::DecAssign;
                        assign->declare = dec;
                        assign->expr = this->parseExpr(tokens);
                        output = assign;
                    }
                    
                }
                
            }
            else{
                throw err::Exception("Unparsable token found");
            }
        }
        else if (obj.meta == "return")
        {
            AST::Return * ret = new AST::Return;
            ret->expr = this->parseExpr(tokens);
            output = ret;
        }else{
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                lex::OpSym * sym = dynamic_cast<lex::OpSym *> (tokens.pop());
                if(sym->Sym == '='){
                    AST::Assign * assign = new AST::Assign();
                    assign->Ident = obj.meta;
                    assign->expr = this->parseExpr(tokens);
                    output = assign;
                }else if (sym->Sym == '(')
                {
                    AST::Call * call = new AST::Call();
                    call->ident = obj.meta;
                    call->Args = this->parseArgs(tokens);
                    output = call;
                }
                 else throw err::Exception("expected assignment oporator");
            }else throw err::Exception("expected Asignment oporator after " + obj.meta);
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
    } else{ if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr & tokens.head->next == nullptr){
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

AST::Statment* parse::Parser::parseArgs(links::LinkedList<lex::Token*> &tokens){
    AST::Statment* output = new AST::Statment();
    if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        if(obj.meta == "int"){
            AST::Declare * declare = new AST::Declare;
            declare->type = AST::Int;
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj l = *dynamic_cast<lex::LObj *>(tokens.pop());
                declare->Ident = l.meta;
                declare->type = AST::Int;
                output = declare;
            }
        }
    }

    if (tokens.head == nullptr){
        throw err::Exception("unterminated functioncall");
    } else if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr & tokens.head->next != nullptr)
    {
        lex::OpSym obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
        tokens.pop();

        if(obj.Sym == ','){
        AST::Sequence * s = new AST::Sequence;
        s->Statment1 = output;
        s->Statment2 = this->parseStmt(tokens);
        return s;
        }
        else if(obj.Sym == ')'){
            return output;
        }
    }
    return output;
}


AST::Expr* parse::Parser::parseExpr(links::LinkedList<lex::Token*> &tokens){
    if (dynamic_cast<lex::StringObj *>(tokens.peek()) != nullptr){
        lex::StringObj stringObj = *dynamic_cast<lex::StringObj *>(tokens.peek());
        tokens.pop();
        AST::StringLiteral * slit = new AST::StringLiteral();
        slit->val = stringObj.value;
        return slit;
    }
    if(dynamic_cast<lex::INT *>(tokens.peek())!=nullptr){
        lex::INT intObj = *dynamic_cast<lex::INT *>(tokens.pop());
        AST::IntLiteral * ilit = new AST::IntLiteral();
        ilit->val = std::stoi(intObj.value);
        return ilit;
    }
    else throw(tokens.peek());
}