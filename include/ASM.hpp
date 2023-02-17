#if !defined(ASM)
#define ASM

#include <string>

#include "LinkedList.hpp"

namespace asmc {

enum OpType {
  Float,
  Hard,
};
enum Size { Byte, Word, DWord, QWord, AUTO };

class Instruction {
 public:
  virtual std::string toString();
  int logicalLine = 0;
};

class Label : public Instruction {
 public:
  std::string label;
  std::string toString();
};

class LinkTask : public Instruction {
 public:
  std::string command;
  std::string operand;
  std::string toString();
};

class Mov : public Instruction {
 public:
  std::string to;
  std::string from;
  Size size;
  OpType op = Hard;
  std::string toString();
};

class nop : public Instruction {
 public:
  std::string toString() { return ""; };
};

class Movzbl : public Instruction {
 public:
  std::string to;
  std::string from;
  std::string toString();
};

class Cmp : public Instruction {
 public:
  std::string to;
  std::string from;
  Size size;
  OpType op = Hard;
  std::string toString();
};

class Jmp : public Instruction {
 public:
  std::string to;
  std::string toString();
};

class Jne : public Instruction {
 public:
  std::string to;
  std::string toString();
};

class Je : public Instruction {
 public:
  std::string to;
  std::string toString();
};

class Jl : public Instruction {
 public:
  std::string to;
  std::string toString();
};

class Jle : public Instruction {
 public:
  std::string to;
  std::string toString();
};

class Jg : public Instruction {
 public:
  std::string to;
  std::string toString();
};

class Jge : public Instruction {
 public:
  std::string to;
  std::string toString();
};

class StringLiteral : public Instruction {
 public:
  std::string value;
  std::string toString();
};

class FloatLiteral : public Instruction {
 public:
  std::string value;
  std::string toString();
};

class Lea : public Instruction {
 public:
  std::string to;
  std::string from;
  std::string toString();
};

class Movl : public Instruction {
 public:
  std::string to;
  std::string from;
  std::string toString();
};

class Movq : public Instruction {
 public:
  std::string to;
  std::string from;
  std::string toString();
};

class Movdqu : public Instruction {
 public:
  std::string to;
  std::string from;
  std::string toString();
};

class Call : public Instruction {
 public:
  std::string function;
  std::string toString();
};

class Xor : public Instruction {
 public:
  std::string op1;
  std::string op2;
  std::string toString();
};

class Sete : public Instruction {
 public:
  std::string op;
  std::string toString();
};

class Setne : public Instruction {
 public:
  std::string op;
  std::string toString();
};

class Setl : public Instruction {
 public:
  std::string op;
  std::string toString();
};

class Setle : public Instruction {
 public:
  std::string op;
  std::string toString();
};

class Setg : public Instruction {
 public:
  std::string op;
  std::string toString();
};

class Setge : public Instruction {
 public:
  std::string op;
  std::string toString();
};

class Push : public Instruction {
 public:
  std::string op;
  asmc::Size size = asmc::QWord;
  std::string toString();
};

class Pop : public Instruction {
 public:
  std::string op;
  asmc::Size size = asmc::QWord;
  std::string toString();
};

class SysCall : public Instruction {
 public:
  std::string toString();
};

class Return : public Instruction {
 public:
  std::string toString();
};

class Define : public Instruction {
  public:
  std::string name;
  std::string value;
  int type;
  std::string toString();
};

class ArithInst : public Instruction {
 public:
  std::string op1;
  std::string op2;
  asmc::Size size;
  asmc::OpType opType = asmc::Hard;
};


class Add : public ArithInst {
 public:
  std::string toString();
};

class Sub : public ArithInst {
 public:
  std::string toString();
};

class Mul : public ArithInst {
 public:
  std::string toString();
};

class Div : public ArithInst {
 public:
  std::string toString();
};

class Subq : public Instruction {
 public:
  std::string op1;
  std::string op2;
  std::string toString();
};

class And : public ArithInst {
 public:
  std::string toString();
};

class Or : public ArithInst {
 public:
  std::string toString();
};

class Sal : public ArithInst {
 public:
  std::string toString();
};

class Sar : public ArithInst {
 public:
  std::string toString();
};

class File {
 public:
  links::LinkedList<Instruction*> text = links::LinkedList<Instruction*>();
  links::LinkedList<Instruction*> bss = links::LinkedList<Instruction*>();
  links::LinkedList<Instruction*> data = links::LinkedList<Instruction*>();
  links::LinkedList<Instruction*> linker = links::LinkedList<Instruction*>();
  bool hasLambda = false;
  File* lambdas;
  void operator<<(asmc::File file);
  void operator>>(asmc::File file);
  void cstitch(asmc::File file);
  void collect();
  File();
};

class Register {
 private:
  std::string qWord;
  std::string dWord;
  std::string word;
  std::string byte;

 public:
  Register(std::string _qWord, std::string _dWord, std::string _word,
           std::string _byte);
  Register() {}
  bool inuse = false;
  // a linked list of int process id's that are using this register
  links::LinkedList<int> users;
  std::string get(asmc::Size wants);
  static bool compare(Register R, std::string input) {
    if (input == R.qWord || input == R.dWord || input == R.word ||
        input == R.byte) {
      return true;
    }
    return false;
  }
};

}  // namespace asmc
#endif  // ASM
