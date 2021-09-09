#if !defined(ASM)
#define ASM

#include <string>
#include "LinkedList.hpp"

namespace ASMC{

    enum Size{
        Byte,
        Word,
        DWord,
        QWord,
        AUTO
    };

    class Instruction{
        public:
        virtual std::string toString();
    };

    class Lable : public Instruction{
        public:
        std::string lable;
        std::string toString();
    };

    class LinkTask : public Instruction{
        public:
        std::string command;
        std::string operand;
        std::string toString();
    };

    class Mov : public Instruction{
        public:
        std::string to;
        std::string from;
        Size size;
        std::string toString();
    };

    class Lea : public Instruction{
        public:
        std::string to;
        std::string from;
        std::string toString();
    };

    class Movl : public Instruction{
        public:
        std::string to;
        std::string from;
        std::string toString();
    };

    class Movq : public Instruction{
        public:
        std::string to;
        std::string from;
        std::string toString();
    };

    class Call : public Instruction{
        public:
        std::string function;
        std::string toString();
    };

    class XOR : public Instruction{
        public:
        std::string op1;
        std::string op2;
        virtual std::string toString();
    };

    class Push : public Instruction{
        public:
        std::string op;
        std::string toString();
    };

    class Pop : public Instruction{
        public:
        std::string op;
        std::string toString();
    };

    class SysCall : public Instruction{
        public:
        std::string toString();
    };

    class Return : public Instruction{
        public:
        std::string toString();
    };

    class Add : public Instruction{
        public:
        std::string op1;
        std::string op2;
        std::string toString();
    };

    class File{
        public:
        links::LinkedList<Instruction * > text;
        links::LinkedList<Instruction * > data;
        links::LinkedList<Instruction * > linker;
        void operator<<(ASMC::File file);
    };

    class Register{
        public:
        std::string qWord;
        std::string dWord;
        std::string word;
        std::string byte;
        Register(std::string _qWord, std::string _dWord, std::string _word, std::string _byte);
        Register(){}
        static bool compair(Register R, std::string input){
            if (input == R.qWord || input == R.dWord || input == R.word || input == R.byte ){
                return true;
            }
            return false;
        }
    };

}
#endif // ASM

