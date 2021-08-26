#ifndef STRUCT
#define STRUCT

#include <string>
#include "LinkedList.hpp"

namespace AST{
    class ProgramMember{
        public:
        virtual std::string toString(){};
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
        virtual std::string toString(){};
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
        Char,
        Byte
    };

    class Arg{
        public:
        virtual std::string toString(){};
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



    class Function : public Member, public Statment{
        public:
        Type type;
        Ident ident;
        Statment * args;
        Statment * statment;
        std::string toString();
    };


    class Declare : public Arg, public Statment{
        public:
        std::string toString();
        std::string Ident;
        Type type;
    };

    class Argument : public Arg, public Statment{
        public:
        std::string Ident;
        Type type;
    };



    class Expr{
        public:
        virtual std::string toString(){};
    };

    class ParenExpr : public Expr{
        public:
        Expr * expr;
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
        Declare * declare;
        Expr * expr;
    };

    class Assign : public Statment{
        public:
        std::string Ident;
        Expr * expr;
    };

    class While : public Statment{
        public:
        ParenExpr parenExpr;
    };

    class For : public Statment{
        public:
        Declare declare;
        Expr * expr;
        Statment ReRun;
        Statment Run;
    };

    class Call : public Statment{
        public:
        std::string ident;
        links::LinkedList<Expr *> Args;
    };

    class Sequence : public Statment{
        public:
        Statment * Statment1;
        Statment * Statment2;
        std::string toString();
    };

    class Iflush : public Statment{
    };

    class CWrite : public Statment{
        public:
        Expr * expr;
        std::string toString();
    };

    class ORead : public Statment{
    };
    
    class Return : public Statment{
        public:
        Expr * expr;
        std::string toString();
    };

    class StringLiteral : public Expr{
        public:
        std::string val;
    };

    class ByteLiteral : public Expr{
        public:
        std::byte val;
    };

    class IntLiteral : public Expr{
        public:
        int val;
    };

    class Compound : public Expr{
        public:
        Expr * expr1;
        Op op;
        Expr * expr2;
    };

}

#endif