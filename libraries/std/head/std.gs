
struct Time {
    adr amount;
};

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


