#ifndef LEX
#define LEX

#include <string>
#include "LinkedList.hpp"


using std::string;
using links::LinkedList;

namespace lex{

    class Token{
        public:
        virtual ~Token() = default;
    };

    class Symbol: public Token{
        public:
        string meta;
    };

    class LObj: public Token{
        public:
        string meta;
    };

    class Ref: public Token{};

    class StringObj: public Token{
        public:
        string value;
    };

    class CharObj: public Token{
        public:
        char value;
    };

    class OpSym: public Token{
        public:
        char Sym;
    };

    class INT: public Token{
        public:
        string value;
    };

    class HEX: public Token{
        public:
        string value;
    };

    class FloatLit: public Token{
        public:
        string value;
    };

    /*This will take apart the string and breake it down into "Words"
    that the parser will be able to understand*/
    class Lexer{
        public:
            LinkedList<Token*> Scan(string input);
    };


};
#endif