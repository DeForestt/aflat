
struct Time {
    adr amount;
};

int free(adr ptr);
int memcopy (adr ptr, adr ptr1, int size);
Time newTime(adr size);
adr malloc(int size);
adr realloc(adr ptr, int size);

//size 16 Block
struct Block{
    int size; 
    int free;
    adr next;
};


