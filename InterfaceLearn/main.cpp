#include <cstdio>
#include "ReverseList.h"
const int LISTSIZE = 5;
int main()
{
    Node<int>* head = nullptr;
    Node<int>* p = head;
 
    for (int i = 0 ; i < LISTSIZE; i++)
    {
        Node<int>* new_node = new Node<int>();
        new_node->data = i;
        new_node->next = nullptr;
        if (i == 0)
        {
            head = new_node;
            p = new_node;
        }
        else
        {
            p->next = new_node;
            p = new_node;
        }
    }
    auto tail = ReverseListByStack(head);
    PrinfList(tail);
  
    printf("hello from InterfaceLearn!\n");
    return 0;
}