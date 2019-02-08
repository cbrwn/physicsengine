#pragma once
/*
DArray - Dynamic Array
Simple replacement for std::vector
*/

#include <iostream>
#include <cstring>

template <class T>
class DArray
{
public:
	// typedef for sorting functions
	typedef bool(*CompareFunction)(T lhs, T rhs);
	typedef int(*IntegizeFunction)(T obj); // this name is great go away

	DArray()
	{
		// arbitrarily start at 8 items
		m_size = 8;
		m_itemCount = 0;

		m_items = new T[m_size];
	}
	~DArray() { delete[] m_items; }

	// copy constructors
	DArray(DArray& da)
	{
		m_size = da.getCurrentSize();
		m_itemCount = da.getCount();
		m_items = new T[m_size];
		for (int i = 0; i < da.getCount(); ++i)
			m_items[i] = da[i];
	}
	DArray& operator=(DArray const& da)
	{
		m_size = da.getCurrentSize();
		m_itemCount = da.getCount();
		resize(m_size);
		for (int i = 0; i < da.getCount(); ++i)
			m_items[i] = da[i];
		return *this;
	}

	// I'm not sure how move constructors work so leave them as default
	DArray(DArray&& da) noexcept
	{
		m_size = da.getCurrentSize();
		m_itemCount = da.getCount();
		m_items = new T[m_size];
		for (int i = 0; i < m_itemCount; ++i)
			m_items[i] = da[i];

		da.clear();
	}
	DArray& operator=(DArray&& da) noexcept
	{
		m_size = da.getCurrentSize();
		m_itemCount = da.getCount();
		resize(m_size);
		for (int i = 0; i < m_itemCount; ++i)
			m_items[i] = da[i];

		da.clear();
		return *this;
	}

	T& operator[](int index) { return m_items[index]; }

	/***
	 *  @brief Adds an element to the array
	 *
	 *  @param item Item to add to the end of the array
	 *  @return
	 */
	void add(const T item)
	{
		// stick it on the end of the array
		m_items[m_itemCount] = item;
		m_itemCount++;

		// resize if we get close to max size
		if (m_itemCount >= m_size - 1)
			resize(m_size * 2);
	}

	/***
	 *  @brief Inserts an item into the array at a specified index
	 *
	 *  @param item Item to insert into the array 
	 *  @param index Index to insert the item at
	 */
	void insert(const T& item, int index)
	{
		if (index >= m_itemCount)
			resize(index + 1);
		if (index < 0)
			index = 0;

		// move everything after the index
		for (int i = m_itemCount -1; i >= index-1; --i)
			m_items[i + 1] = m_items[i];

		// then stick the item in
		m_items[index] = item;

		m_itemCount++;

		// resize if we get close to max size
		if (m_itemCount >= m_size - 1)
			resize(m_size * 2);
	}

	/***
	 *  @brief Removes an item from the array by index
	 *
	 *  @param index The index of the item to remove from the array
	 */
	void removeAt(int index)
	{
		// remove by putting the top of the array into the index then popping
		T lastElement = m_items[m_itemCount - 1];
		m_items[index] = lastElement;
		pop();
	}

	/***
	 *  @brief Removes an item from the array by value
	 *
	 *  @param item Item to search for and remove
	 */
	void remove(const T& item)
	{
		for (int i = 0; i < m_itemCount; ++i)
		{
			if (m_items[i] == item)
			{
				removeAt(i);
				break;
			}
		}
	}

	/***
	 *  @brief Removes an item from the array by index, 
	 *			keeping the array in order
	 *
	 *  @param index Index of the item to remove from the array
	 */
	void removeAtOrdered(int index)
	{
		// removing the last item is just popping off the top
		if (index == m_itemCount - 1)
		{
			pop();
		}
		else
		{
			// make sure we're in the bounds of the array
			if (index < 0 || index >= m_itemCount)
				return;

			// shift all elements left, overwriting the element at index
			for (int i = index + 1; i < m_itemCount; ++i)
				m_items[i - 1] = m_items[i];
			m_itemCount--;
		}

		// resize if it gets below half the max size
		if (m_itemCount <= (m_size / 2) - 1)
			resize(m_size / 2);
	}

	/***
	 *  @brief Removes an item from the array by value, keeping the
	 *			array in order
	 *
	 *  @param item Item to search for and remove
	 */
	void removeOrdered(const T& item)
	{
		for (int i = 0; i < m_itemCount; ++i)
		{
			if (m_items[i] == item)
			{
				removeAtOrdered(i);
				break;
			}
		}
	}

	/***
	 *  @brief Gets the actual size of the array
	 *
	 *  @return The size of the array
	 */
	int getCurrentSize() const { return m_size; }
	/***
	 *  @brief Gets the number of items in the array
	 *
	 *  @return The number of items in the array
	 */
	int getCount() const { return m_itemCount; }

	// pop and clear don't actually do much
	// they just change the item count so that unwanted items aren't
	//   accessed and will be overwritten
	void pop() { m_itemCount--; }
	void clear() { m_itemCount = 0; }

	// use subscript operator to get array elements, not this!
	T* _getArray() { return m_items; }

	// SORTS BELOW
	// ordered by speed/complexity/whatever
	// basically bubble sort is worst and radix sort is best

	/***
	 *  @brief Sorts the array using bubble sort
	 *
	 *  @param cmp Pointer to a function which will be used to compare items
	 */
	void bubbleSort(CompareFunction cmp)
	{
		for (int i = 0; i < getCount() - 1; ++i)
		{
			// keep track of if anything was changed
			// if nothing's changed then it's sorted!
			bool changed = false;
			for (int j = 0; j < getCount() - 1; ++j)
			{
				if (cmp(m_items[j + 1], m_items[j]))
				{
					// swap items
					int temp = m_items[j];
					m_items[j] = m_items[j + 1];
					m_items[j + 1] = temp;
					changed = true;
				}
			}
			if (!changed)
				break;
		}
	}

