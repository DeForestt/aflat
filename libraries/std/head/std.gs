
struct Time {
    adr amount;
};

struct Times{
    adr tms_utime;
    adr tms_stime;
    adr tms_cutime;
    adr tms_cstime;
};

Times newTimes();

struct Block{
    int size; 
    int free;
    adr next;
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

