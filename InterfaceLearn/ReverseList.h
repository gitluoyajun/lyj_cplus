#pragma once
#include <iostream>
#include <stack>

template <class T>
struct Node
{
	T data;
	Node* next;
};

//从头开始反转
template <class T>
Node<T>* ReverseListPre(Node<T>* head)
{
	if (!head || !head->next)
	{
		return head;
	}
	Node<T>* cur = head;
	Node<T>* pre = nullptr;
	Node<T>* next = head->next;
	cur->next = pre;
	while (cur && next)
	{
		pre = cur;
		cur = next;
		next = next->next;
		cur->next = pre;
	}
	return cur;
}


//从尾开始反转,使用栈
template<typename T>
Node<T>* ReverseListByStack(Node <T>* head)
{
	std::stack<Node <T>*> stack;
	while (head && head->next)
	{
		stack.push(head);
		head = head->next;
	}
	Node<T>* newHead = head;
	Node<T>* p = head;
	while (!stack.empty())
	{
		auto next = stack.top();
		stack.pop();
		p->next = next;
		p = next;
	}
	p->next = nullptr;
	return newHead;
}
//从尾开始反转,使用函数调用栈
template<typename T>
Node<T>* ReverseListLast(Node <T>* head)
{
	if (!head || !head->next)
	{
		return head;
	}
	Node<T>* tail = ReverseListLast(head->next);
	head->next->next = head;
	head->next = nullptr;
	return tail;
}

template <class T>
void PrinfList(Node<T>* head)
{
	Node<T>* next = head;
	while (next)
	{
		std::cout << next->data << " -> ";
		next = next->next;
	}
	std::cout << std::endl;
}

