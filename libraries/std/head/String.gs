.needs <Collections>
class String{
    private adr head;
    String init(adr value);
    String concat<<&>>(adr s);
    String stitch<<+>>(adr s);
    String copy<<=>>(adr s);
    String replace(char o, char n);
    LinkedList split(char c);
    String toLower();
    String toUpper();
    String trim(char ch);
    char at(int i);
    adr getPrimitive();

    String subString(int start, int end);

    // compare is for compairing to another string object
    bool compare(adr s);
    // equal is for compairing to a string literal or a char array
    bool equal(adr s);

    int find(char c);
    int toInt();
    int len();

    int delete();
};

String printString(String s); 
String getString();