#if !defined(ASM)
#define ASM

#include <string>

namespace ASMC{

    class Instruction{
        public:
        virtual std::string toString();
    };

    class Lable : public Instruction{
        public:
        std::string lable;
        std::string toString();
    };

    class SectionHeader : public Instruction{
        public:
        std::string section;
        std::string toString();
    };

    class ExIn : public Instruction{
        public:
        std::string mnumonic;
        std::string operand1;
        std::string operand2;
        std::string toString();
    };

}
#endif // ASM

