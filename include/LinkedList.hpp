/* This linked list class will be nessisary for carrying and
manipulating the strings being built.*/
#ifndef LINK
#define LINK

namespace links{
    
    /*The Node is a genaric class used to hold values
    in the linked list.  It is generic in case I want to
    use it for somthing else in the futer.*/
    
    template<typename T>
    class Node{
        public:
            T data;
            Node * next;
    };

    template<typename T>
    class LinkedList{
        public:
            Node<T> * head;
            LinkedList();

            /*Push a new value to the top of the list*/
            void push(T value);

            /*Reverses the direction of the list*/
            void invert();

            /*Sattatches the head of another linked list to the tail of this one*/
            void stitch(links::LinkedList<T> l);

            /*Pop removes the top eleent for the list
            Behaves likle a stack*/ 
            T pop();

            /*Observes the first node on the list and returns its value withou
            popping it.*/
            T peek();
    };
}

template <typename T>
links::LinkedList<T>::LinkedList(){
    head = nullptr;
}

template <typename T>
void links::LinkedList<T>::push(T value){
    Node<T>* push = new Node<T>();
    push->next = this->head;
    push->data = value;
    this->head = push;
}

template <typename T>
void links::LinkedList<T>::invert(){
    Node<T>* prev = nullptr;
    Node<T>* curr = this->head;
    Node<T>* next = nullptr;

    while (curr!= nullptr){
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    this->head = prev;
}

template <typename T>
T links::LinkedList<T>::pop(){
    T data = this->head->data;
    Node<T>* poper = this->head;
    this->head = this->head->next;
    delete poper;
    return data;
}

template<typename T>
T links::LinkedList<T>::peek(){
    return this->head->data;
}

template<typename T>
void links::LinkedList<T>::stitch(LinkedList<T> l){
    links::Node<T> * pointer = head;
    while (pointer->next != nullptr){
        pointer = pointer->next;
    }
    pointer->next = l.head;
}

#endif