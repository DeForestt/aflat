#include "Parser.hpp"
#include "Exceptions.hpp"
#include "AST.hpp"

parse::Parser::Parser(){
    this->typeList.foo = ast::Type::compair;

    // Int Type
    ast::Type Int = ast::Type();
    Int.typeName = "int";
    Int.opType = asmc::Hard;
    Int.size = asmc::DWord;

    // Sort Type
    ast::Type Short = ast::Type();
    Short.typeName = "short";
    Short.opType = asmc::Hard;
    Short.size = asmc::Word;


    // Char Type
    ast::Type Char = ast::Type();
    Char.typeName = "char";
    Char.opType = asmc::Hard;
    Char.size = asmc::Byte;

    // Long Type
    ast::Type Long = ast::Type();
    Long.typeName = "long";
    Long.opType = asmc::Hard;
    Long.size = asmc::QWord;
    
    // Pointer Type
    ast::Type Adr = ast::Type();
    Adr.typeName = "adr";
    Adr.opType = asmc::Hard; 
    Adr.size = asmc::QWord;

    // Byte type
    ast::Type Byte = ast::Type();
    Byte.typeName = "byte";
    Byte.opType = asmc::Hard;
    Byte.size = asmc::Byte;

    // Float Type
    ast::Type Float = ast::Type();
    Float.typeName = "float";
    Float.opType = asmc::Float;
    Float.size = asmc::DWord;

    // Add Types to TypeList
    this->typeList << Int;
    this->typeList << Char;
    this->typeList << Byte;
    this->typeList << Adr;
    this->typeList << Float; 
    this->typeList << Short;
    this->typeList << Long;
}

/*
 * function name: Parser::parseStmt
 * description: Recursive function that parses a statement and returns the AST as a Statment
 * parameters: LinkedList<Token> &tokens - the list of tokens to parse
 * return: AST::Statement - the parsed statement
 */
