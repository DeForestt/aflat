class String{
    adr head;
    String init(adr value);
    String concat<<&>>(adr s);
    String stitch<<+>>(adr s);
    String copy<<=>>(adr s);
    String replace(char o, char n);
    String trim();
    char at(int i);

    String subString(int start, int end);

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