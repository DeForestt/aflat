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
        Member members;
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
        public:
        Statment Statment1;
        Statment Statment2;
    };

    class Iflush : public Statment{
    };

    class OFlush : public Statment{
    };

    class oread : public Statment{
    };
    
    class Return : public Statment{
        public:
        Expr expr;
    };

    class Expr{
        public:
        virtual std::string toString();
    };

    class ParenExpr : public Expr{
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

    enum Op{
        Plus,
        Minus,
        Mul,
        Div,
        Equ,
        NotEqu
    };

}

#endif