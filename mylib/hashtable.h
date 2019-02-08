#pragma once

#include <iostream>

#include "gmath.h"
#include "linkedlist.h"

template <typename V>
struct HashPair
{
	const char* key;
	V value;
};

template <typename V>
class HashTable
{
public:
	HashTable(int size)
		: m_size(size)
	{
		m_items = new LinkedList<HashPair<V>>[size];
	}

	~HashTable() { delete[] m_items; }

	/***
	 *  @brief Gets a value by key. If the key doesn't exist, make a new entry
	 * 
	 *  @param key Key associated with a value
	 *  @return Value associated with the key
	 */
	V& operator[](const char* key)
	{
		unsigned int hashed = hash(key, (int)strlen(key)) % m_size;
		auto n = m_items[hashed].begin();
		while (n)
		{
			if (strcmp(key, n->value.key) == 0)
				return n->value.value;
			n = n->next;
		}

		// nothing was found, make a new entry
		HashPair<V> newPair = { key, (V)0 };
		m_items[hashed].pushBack(newPair);
		return m_items[hashed].last().value;
	}

	/***
	 *  @brief Gets a list of all values in the hashtable
	 * 
	 *  @return A LinkedList of all values
	 */
	LinkedList<V> getAllValues()
	{
		LinkedList<V> result;
		for (int i = 0; i < m_size; ++i)
		{
			LinkedList<HashPair<V>> thisList = m_items[i];
			auto n = thisList.begin();
			while (n)
			{
				result.pushBack(n->value.value);
				n = n->next;
			}
		}
		return result;
	}

private:
	LinkedList<HashPair<V>>* m_items;
	int m_size;
};