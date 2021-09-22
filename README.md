AFLAT compiles to GNU ASM <br>
Please see example project
**Heap Access Coming Soon**

    Build:
        $git pull
        $make

    Data Types:
        Int: DWord signed integer eg: int i = 1;
        Char: Byte character eg: char a = 'A';
        Adr: a pointer eg: adr pointer = ?a;


    Oporators:
        +: add
        -: subtract
        *: multiply
        /: devide **Curently cannot devide with constants. Need to load int into a variable first**
        ?: refrence eg: adr pointer = ?veriable
        as: de-refrense eg char i = pointer as char

        boolean:
            ==: equals
            !=: not equals
            >: greater than
            <: less than

    statments:
        function declare: type ident(args){statment;};
        if: if(expr bool expr){statment;};
        while: if(expr bool expr){statment;};
        push: push pointer; (pushes a single char to the output buffer)
        pull: pull pointer; (reads a value from the input buffer);
        return: return value;

    to compile:
        $~/aflat/bin/main src.af output.s
        $gcc -no-pie output.s
