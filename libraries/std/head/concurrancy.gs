class Process{
    private int pid;
    private adr foo;

    int init(adr foo);
    int start(adr args);
    int isRunning();
    int getPid();
};

Process newProcess(adr foo);

class Pipe {
    int in;
    int out;
    int write(adr buf, int len);
    int read(adr buf, int len);
    int init();
};

Pipe newPipe();

int exit(int status);
int wait(Process p);