ast::Statment* parse::Parser::parseStmt(links::LinkedList<lex::Token*> &tokens, bool singleStmt = false){
    ast::Statment* output = new ast::Statment;
    if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
        bool mask = false;
        // the default access modifier is public
        ast::ScopeMod scope = ast::Public;
        tokens.pop();
        
        // check if the next token is a scope modifier
        if(obj.meta == "public"){
            // set the scope modifier to public
            scope = ast::Public;
            if(dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr) throw err::Exception("Expected statent after public on line " + std::to_string(obj.lineCount));
            obj = *dynamic_cast<lex::LObj *>(tokens.peek());
            tokens.pop();
        } else if(obj.meta == "private"){
            // set the scope modifier to private
            scope = ast::Private;
            if(dynamic_cast<lex::LObj *>(tokens.peek()) == nullptr) throw err::Exception("Expected statent after private one line " + std::to_string(obj.lineCount));
            obj = *dynamic_cast<lex::LObj *>(tokens.peek());
            tokens.pop();
        }

        //Declare a variable
        if(typeList[obj.meta] != nullptr){
            ast::Declare * dec = new ast::Declare();
            //ensures the the current token is an Ident
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj Ident = *dynamic_cast<lex::LObj *>(tokens.peek());
                tokens.pop();
                dec->Ident = Ident.meta;
                dec->type = *this->typeList[obj.meta];
                dec->mask = mask;
                dec->scope = scope;
                output = dec;
                ast::Op overload = ast::Op::None;
                if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                    lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                    std::string scopeName = "global";
                    if (sym.Sym == '<'){
                        tokens.pop();
                        lex::OpSym * next = dynamic_cast<lex::OpSym *>(tokens.pop());
                        if(next == nullptr) throw err::Exception("Expected and overloade operator on line " + std::to_string(obj.lineCount));
                        if(next->Sym == '>'){
                            overload = ast::Op::Less;
                        } else if(next->Sym == '<'){
                            overload = ast::Op::Great;
                        } else if(next->Sym == '+'){
                            overload = ast::Op::Plus;
                        } else if(next->Sym == '-'){
                            overload = ast::Op::Minus;
                        } else if(next->Sym == '*'){
                            overload = ast::Op::Mul;
                        } else if(next->Sym == '/'){
                            overload = ast::Op::Div;
                        } else if(next->Sym == '%'){
                            overload = ast::Op::Mod;
                        } else if(next->Sym == '!'){
                            overload = ast::Op::NotEqu;
                        } else if(next->Sym == '&'){
                            overload = ast::Op::AndBit;
                        } else if(next->Sym == '|'){
                            overload = ast::Op::OrBit;
                        } else if(next->Sym == '='){
                            overload = ast::Op::Equ;
                        }else{
                            throw err::Exception("Expected and overloade operator on line " + std::to_string(obj.lineCount));
                        }

                        next = dynamic_cast<lex::OpSym *>(tokens.pop());
                        if (next == nullptr) throw err::Exception("Expected a close oporator " + std::to_string(obj.lineCount));
                        if(next->Sym != '>') throw err::Exception("Expected a close oporator  " + std::to_string(obj.lineCount) +  + " got " + next->Sym);
                        if(dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr) throw err::Exception("Expected a symbol on line " + std::to_string(obj.lineCount));
                        sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                    }
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
                        ast::Function * func = new ast::Function();
                        func->ident.ident = dec->Ident;
                        func->type = dec->type;
                        func->scopeName = scopeName;
                        func->mask = mask;
                        func->scope = scope;
                        func->op = overload;
                        func->args = this->parseArgs(tokens, ',', ')', func->argTypes);
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
                        ast::DecAssign * assign = new ast::DecAssign;
                        assign->declare = dec;
                        assign->expr = this->parseExpr(tokens);
                        output = assign;
                    }
                    
                }
                
            } else if(dynamic_cast<lex::INT *>(tokens.peek()) != nullptr){
                lex::INT Int = *dynamic_cast<lex::INT *>(tokens.pop());
                if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                    lex::LObj Ident = *dynamic_cast<lex::LObj *>(tokens.pop());
                    ast::DecArr * decA = new ast::DecArr();
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
            ast::Return * ret = new ast::Return;
            ret->expr = this->parseExpr(tokens);
            output = ret;
        } else if (obj.meta == "push")
        {
            ast::Push * push = new ast::Push;
            push->expr = this->parseExpr(tokens);
            output = push;
        } else if(obj.meta == "pull"){
            ast::Pull * pull = new ast::Pull;
            pull->expr = this->parseExpr(tokens);
            output = pull;
        }else if (obj.meta == "if")
        {
            ast::If * ifstmt = new ast::If;
            ifstmt->elseIf = nullptr;
            ifstmt->elseStatment = nullptr;
            ifstmt ->Condition = this->parseCondition(tokens);
            if(dynamic_cast<lex::OpSym * >(tokens.peek()) != nullptr){
                lex::OpSym sym = *dynamic_cast<lex::OpSym * >(tokens.pop());
                if(sym.Sym == '{'){
                    ifstmt->statment = this->parseStmt(tokens);
                    output = ifstmt;
                }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened If");
                // check for else
                if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr) {
                    lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
                    if (obj.meta == "else") {
                        tokens.pop();
                        if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr) {
                            sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
                            if (sym.Sym == '{') {
                                ifstmt->elseStatment = this->parseStmt(tokens);
                                output = ifstmt;
                            } else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened Else");
                        } else if(dynamic_cast<lex::LObj *>(tokens.peek())){
                            lex::LObj word = *dynamic_cast<lex::LObj *>(tokens.peek());
                            if (word.meta == "if") {
                                ifstmt->elseStatment = this->parseStmt(tokens, true);
                                output = ifstmt;
                            }
                        } else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened Else");
                    } else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unclosed if");
                }
            }
            if (singleStmt) return ifstmt;
        }else if(obj.meta == "while"){
            ast::While * loop = new ast::While;
            
            loop->condition = this->parseCondition(tokens);

            if(dynamic_cast<lex::OpSym * >(tokens.peek()) != nullptr){
                lex::OpSym sym = *dynamic_cast<lex::OpSym * >(tokens.pop());
                if(sym.Sym == '{'){
                        loop->stmt = this->parseStmt(tokens);
                        output = loop;
                    }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened loop");
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened loop");
            
        }
        else if(obj.meta == "for"){
            ast::For * loop = new ast::For;
            if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                // opening curly bracket for statment
                if(sym.Sym != '{') throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + "Un opened for loop body");
                tokens.pop();
                loop->declare = this->parseStmt(tokens);

                loop->condition = this->parseCondition(tokens);
                if(dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + "Un opened for loop body");
                sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                if(sym.Sym != '{') throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + "Un opened for loop body");
                tokens.pop();
                loop->increment = this->parseStmt(tokens);
                if(dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + "Un opened for loop body");
                sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
                if(sym.Sym != '{') throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + "Un opened for loop body");
                tokens.pop();
                loop->Run = this->parseStmt(tokens);
                output = loop;
            } else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + "Un opened for loop header");
        }
        else if(obj.meta == "struct"){
            ast::UDeffType * stc = new ast::UDeffType();
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj ident = *dynamic_cast<lex::LObj *>(tokens.pop());
                stc->ident.ident = ident.meta;
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " struct needs Ident");
            if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                lex::OpSym op = *dynamic_cast<lex::OpSym *>(tokens.pop());
                if(op.Sym != '{')throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened UDeffType");
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened UDeffType");
            stc->statment = this->parseStmt(tokens);
            ast::Type t = ast::Type();
            t.size = asmc::QWord;
            t.typeName = stc->ident.ident;
            this->typeList << t;
            output = stc;
        }else if(obj.meta == "class"){
            ast::Class * item = new ast::Class();

            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj ident = *dynamic_cast<lex::LObj *>(tokens.pop());
                item->ident.ident = ident.meta;
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " class needs Ident");
            
            // check for the word signs
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj sig = *dynamic_cast<lex::LObj *>(tokens.pop());
                if(sig.meta == "signs"){
                    lex::LObj * ident = dynamic_cast<lex::LObj *>(tokens.pop());
                    if (ident != nullptr) {
                        if (typeList[ident->meta] == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Type " + ident->meta + " not found");
                        item->base = ident->meta;
                    } else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " signs needs Ident");
                }
            } else {
                item->base = "";
            };
            
            if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                lex::OpSym op = *dynamic_cast<lex::OpSym *>(tokens.pop());
                if(op.Sym != '{')throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened UDeffType");
            }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened UDeffType");

            // check if there is a contract
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj contract = *dynamic_cast<lex::LObj *>(tokens.peek());
                if(contract.meta == "contract"){
                    tokens.pop();
                    lex::OpSym * sy = dynamic_cast<lex::OpSym *>(tokens.pop());
                    if (sy == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened Contract");
                    if (sy->Sym != '{') throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unopened Contract");
                    item->contract = this->parseStmt(tokens);
                } else item->contract = nullptr;
            } else item->contract = nullptr;
            
            ast::Type t = ast::Type();
            t.size = asmc::QWord;
            t.typeName = item->ident.ident;
            //Check if the class is in the typeList
            if(typeList[item->ident.ident] != nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Class " + item->ident.ident + " already exists");
            this->typeList << t;
            item->statment = this->parseStmt(tokens);
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
                    }else throw err::Exception("Expected, Ident after dot. on line " + std::to_string(sym.lineCount));
                    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                        sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
                    }else throw err::Exception("expected assignment oporator got on line " + std::to_string(sym.lineCount) + " " + sym.Sym);
                }
                if(sym.Sym == '='){
                    ast::Assign * assign = new ast::Assign();
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
                    ast::Call * call = new ast::Call();
                    call->ident = obj.meta;
                    call->modList = modList;
                    bool pop = false;
                    if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr && dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ')'){
                        lex::OpSym * symp = dynamic_cast<lex::OpSym *> (tokens.pop());
                        if (symp->Sym != ')') throw err::Exception("Expected closed perenth got " + symp->Sym);
                    }else{
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
                    }
                    output = call;
                }
                else throw err::Exception("Line: " + std::to_string(obj.lineCount) + " expected assignment oporator after " + obj.meta);
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
        ast::Sequence * s = new ast::Sequence; 
        s->Statment1 = output;
        s->Statment2 = this->parseStmt(tokens);
        this->Output = *s;
        return s;
        }
        else if(obj.Sym == '}'){
            this->Output = *output;
            return output;
        }
        else if(obj.Sym == '!'){
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

ast::Statment* parse::Parser::parseArgs(links::LinkedList<lex::Token*> &tokens, char delimn, char close, std::vector<ast::Type> &types){
    ast::Statment* output = new ast::Statment();
    if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
        lex::LObj obj = *dynamic_cast<lex::LObj *>(tokens.peek());
        tokens.pop();
        if(typeList[obj.meta] != nullptr){
            ast::Declare * dec = new ast::Declare();
            //ensures the the current token is an Ident
            if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
                lex::LObj Ident = *dynamic_cast<lex::LObj *>(tokens.peek());
                tokens.pop();
                dec->Ident = Ident.meta;
                dec->type = *typeList[obj.meta];
                output = dec;
                types.push_back(dec->type);
            }
        } else throw err::Exception("Line: " + std::to_string(obj.lineCount) + " expected type got " + obj.meta);
    }

    if (tokens.head == nullptr){
        throw err::Exception("unterminated functioncall");
    } else if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr & tokens.head->next != nullptr)
    {
        lex::OpSym obj = *dynamic_cast<lex::OpSym *>(tokens.peek());
        tokens.pop();

        if(obj.Sym == delimn){
        ast::Sequence * s = new ast::Sequence;
        s->Statment1 = output;
        s->Statment2 = this->parseArgs(tokens, delimn, close, types);
        return s;
        }
        else if(obj.Sym == close){
            return output;
        }
    }
    return output;
}

