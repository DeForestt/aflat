class ICollection{
    contract {
        adr forEach;
        adr where;
    };
};

class ListNode{
    adr next;
    adr data;
};

ListNode newNode();

class LinkedList signs ICollection{
    public ListNode head;
    
    adr init();
    int append(adr data);
    int insert(adr data, int index);
    int delete(int index);
    adr get(int index);
    adr stitch(adr New);
    LinkedList reverse();
    LinkedList sort(adr compare, adr args);
    int find(adr foo, adr args);
    adr apply(adr foo, adr args);
    adr map(adr foo, adr args);
    int size();
    int erace();
    private LinkedList copy<<=>>(adr list);
};

LinkedList newList();

class Array signs ICollection{
    public int typeSize;
    public adr head;
    int size;

    Array init(int typeSize, int size);
    public adr at(int index);
    public int delete(int index);
    public int extend(int size);
};

Array newArray(int typeSize, int size);