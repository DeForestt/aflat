
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

int af_free(object ptr);
int blockSize(object ptr);
Time newTime(object size);
adr af_malloc(int size);
adr af_realloc(object ptr, int size);
int inspectHeap();
int af_memcpy(object dst, immutable object src, int size);
int panic(object msg);
int assert(bool cond, * object message);
int sleep(int seconds, * int nanoseconds);
adr _fCstr(object fmt, * object args);
adr _fUCstr(object fmt, * object args);
adr mmap(object addr, int len, int prot, int flags, int fd, int offset);
adr create(const int size, ? const any value);
types(T)
fn dupe(const T value) -> T {
    return $value;
};
void _fEachOr(const object iterator, const adr callback, *const any args);
adr _toOption(const any value);
