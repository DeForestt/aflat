#include "Scanner.hpp"
#include "Exceptions.hpp"

#include <ctype.h>

LinkedList<lex::Token*> lex::Lexer::Scan(string input){
                LinkedList<lex::Token*> tokens = LinkedList<lex::Token*>();
                int i = 0;
                int lineCount = 1;
                while (i < input.length())
                {
                    if(input[i] == '\n') lineCount++;
                    else if (std::isalpha(input[i]) || input[i] == '_')
                    {
                       lex::LObj * l_obj = new LObj();
                        l_obj->meta = "";
                        while(std::isalpha(input[i]) || std::isdigit(input[i]) || input[i] == '_' || input[i] == '-'){
                            l_obj->meta += input[i];
                            i++;
                        }
                        l_obj->lineCount = lineCount;
                        tokens.push(l_obj);
                    }
                    else if (std::isdigit(input[i]) || input[i] == '-')
                    {
                        if (input[i] == '-' && !std::isdigit(input[i + 1])){
                            lex::OpSym * sym = new lex::OpSym;
                            sym->Sym = '-';
                            i++;
                            sym->lineCount = lineCount;
                            tokens << sym;
                        }else{
                            lex::INT * IntLit = new lex::INT();
                            IntLit->value = input[i];
                            i++;

                            while(std::isdigit(input[i]) || input[i] == '.'){
                                IntLit->value += input[i];
                                i++;
                            }
                            IntLit->lineCount = lineCount;

                            //Check if IntLit->value contains '.'
                            if(IntLit->value.find('.') != string::npos){
                                //Check if IntLit->value contains more than one '.'
                                if(IntLit->value.find('.') != IntLit->value.rfind('.')){
                                    throw err::Exception("Invalid token: " + IntLit->value + " on line " + std::to_string(lineCount));
                                }

                                lex::FloatLit * FloatLit = new lex::FloatLit();
                                FloatLit->value = IntLit->value;
                                FloatLit->lineCount = lineCount;
                                tokens << FloatLit;
                            }else tokens <<IntLit;
                        }

                    }
                    else if (input[i] == '#')
                    {
                        
                            lex::Long * IntLit = new lex::Long();
                            i++;

                            while(std::isdigit(input[i])){
                                IntLit->value += input[i];
                                i++;
                            }
                            IntLit->lineCount = lineCount;
                            tokens.push(IntLit);

                    }
                    else if(std::isspace(input[i]))
                    {
                        i++;
                    }
                    else if(input[i] == '\"'){
                        lex::StringObj* stringObj = new StringObj();
                        stringObj->value = "";
                        i++;
                        while(input[i] != '\"'){
                            if (input[i] == '\n') {lineCount++; i++;}
                            if (input[i] == '\\'){
                                i++;
                                if (input[i] == 'n'){
                                    stringObj->value += "\\n";
                                }else if (input[i] == 't'){
                                    stringObj->value += "\\t";
                                }else if (input[i] == '\\'){
                                    stringObj->value += '\\';
                                }else if (input[i] == '\"'){
                                    stringObj->value += '\"';
                                }else if (input[i] == '\''){
                                    stringObj->value += '\'';
                                }else if (input[i] == 'r'){
                                    stringObj->value += "\\r";
                                }else if (input[i] == '0'){
                                    stringObj->value += '\0';
                                }else if (input[i] == '\\'){
                                    stringObj->value += '\\';
                                }else{
                                    throw err::Exception("Invalid token: " + stringObj->value + " on line " + std::to_string(lineCount));
                                }
                            }else if (input[i] != '\n'){
                                stringObj->value += input[i];
                            }
                            i++;
                        }
                        i++;
                        tokens.push(stringObj);
                    }
                    else if (input[i] == '\'')
                    {
                        lex::CharObj* charobj = new CharObj();
                        i++;
                        charobj->value = input[i];
                        i++;
                        if (input[i] != '\'') throw err::Exception("Unterminated Char Value");
                        tokens << charobj;
                        i++;
                    }
                    else if(input[i] == ';'){
                        lex::OpSym* semi = new OpSym;
                        semi->Sym = input[i];
                        tokens.push(semi);
                        i++;
                    }
                    else if (input[i] == '?')
                    {
                        lex::Ref* Ref = new lex::Ref;
                        tokens << Ref;
                        i++;
                    }
                    else if(input[i] == '('){
                        lex::OpSym* cPrenth = new OpSym;
                        cPrenth->Sym = input[i];
                        tokens.push(cPrenth);
                        i++;
                    }
                    else if(input[i] == ')'){
                        lex::OpSym* cPrenth = new OpSym;
                        cPrenth->Sym = input[i];
                        tokens.push(cPrenth);
                        i++;
                    }
                    else if(input[i] == '{'){
                        lex::OpSym* cPrenth = new OpSym;
                        cPrenth->Sym = input[i];
                        tokens.push(cPrenth);
                        i++;
                    }
                    else if(input[i] == '}'){
                        lex::OpSym* cPrenth = new OpSym;
                        cPrenth->Sym = input[i];
                        tokens.push(cPrenth);
                        i++;
                    }
                    else if(input[i] == '='){
                        if (input[i+1] == '='){
                            lex::Symbol * equ = new lex::Symbol;
                            equ->meta = "==";
                            i++;
                            tokens << equ;
                        }
                        else{
                            lex::OpSym* equ = new OpSym;
                            equ->Sym = input[i];
                            tokens << equ;
                        }
                        i++;
                    }
                    else if (input[i] == '<')
                    {
                        lex::Symbol * sym = new lex::Symbol;
                        sym->meta = "<";
                        tokens << sym;
                        i++;
                    }
                    else if (input[i] == '>')
                    {
                        lex::Symbol * sym = new lex::Symbol;
                        sym->meta = ">";
                        tokens << sym;
                        i++;
                    }
                    else if(input[i] == '!'){
                        if (input[i+1] == '='){
                            lex::Symbol * equ = new lex::Symbol;
                            equ->meta = "!=";
                            i++;
                            tokens << equ;
                        }
                        else{
                            lex::OpSym* equ = new OpSym;
                            equ->Sym = input[i];
                            tokens << equ;
                        }
                        i++;
                    }else if(input[i] == ','){
                        lex::OpSym* com = new OpSym;
                        com->Sym = input[i];
                        tokens.push(com);
                        i++;
                    }else if(input[i] == '+'){
                        lex::OpSym* add = new OpSym;
                        add->Sym = input[i];
                        tokens.push(add);
                        i++;
                    }else if (input[i] == '*')
                    {
                        lex::OpSym* mul = new OpSym;
                        mul->Sym = input[i];
                        tokens << mul;
                        i++;
                    }else if (input[i] == '%'){
                        lex::OpSym* mul = new OpSym;
                        mul->Sym = input[i];
                        tokens << mul;
                        i++;
                    }else if (input[i] == ':')
                    {
                        lex::OpSym* col = new OpSym;
                        col->Sym = input[i];
                        tokens << col;
                        i++;
                    }else if (input[i] == '@')
                    {
                        lex::OpSym* at = new OpSym;
                        at->Sym = input[i];
                        tokens << at;
                        i++;
                    }else if (input[i] == '.')
                    {
                        lex::OpSym* mul = new OpSym;
                        mul->Sym = input[i];
                        tokens << mul;
                        i++;
                    }
                    else if (input[i] == '/')
                    {
                        lex::OpSym* div = new OpSym;
                        div->Sym = input[i];
                        tokens << div;
                        i++;
                    }
                    
                    else{
                        ///Unknown token
                        throw err::Exception("Unknown token on line " + std::to_string(lineCount));
                    }
                }
                return tokens;
            }