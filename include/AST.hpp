#ifndef STRUCT
#define STRUCT

#include <string>

namespace AST{

    class Program{
        public:
        ProgramMember members;
    };

    class ProgramMember{
        public:
        virtual std::string toString();
    };

    class ClassList : public ProgramMember{
        Class class1;
        Class Class2;
    };

    class Member{
        public:
        virtual std::string toString();
    };

    class Class : public ProgramMember{
        public:
        std::string Ident;
        Member members
    };


    class Function : public Member{
        public:
        Type type;
        Ident ident;
        Arg arg;
        Statment statment;
    };

    class Arg{
        public:
        virtual std::string toString();
    };

    class Declare : public Arg, public Statment{
        public:
        std::string toString();
    };

    enum Type{
        Int,
        Float,
        String,
        Char
    };

    class Ident{
        public:
        std::string ident;
    };

    class Statment{
        public:
        virtual std::string toString();
    };

    class If : public Statment{
        public:
        ParenExpr parenExpr;
        Statment statment;
    };

    class ElIf : public Statment{
        public:
        ParenExpr parenExpr;
        Statment tStatment;
        Statment fStatment;
    };

    class DecAssign : public Statment{
        public:
        Declare declare;
        Expr expr;
    };

    class Assign : public Statment{
        public:
        std::string Ident;
        Expr expr;
    };

    class While : public Statment{
        public:
        ParenExpr parenExpr;
    };

    class For : public Statment{
        public:
        Declare declare;
        Expr expr;
        Statment ReRun;
        Statment Run;
    };

    class Sequence : public Statment{
        
    }
}
#endif