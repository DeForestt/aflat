#include "Parser.hpp"


AST::Stmt* parse::Parser::parseStmt(links::LinkedList<lex::Token*> &tokens){
    AST::Stmt* output = new AST::Stmt;
    if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        if(obj.meta == "print"){
            AST::Print * print = new AST::Print;
            print->expr = this->parseExpr(tokens);
            output = print;
        }
        else if(obj.meta == "int"){
            AST::Declare * declare = new AST::Declare;
            declare->dataType = AST::Int;
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj l = *dynamic_cast<lex::LObj *>(tokens.pop());
                declare->symbol = l.meta;
                declare->dataType = AST::Int;
                output = declare;
            }
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr)
            {
                lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                if (sym.Sym == '('){
                    AST::FunctionDec * funcDec = new AST::FunctionDec;
                    funcDec->Symbol = declare->symbol;
                    funcDec->dataType = declare->dataType;
                    tokens.pop();
                    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                        sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                            tokens.pop();
                            if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                               sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                                if (sym.Sym == '{'){
                                    tokens.pop();
                                    funcDec->body = this->parseStmt(tokens);
                                }
                            }
                        }
                    output = funcDec;
                    delete(declare);
                    }
                }
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
        s->stmt1 = output;
        s->stmt2 = this->parseStmt(tokens);
        this->Output = *s;
        return s;
        }
        else if(obj.Sym == '}'){
            this->Output = *output;
            return output;
        }
    }
    return output;
}

AST::Stmt* parse::Parser::parseArgs(links::LinkedList<lex::Token*> &tokens){
    AST::Stmt* output = new AST::Stmt;
    if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        if(obj.meta == "int"){
            AST::Declare * declare = new AST::Declare;
            declare->dataType = AST::Int;
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj l = *dynamic_cast<lex::LObj *>(tokens.pop());
                declare->symbol = l.meta;
                declare->dataType = AST::Int;
                output = declare;
            }
            
        }
    }

    if (tokens.head == nullptr){
        throw "Bad Move";
    } else if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr & tokens.head->next != nullptr)
    {
        lex::OpSym obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
        tokens.pop();

        if(obj.Sym == ','){
        AST::Sequence * s = new AST::Sequence;
        s->stmt1 = output;
        s->stmt2 = this->parseStmt(tokens);
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
        slit->value = stringObj.value;
        return slit;
    }
    return nullptr;
}