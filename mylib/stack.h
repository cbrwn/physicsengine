#pragma once

#include "darray.h"

template <class T>
class Stack
{
public:
	Stack()
	{
	}

	~Stack()
	{
	}

	/***
	 *  @brief Push a value onto the top of the stack
	 * 
	 *  @param item Item to push onto the stack 
	 */
	void push(T item)
	{
		m_items.add(item);
	}

	/***
	 *  @brief Pops an item off the top of the stack
	 * 
	 *  @return The value of the item popped off
	 */
	T pop()
	{
		T toReturn = m_items[m_items.getCount() - 1];
		m_items.removeAt(m_items.getCount() - 1);
		return toReturn;

	}
	/***
	 *  @brief Gets the item at the top of the stack
	 * 
	 *  @return The value of the item at the top of the stack
	 */
	T peek() { return m_items[m_items.getCount() - 1]; }

	/***
	 *  @brief Gets the number of items in the stack
	 * 
	 *  @return Number of items in the stack
	 */
	int getCount() { return m_items.getCount(); }
	/***
	 *  @brief Checks whether or not the stack is empty
	 * 
	 *  @return Whether or not the stack is empty
	 */
	bool isEmpty() { return m_items.getCount() <= 0; }

private:
	DArray<T> m_items;
};