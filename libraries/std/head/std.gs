
struct Time {
    adr amount;
};

int free(adr ptr);
Time newTime(adr size);
adr malloc(int size);

//size 16 Block
struct Block{
    int size; 
    int free;
    adr next;
};


