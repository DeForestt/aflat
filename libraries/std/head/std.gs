
class Time {
    public adr amount;
};

class TimeSpec{
    long tv_sec;
    long tv_nsec;
};

class Times{
    public int tms_utime;
    public int tms_stime;
    public int tms_cutime;
    public int tms_cstime;
};

Times newTimes();

class Block{
    public int size; 
    public int free;
    public adr next;
};

class Bit{
	int value;
	adr last;
	adr next;
};

Bit newBit(int value);

int free(adr ptr);
int blockSize(adr ptr);
Time newTime(adr size);
adr malloc(int size);
adr realloc(adr ptr, int size);
int inspectHeap();
int memcpy(adr dst,adr src, int size);
int panic(adr msg);
int assert(bool cond, * adr message);
int sleep(int seconds, * int nanoseconds);


// The following functions are not part of the standard library, but are not type checked

const let pick = [bool cond, adr a, adr b] =>
    if cond return a else return b;