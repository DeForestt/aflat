struct ListNode{
    adr next;
    adr data;
};

ListNode newNode();


class LinkedList{
    ListNode head;
    int count;
    
    int append(adr data);
    int insert(adr data, int index);
    int delete(int index);
    adr get(int index);
    int size();
};

LinkedList newList();
