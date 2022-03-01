class ICollection{
    contract {
        adr forEach;
        adr where;
        adr size;
    };
};

class ListNode{
    adr next;
    adr data;
};

ListNode newNode();

class LinkedList{
    private ListNode head;
    
    int init();
    int append(adr data);
    int insert(adr data, int index);
    int delete(int index);
    adr get(int index);
    adr stitch(adr new);
    public int forEach(adr foo, args);
    int size();
};

LinkedList newList();

class Array{
    private int typeSize;
    private adr head;
    int size;

    public int init(int typeSize, int size);
    public adr at(int index);
    public int delete(int index);
    public int extend(int size);
    public int forEach(adr foo, args);
};

Array newArray(int typeSize, int size);