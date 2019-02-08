#pragma once

#include "linkedlist.h"

template <class T>
class Queue
{
public:
	Queue()
	{
		m_itemCount = 0;
	}
	~Queue() = default;

	/***
	 *  @brief Pushes an item onto the back of the queue
	 * 
	 *  @param item Item to push onto the queue
	 */
	void push(T item)
	{
		m_list.pushBack(item);
		m_itemCount++;
	}

	/***
	 *  @brief Pops an item off the front of the queue
	 * 
	 *  @return The value of the popped item
	 */
	T pop()
	{
		if (m_itemCount > 0)
			m_itemCount--;
		return m_list.popFront();
	}

	// getting the front/back items of the queue
	T front() { return m_list.first(); }
	T back() { return m_list.last(); }

	/***
	 *  @brief Gets how many items are in the queue
	 * 
	 *  @return Number of items in the queue
	 */
	int getCount() { return m_itemCount; }
	/***
	 *  @brief Checks whether or not the queue is empty
	 * 
	 *  @return Whether or not the queue is empty
	 */
	bool isEmpty() { return m_itemCount <= 0; }

private:
	LinkedList<T> m_list;
	int m_itemCount;
};