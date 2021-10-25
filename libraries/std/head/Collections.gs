struct DoubleNode{
    adr prev;
    adr next;
    adr data;
};

DoubleNode newDoubleNode();

class ListNode{
    adr next;
    adr data;
};

ListNode newNode();

class Stack{
    ListNode head;
    int count;
    
    int push(adr data);
    adr pop();
    adr peek();
};

Stack newStack();

class List{
    DoubleNode head;
    DoubleNode tail;

    int append(adr data);
    adr at(int index);
    adr rm(int index);
    int insert(int index, adr data);
    int len();
};

List newList();