ast::ConditionalExpr* parse::Parser::parseCondition(links::LinkedList<lex::Token*> &tokens){

    ast::ConditionalExpr * output = new ast::ConditionalExpr();

    if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
        lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
        if(sym.Sym != '(') throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) +"\'" +sym.Sym+ "\"" + "unOpened Condition.  Please open with: (");
    }else throw err::Exception("Line: " + '\'' + dynamic_cast<lex::LObj *>(tokens.peek())->meta + '\'' + " unOpened Condition.  Please open with: (");

    output->expr1 = this->parseExpr(tokens);

    if(dynamic_cast<lex::Symbol *>(tokens.peek()) != nullptr){
        lex::Symbol sym = *dynamic_cast<lex::Symbol *>(tokens.pop());
        if(sym.meta == "=="){
            output->op = ast::Equ;
        }else if(sym.meta == "!="){
            output->op = ast::NotEqu;
        }else if(sym.meta == "<"){
            output->op = ast::Less;
        }else if(sym.meta == ">"){
            output->op = ast::Great;
        }
    }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Condition with now conditional Oporator");

    output->expr2 = this->parseExpr(tokens);

    if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
        lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.pop());
        if(sym.Sym != ')') throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " unTerminated Condition.  Please terminate with: )");
    }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " unTerminated Condition.  Please terminate with: )");

    return output;
}

