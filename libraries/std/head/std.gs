
class Time {
    public adr amount;
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
Time newTime(adr size);
adr malloc(int size);
adr realloc(adr ptr, int size);
int inspectHeap();

