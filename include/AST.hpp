#ifndef STRUCT
#define STRUCT

#include <string>
#include "ASM.hpp"
#include "LinkedList.hpp"

namespace AST{
    class ProgramMember{
        public:
        virtual std::string toString(){return "";};
    };

    class Ident{
        public:
        std::string ident;
    };


    class Statment{
        public:
        virtual std::string toString(){return "";};
    };

    class Member{
        public:
        virtual std::string toString(){return "";};
    };

    enum Op{
        Plus,
        Minus,
        Mod,
        Mul,
        Div,
        Equ,
        NotEqu,
        Less,
        Great,
    };

    class Expr{
        public:
        virtual std::string toString(){return "";};
    };

    class ConditionalExpr{
        public:
        Expr * expr1;
        Op op;
        Expr * expr2;
    };
    
    class Type{
        public:
        std::string typeName;

        ASMC::Size size;
        static bool compair(Type t, std::string name);
    };

    class Arg{
        public:
        virtual std::string toString(){return "";};
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

    class UDeffType : public Member, public Statment{
        public:
        Ident ident;
        Statment * statment;
    };

    class Declare : public Arg, public Statment{
        public:
        std::string toString();
        std::string Ident;
        std::string TypeName;
        Type type;
    };

    class Argument : public Arg, public Statment{
        public:
        std::string Ident;
        Type type;
    };

    class ParenExpr : public Expr{
        public:
        Expr * expr;
    };

    class CharLiteral : public Expr{
        public:
        char value;
    };


    class If : public Statment{
        public:
        ConditionalExpr * Condition;
        Statment * statment;
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
        bool refrence = false;
        Expr * expr;
        links::LinkedList<std::string> modList;
    };

    class While : public Statment{
        public:
        ConditionalExpr * condition;
        Statment  * stmt;
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

    class Push : public Statment{
        public:
        Expr * expr;
    };

    class Pull : public Statment{
        public:
        Expr * expr;
    };
    
    class DecArr : public Statment{
        public:
        std::string ident;
        AST::Type type;
        int count;
    };

    class Return : public Statment{
        public:
        Expr * expr;
        std::string toString();
    };

    class Var : public Expr{
        public:
        std::string Ident;
        links::LinkedList<std::string> modList;
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
    
    class Refrence : public Expr{
        public:
        std::string Ident;
    };

    class DeRefence : public Expr{
        public:
        std::string Ident;
        links::LinkedList<std::string> modList;
        AST::Type type;
    };

    class CallExpr : public Expr{
        public:
        AST::Call * call;
    };

}

#endif