ast::Expr* parse::Parser::parseExpr(links::LinkedList<lex::Token*> &tokens){
    ast::Expr * output = new ast::Expr();
    if (dynamic_cast<lex::StringObj *>(tokens.peek()) != nullptr){
        lex::StringObj stringObj = *dynamic_cast<lex::StringObj *>(tokens.peek());
        tokens.pop();
        ast::StringLiteral * slit = new ast::StringLiteral();
        slit->val = stringObj.value;
        output = slit;
    } else if(dynamic_cast<lex::INT *>(tokens.peek()) != nullptr){
        lex::INT intObj = *dynamic_cast<lex::INT *>(tokens.pop());
        ast::IntLiteral * ilit = new ast::IntLiteral();
        ilit->val = std::stoi(intObj.value);
        output = ilit;
    }
    else if(dynamic_cast<lex::FloatLit *>(tokens.peek()) != nullptr){
        lex::FloatLit floatObj = *dynamic_cast<lex::FloatLit *>(tokens.pop());
        ast::FloatLiteral * flit = new ast::FloatLiteral();
        flit->val = floatObj.value;
        output = flit;
    }
    else if(dynamic_cast<lex::Long *>(tokens.peek()) != nullptr){
        lex::Long intObj = *dynamic_cast<lex::Long *>(tokens.pop());
        ast::LongLiteral * ilit = new ast::LongLiteral();
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
        
        if(obj.meta == "new"){
            ast::NewExpr * newExpr = new ast::NewExpr();
            lex::LObj * typeName = dynamic_cast<lex::LObj *>(tokens.pop());
            if (typeName == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Expected, Ident after new.");
            ast::Type * nType = this->typeList[typeName->meta];
            if(nType == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + ": " + typeName->meta + " is not a valid type.");
            newExpr->type = *nType;
            lex::OpSym * sym = dynamic_cast<lex::OpSym *>(tokens.peek());
            if(sym != nullptr && sym->Sym == '('){
                tokens.pop();
                lex::OpSym * testSym = dynamic_cast<lex::OpSym *>(tokens.peek());
                bool pop = false;
                newExpr->args = links::LinkedList<ast::Expr*>();
                if(testSym != nullptr && testSym->Sym != '['){
                    lex::OpSym * symp = dynamic_cast<lex::OpSym *> (tokens.pop());
                    if (symp->Sym != ')') throw err::Exception("Expected closed perenth got " + symp->Sym);
                }else{
                    do{
                        if (pop) tokens.pop();
                        newExpr->args.push(this->parseExpr(tokens));
                        pop = true;
                    }while(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr
                    && dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == ',');

                    if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
                        lex::OpSym * symp = dynamic_cast<lex::OpSym *> (tokens.pop());
                        if (symp->Sym != ')') throw err::Exception("Expected closed perenth got " + symp->Sym);
                    }
                    output = newExpr;
                }
            }
            output = newExpr;
        }
        else if(dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
            lex::LObj aobj = *dynamic_cast<lex::LObj *>(tokens.peek());
            if(aobj.meta == "as"){
                ast::DeRefence * deRef = new ast::DeRefence();
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
                ast::Call * call = new ast::Call();
                call->ident = obj.meta;
                call->modList = modList;
                bool pop = false;
                lex::OpSym * testSym = dynamic_cast<lex::OpSym *>(tokens.peek());
                if( testSym != nullptr && testSym->Sym != '['){
                    lex::OpSym * symp = dynamic_cast<lex::OpSym *> (tokens.pop());
                    if (symp->Sym != ')') throw err::Exception("Expected closed perenth got " + symp->Sym);
                }else{
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
                }

                ast::CallExpr * callExpr = new ast::CallExpr;
                callExpr->call = call;

                output = callExpr;
            }else{
                ast::Var * var = new ast::Var();
                var->Ident = obj.meta;
                var->modList = modList;
                output = var;
            }
        }
         else {
            ast::Var * var = new ast::Var();
            var->Ident = obj.meta;
            var->modList = modList;
            output = var;
        }
    }
    else if (dynamic_cast<lex::CharObj *>(tokens.peek()) != nullptr){
        lex::CharObj obj = *dynamic_cast<lex::CharObj *>(tokens.pop());
        ast::CharLiteral * charlit = new ast::CharLiteral();
        charlit->value = obj.value;
        output = charlit;
    }
    else if (dynamic_cast<lex::Ref *>(tokens.peek()) != nullptr){
        tokens.pop();
        ast::Refrence * ref = new ast::Refrence();
        if (dynamic_cast<lex::LObj *>(tokens.peek()) != nullptr){
            lex::LObj obj = * dynamic_cast<lex::LObj *>(tokens.pop());
            ref->Ident = obj.meta;
            output = ref;
        }else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " No object given to refrece");
    }
    else if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){
        lex::OpSym eq = *dynamic_cast<lex::OpSym *>(tokens.peek());
        if(eq.Sym == '['){
            tokens.pop();
            ast::Lambda * lambda = new ast::Lambda();
            lambda->function = new ast::Function();
            lambda->function->args = this->parseArgs(tokens, ',', ']', lambda->function->argTypes);
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Need an > to start lambda not a symbol");
            if ((dynamic_cast<lex::OpSym *>(tokens.pop())->Sym != '=')) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " GOT: " + dynamic_cast<lex::OpSym *>(tokens.pop())->Sym + " Need an > to start lambda");

            if (dynamic_cast<lex::Symbol *>(tokens.peek()) == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Need an > to start lambda not a symbol");
            if ((dynamic_cast<lex::Symbol *>(tokens.pop())->meta != ">")) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " GOT: " + dynamic_cast<lex::OpSym *>(tokens.pop())->Sym + " Need an > to start lambda");
            if(dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr && dynamic_cast<lex::OpSym *>(tokens.peek())->Sym == '{'){
                tokens.pop();
                lambda->function->statment = this->parseStmt(tokens);
                ast::Type Adr = ast::Type();
                Adr.typeName = "any";
                Adr.opType = asmc::Hard; 
                Adr.size = asmc::QWord;
                lambda->function->type = Adr;
            } else this->parseStmt(tokens, true);
            output = lambda;
        } else if(eq.Sym == '('){
            tokens.pop();
            ast::ParenExpr * paren = new ast::ParenExpr();
            paren->expr = this->parseExpr(tokens);
            if (dynamic_cast<lex::OpSym *>(tokens.peek()) == nullptr) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Need an ) to end parenth not a symbol");
            if ((dynamic_cast<lex::OpSym *>(tokens.pop())->Sym != ')')) throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " GOT: " + dynamic_cast<lex::OpSym *>(tokens.pop())->Sym + " Need an ) to end parenth");
            output = paren;
        };
    }
    else throw err::Exception("Line: " + std::to_string(tokens.peek()->lineCount) + " Unknown Expr");

    if (dynamic_cast<lex::OpSym *>(tokens.peek()) != nullptr){

        ast::Compound  * compound = new ast::Compound();

        lex::OpSym sym = *dynamic_cast<lex::OpSym *>(tokens.peek());
        if (sym.Sym == '+')
        {
            tokens.pop();
            compound->op = ast::Plus;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '-')
        {
            tokens.pop();
            compound->op = ast::Minus;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '*')
        {
            tokens.pop();
            compound->op = ast::Mul;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '/')
        {
            tokens.pop();
            compound->op = ast::Div;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '%')
        {
            tokens.pop();
            compound->op = ast::Mod;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '&')
        {
            tokens.pop();
            compound->op = ast::AndBit;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '>'){
            tokens.pop();
            compound->op = ast::Great;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '<'){
            tokens.pop();
            compound->op = ast::Less;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
        else if (sym.Sym == '|'){
            tokens.pop();
            compound->op = ast::OrBit;
            compound->expr1 = output;
            compound->expr2 = this->parseExpr(tokens);
            return compound;
        }
    }
    return output;
}