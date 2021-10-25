
struct Time {
    adr amount;
};

//size 16 Block
struct Block{
    int size; 
    int free;
    adr next;
};

int free(adr ptr);

Time newTime(adr size);
adr malloc(int size);