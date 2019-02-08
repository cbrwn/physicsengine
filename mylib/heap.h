#pragma once

#include <functional>
#include "darray.h"

template <class T>
class Heap
{
public:
	Heap(bool maxHeap = false)
		: m_maxHeap(maxHeap)
	{
		m_compareFunction = [](T a, T b)
		{
			return a < b;
		};
	}

	/***
	 *  @brief Create a heap by copying values from a DArray
	 *
	 *  @param da DArray to copy from
	 */
	explicit Heap(DArray<T> const& da)
	{
		clear();
		for (int i = 0; i < da.getCount(); ++i)
			push(da[i]);
	}

	void setCompareFunction(std::function<bool(T, T)> func)
	{
		m_compareFunction = func;
	}

	/***
	 *  @brief Pushes a value onto the back of the heap
	 *
	 *  @param item Value to add
	 */
	void push(T item)
	{
		m_items.add(item);

		// grab the index of the new item
		int index = m_items.getCount() - 1;
		// and balance the heap from there
		balanceUp(index);
	}

	/***
	 *  @brief Removes an item from the heap at a specified index
	 *
	 *  @param i Index to remove
	 */
	void removeAt(int i)
	{
		m_items.removeAt(i);

		bool changed = true;
		// continue to balance the heap until it's properly balanced
		while (changed)
		{
			changed = false;
			for (int j = m_items.getCount() - 1; j >= 0; --j)
				if (balanceUp(j))
					changed = true;
		}
	}

	/***
	 *  @brief Checks if the heap is properly balanced
	 *
	 *  @return Whether or not the heap is balanced
	 */
	bool isBalanced()
	{
		for (int i = 0; i < m_items.getCount(); ++i)
		{
			int value = m_items[i];

			int leftChild = firstChildIndex(i);
			int rightChild = secondChildIndex(i);

			// check if the left child exists
			if (leftChild < m_items.getCount())
			{
				int leftValue = m_items[leftChild];
				// if this item is out of order then the heap is not balanced!
				if (m_compareFunction(leftValue, value) && m_maxHeap)
					return false;
				if (m_compareFunction(value, leftValue) && !m_maxHeap)
					return false;
			}
			// check if the right child exists
			if (rightChild < m_items.getCount())
			{
				int rightValue = m_items[rightChild];
				// if this item is out of order then the heap is not balanced!
				if (m_compareFunction(rightValue, value) && m_maxHeap)
					return false;
				if (m_compareFunction(value, rightValue) && !m_maxHeap)
					return false;
			}
		}
		// checked every item against its parent and it's all balanced!
		return true;
	}

	/***
	 *  @brief Gets the value at the top of the heap
	 *			This will be either the maximum or the minimum value, depending
	 *			on m_maxHeap
	 *
	 *  @return Value at the top of the heap
	 */
	T peek()
	{
		return m_items[0];
	}
	/***
	 *  @brief Removes the value at the top of the heap and returns it
	 *			Value will either be the maximum or the minimum value,
	 *			depending on m_maxHeap
	 *
	 *  @return Value which was just popped off the heap
	 */
	T pop()
	{
		T item = peek();
		removeAt(0);
		return item;
	}
	/***
	 *  @brief Removes everything from the heap
	 */
	void clear() { m_items.clear(); }

	int getCount() { return m_items.getCount(); }
	T& operator[](int index) { return m_items[index]; }

	// static functions to help with getting parent/child indices
	static int parentIndex(int i) { return (i - 1) / 2; }
	static int firstChildIndex(int i) { return 2 * i + 1; }
	static int secondChildIndex(int i) { return 2 * i + 2; }
private:
	DArray<T> m_items;
	bool m_maxHeap;

	std::function<bool(T, T)> m_compareFunction;

	/***
	 *  @brief Balances the heap upwards recursively starting from an index
	 *
	 *  @param index Index to start balancing at
	 *  @return Whether or not any items were moved
	 */
	bool balanceUp(int index)
	{
		if (index <= 0)
			return false;

		int parent = parentIndex(index);
		if (parent < 0) // should be returned by now anyway
			return false;

		T parentValue = m_items[parent];
		T thisValue = m_items[index];

		// this assumes T has a >/< operator
		bool swapItems = m_compareFunction(parentValue, thisValue);
		if (!m_maxHeap)
			swapItems = m_compareFunction(thisValue, parentValue);
		if (swapItems)
		{
			m_items[index] = parentValue;
			m_items[parent] = thisValue;
			balanceUp(parent);
			return true;
		}
		return false;
	}

};