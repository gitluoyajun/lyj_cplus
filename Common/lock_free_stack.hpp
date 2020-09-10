#pragma once
#include <atomic>
#include <memory>

template<typename T>
class lock_free_stack
{
private:
	struct node
	{
		std::shared_ptr<T> data;
		node* next;
		node(const T& data_) : data(std::make_shared<T>(data_)) {}
	};

	std::atomic<node*> head;
public:
	void push(T const& data_)
	{
		node* const new_node = new node(data_);
		new_node->next = head.load();
		while (!head.compare_exchange_weak(new_node->next, new_node));
	}
	
	std::shared_ptr<T> pop()
	{
		node* old_head = head.load();
		while (old_head && !head.compare_exchange_weak(old_head, old_head->next))
		{
			return old_head ? old_head->data : std::shared_ptr<T>();
		}
	}
};