#include "Scanner.hpp"
#include <ctype.h>

LinkedList<lex::Token*> lex::Lexer::Scan(string input){
                LinkedList<lex::Token*> tokens = LinkedList<lex::Token*>();
                int i = 0;
                while (i < input.length())
                {
                    if (std::isalpha(input[i]) || input[i] == '_')
                    {
                       lex::LObj * l_obj = new LObj();
                        l_obj->meta = "";
                        while(std::isalpha(input[i]) || std::isdigit(input[i]) || input[i] == '_' || input[i] == '-'){
                            l_obj->meta += input[i];
                            i++;
                        }
                        tokens.push(l_obj);
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
                            stringObj->value += input[i];
                            i++;
                        }
                        i++;
                        tokens.push(stringObj);
                    }
                    else if(input[i] == ';'){
                        lex::OpSym* semi = new OpSym;
                        semi->Sym = input[i];
                        tokens.push(semi);
                        i++;
                    }
                    else if(input[i] == '('){
                        lex::OpSym* oPrenth = new OpSym;
                        oPrenth->Sym = input[i];
                        tokens.push(oPrenth);
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
                    else{
                        throw i;
                    }
                }
                return tokens;
            }