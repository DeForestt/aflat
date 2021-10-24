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