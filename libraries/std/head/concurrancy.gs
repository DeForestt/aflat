class Process{
    private int pid;
    private adr foo;

    Process init(adr foo);
    int start(* adr args);
    bool isRunning();
    int getPid();
};

Process newProcess(adr foo);

class Pipe {
    int in;
    int out;
    int write(adr buf, int len);
    int read(adr buf, int len);
    Pipe init();
};

Pipe newPipe();

int exit(int status);
int wait(Process p);