class String{
    adr head;
    adr init(adr value);
    adr concat<<&>>(adr s);
    adr stitch<<+>>(adr s);
    int copy<<=>>(adr s);
    char at(int i);

    adr subString(int start, int end);

    // compare is for compairing to another string object
    int compare(adr s);
    // equal is for compairing to a string literal or a char array
    int equal(adr s);

    int find(char c);
    int len();

    int delete();
};

String printString(String s); 
String getString();