#include "Parser.hpp"


AST::Statment* parse::Parser::parseStmt(links::LinkedList<lex::Token*> &tokens){
    AST::Statment* output = new AST::Statment;
    if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        if(obj.meta == "oread"){
            AST::ORead * oread = new AST::ORead;
            //print->expr = this->parseExpr(tokens);
            output = oread;
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
    return output;
}

// AST::Stmt* parse::Parser::parseArgs(links::LinkedList<lex::Token*> &tokens){
//     AST::Stmt* output = new AST::Stmt;
//     if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
//         lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
//         tokens.pop();
//         if(obj.meta == "int"){
//             AST::Declare * declare = new AST::Declare;
//             declare->dataType = AST::Int;
//             if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
//                 lex::LObj l = *dynamic_cast<lex::LObj *>(tokens.pop());
//                 declare->symbol = l.meta;
//                 declare->dataType = AST::Int;
//                 output = declare;
//             }
            
//         }
//     }

//     if (tokens.head == nullptr){
//         throw "Bad Move";
//     } else if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr & tokens.head->next != nullptr)
//     {
//         lex::OpSym obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
//         tokens.pop();

//         if(obj.Sym == ','){
//         AST::Sequence * s = new AST::Sequence;
//         s->stmt1 = output;
//         s->stmt2 = this->parseStmt(tokens);
//         return s;
//         }
//         else if(obj.Sym == ')'){
//             return output;
//         }
//     }
//     return output;
// }


// AST::Expr* parse::Parser::parseExpr(links::LinkedList<lex::Token*> &tokens){
//     if (dynamic_cast<lex::StringObj *>(tokens.peek()) != nullptr){
//         lex::StringObj stringObj = *dynamic_cast<lex::StringObj *>(tokens.peek());
//         tokens.pop();
//         AST::StringLiteral * slit = new AST::StringLiteral();
//         slit->value = stringObj.value;
//         return slit;
//     }
//     return nullptr;
// }