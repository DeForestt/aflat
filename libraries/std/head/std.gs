
struct Time {
    adr amount;
};

//size 16 Block
struct Block{
    int size; 
    int free;
    adr next;
};

free();

Time newTime(adr size);
adr malloc(int size);
int init();