	/***
	 *  @brief Sorts the array using selection sort
	 *			Written with the help of:
	 *			https://www.cs.usfca.edu/~galles/visualization/ComparisonSort.html
	 *
	 *  @param cmp Pointer to a function which will be used to compare items
	 */
	void selectionSort(CompareFunction cmp)
	{
		for (int i = 0; i < getCount(); ++i)
		{
			// get the smallest value in the array
			int smallestIndex = i;
			T smallest = m_items[i];
			for (int j = i; j < getCount(); ++j)
			{
				if (cmp(m_items[j], smallest))
				{
					smallest = m_items[j];
					smallestIndex = j;
				}
			}

			// swap that value with the current one
			T temp = m_items[i];
			m_items[i] = m_items[smallestIndex];
			m_items[smallestIndex] = temp;
		}
	}

	/***
	 *  @brief Sorts the array using heapsort
	 *
	 *  @param cmp Pointer to a function which will be used to compare items
	 */
	void heapSort(CompareFunction cmp)
	{
		// build heap in place
		for (int i = ((getCount() - 2) / 2); i >= 0; --i)
			heapify(cmp, i, getCount() - 1);

		for (int i = getCount() - 1; i >= 0; --i)
		{
			// swap root element with last element which hasn't been sorted
			T temp = m_items[i];
			m_items[i] = m_items[0]; // now in the correct position
			m_items[0] = temp;

			// heapify from bottom up
			heapify(cmp, 0, i - 1);
		}
	}

	/***
	 *  @brief Sorts the array using radix sort
	 *			Should only be performed on types that can be properly
	 *			"integized" - meaning an integer can be gotten from it and used
	 *			to sort
	 *
	 *  @param fnc Pointer to a function which should return an integer to sort
	 *				by. Should only return positive values!
	 */
	void radixSort(IntegizeFunction fnc)
	{
		int highestDigits = 1;
		for (int i = 0; i < getCount(); ++i)
		{
			// if it's >= highest*10, the highest is highest*10
			if (fnc(m_items[i]) >= highestDigits * 10)
			{
				highestDigits *= 10;
				// process this element again in case it's 10x larger than this
				--i;
			}
		}

		// declare stuff here so we're not allocating things all the time
		int columnCount[10]; // 10 because there's 10 possible digits, 0 - 9
		// need to dynamically allocate this because our size is variable
		auto result = new T[getCount()];

		// the current column we're processing in radix sort
		int column = 1;
		for (; column <= highestDigits; column *= 10)
		{
			// body of radix sort

			// clear arrays we'll be using
			memset(columnCount, 0, 10 * sizeof(int));
			memset(result, 0, getCount() * sizeof(T));

			// count the occurrence of digits
			for (int i = 0; i < getCount(); ++i)
			{
				int numInColumn = (fnc(m_items[i]) / column) % 10;
				columnCount[numInColumn]++;
			}

			// add up these occurrences
			for (int i = 0; i < 9; ++i)
				columnCount[i + 1] += columnCount[i];

			// arrange elements based on their number in this column
			for (int i = getCount() - 1; i >= 0; --i)
			{
				int numInColumn = (fnc(m_items[i]) / column) % 10;
				int newIndex = --columnCount[numInColumn];
				result[newIndex] = m_items[i];
			}

			// stick those values in that order back into the array
			for (int i = 0; i < getCount(); ++i)
				m_items[i] = result[i];
		}

		// clean up our dynamically allocated array
		delete[] result;
	}
private:
	T * m_items;
	int		m_size;
	int		m_itemCount;

	/***
	 *  @brief Orders the array in heap ordering from 'start' to 'end'
	 *			Written using the pseudocode for 'siftDown' from:
	 *			https://en.wikipedia.org/wiki/Heapsort
	 *
	 *  @param cmp Pointer to a function which will be used to compare items
	 *  @param start Index to start heapifying from
	 *  @param end Index to end heapifying
	 */
	void heapify(CompareFunction cmp, int start, int end)
	{
		int root = start;

		// continue while root has a child
		while (2 * root + 1 <= end)
		{
			int child = 2 * root + 1;
			int swap = root; // keep track of the index to swap

			if (cmp(m_items[swap], m_items[child]))
				swap = child;
			// check for a second child and compare it
			if (child + 1 <= end && cmp(m_items[swap], m_items[child + 1]))
				swap = child + 1;

			if (swap == root)
			{
				// reached the top!
				break;
			}

			// we need to swap
			T temp = m_items[swap];
			m_items[swap] = m_items[root];
			m_items[root] = temp;
			root = swap;
		}
	}

	/***
	 *  @brief Resizes the array by creating a new one and copying everything
	 * 
	 *  @param newSize Size of the new array
	 */
	void resize(int newSize)
	{
		m_size = newSize;

		// make a whole new array with the new size
		auto resized = new T[newSize];

		// make sure we're not trying to copy more than the size
		int amountToCopy = m_itemCount;
		if (newSize < amountToCopy)
			amountToCopy = newSize;

		// copy all the stuff over
		for (int i = 0; i < amountToCopy; ++i)
			resized[i] = m_items[i];

		// get rid of the old array
		delete[] m_items;
		// and make it point to the new one
		m_items = resized;

		// apparently realloc doesn't like to work :(
		//m_items = (T*)realloc(m_items, m_size);
	}
};