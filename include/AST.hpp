#ifndef STRUCT
#define STRUCT

#include <string>

namespace AST{
    class ProgramMember{
        public:
        virtual std::string toString();
    };

    class Ident{
        public:
        std::string ident;
    };


    class Statment{
        public:
        virtual std::string toString();
    };

    class Member{
        public:
        virtual std::string toString();
    };

    enum Op{
        Plus,
        Minus,
        Mul,
        Div,
        Equ,
        NotEqu
    };
    
    enum Type{
        Int,
        Float,
        String,
        Char
    };

    class Arg{
        public:
        virtual std::string toString();
    };


    class Program{
        public:
        ProgramMember members;
    };

    class Class : public ProgramMember{
        public:
        std::string Ident;
        Member members;
    };

    class ClassList : public ProgramMember{
        Class class1;
        Class Class2;
    };



    class Function : public Member{
        public:
        Type type;
        Ident ident;
        Arg arg;
        Statment statment;
    };


    class Declare : public Arg, public Statment{
        public:
        std::string toString();
    };



    class Expr{
        public:
        virtual std::string toString();
    };

    class ParenExpr : public Expr{
        public:
        Expr expr;
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
        public:
        Statment * Statment1;
        Statment * Statment2;
    };

    class Iflush : public Statment{
    };

    class OFlush : public Statment{
    };

    class ORead : public Statment{
    };
    
    class Return : public Statment{
        public:
        Expr expr;
    };

    class StringLiteral : public Expr{
        public:
        std::string val;
    };

    class IntLiteral : public Expr{
        public:
        int val;
    };

    class Compound : public Expr{
        public:
        Expr expr1;
        Op op;
        Expr expr2;
    };

}

#endif