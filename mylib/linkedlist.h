#pragma once

template <class T>
class ListNode
{
public:
	explicit ListNode(T& val)
	{
		value = val;
		previous = nullptr;
		next = nullptr;
	}

	ListNode<T>* previous;
	ListNode<T>* next;
	T value;
};

template <class T>
class LinkedList
{
public:
	LinkedList()
	{
		m_firstNode = nullptr;
		m_lastNode = nullptr;
	}

	~LinkedList()
	{
		auto node = m_firstNode;
		while (node)
		{
			// keep the next node here so we can use it after deleting node
			auto nextNode = node->next;
			// kill the node
			delete node;
			// move to the next node
			node = nextNode;
		}
	}

	// copy constructors
	LinkedList(LinkedList& list)
	{
		m_firstNode = nullptr;
		m_lastNode = nullptr;
		for (auto n = list.begin(); n; n = n->next)
			pushBack(n->value);
	}

	LinkedList& operator=(LinkedList const& list)
	{
		// clear out this list
		auto node = m_firstNode;
		while (node)
		{
			// keep the next node here so we can use it after deleting node
			auto nextNode = node->next;
			// kill the node
			delete node;
			// move to the next node
			node = nextNode;
		}
		m_firstNode = nullptr;
		m_lastNode = nullptr;
		for (auto n = list.begin(); n; n = n->next)
			pushBack(n->value);
		return *this;
	}

	LinkedList(LinkedList&& list) noexcept
	{
		m_firstNode = nullptr;
		m_lastNode = nullptr;
		for (auto n = list.begin(); n; n = n->next)
			pushBack(n->value);

		while (list.begin())
			list.popFront();
	}

	LinkedList& operator=(LinkedList const&& list) noexcept
	{
		// clear out this list
		auto node = m_firstNode;
		while (node)
		{
			// keep the next node here so we can use it after deleting node
			auto nextNode = node->next;
			// kill the node
			delete node;
			// move to the next node
			node = nextNode;
		}
		m_firstNode = nullptr;
		m_lastNode = nullptr;
		for (auto n = list.begin(); n; n = n->next)
			pushBack(n->value);

		while (list.begin())
			list.popFront();

		return *this;
	}

	/***
	 *  @brief Pushes an item onto the back of the list
	 *
	 *  @param item Item to put onto the list
	 */
	void pushBack(T item)
	{
		// first we make a node for the item
		auto node = new ListNode<T>(item);

		// make sure our last node and this one know about each other
		if (m_lastNode)
		{
			m_lastNode->next = node;
			node->previous = m_lastNode;
		}

		// check if we don't have a beginning yet
		if (!m_firstNode)
			m_firstNode = node;

		m_lastNode = node;
	}

	/***
	 *  @brief Pushes an item onto the front of the list
	 *
	 *  @param item Item to put onto the list
	 */
	void pushFront(T item)
	{
		// first we make a node for the item
		auto node = new ListNode<T>(item);

		// make sure our first node and this one know about each other
		if (m_firstNode)
		{
			m_firstNode->previous = node;
			node->next = m_firstNode;
		}

		// check if we don't have an end yet
		if (!m_lastNode)
			m_lastNode = node;

		m_firstNode = node;
	}

	/***
	 *  @brief Inserts an item before a node
	 *
	 *  @param before Node to insert item before
	 *  @param item Item to insert into the list
	 */
	void insert(ListNode<T>* before, T item)
	{
		// first we make a node for the item
		auto node = new ListNode<T>(item);

		// grab the node before the node we're inserting before
		auto previousNode = before->previous;
		if (previousNode)
		{
			previousNode->next = node;
			node->previous = previousNode;
		}

		before->previous = node;
		node->next = before;
	}

	/***
	 *  @brief Gets a node at a certain index
	 *
	 *  @param index Index to look for
	 *  @return The node at the specified index, or nullptr if it doesn't exist
	 */
	ListNode<T>* get(int index)
	{
		int currentIndex = 0;
		auto node = m_firstNode;
		for (; node; node = node->next)
		{
			if (currentIndex == index)
				return node;
			currentIndex++;
		}
		return nullptr;
	}

	/***
	 *  @brief Pops an item off the front of the list
	 *
	 *  @return The data in the node that was popped
	 */
	T popFront()
	{
		// not sure how to handle this, just casting 0 to the type
		// should work fine for numbers and pointers
		if (!m_firstNode)
			return (T)0;

		// grab the value we're going to return
		T toReturn = m_firstNode->value;

		// get the new first node
		auto newFirst = m_firstNode->next;

		// we can get rid of the first node we're popping
		delete m_firstNode;

		// set the new first node
		if (newFirst)
			newFirst->previous = nullptr;
		m_firstNode = newFirst;

		return toReturn;
	}

	/***
	 *  @brief Pops an item off the end of the list
	 *
	 *  @return The data in the node that was popped
	 */
	T popBack()
	{
		// not sure how to handle this
		if (!m_lastNode)
			return (T)0;

		// keep track of the new last node
		auto newLast = m_lastNode->previous;

		// grab the value we're going to return
		T toReturn = m_lastNode->value;

		// get rid of the last node
		delete m_lastNode;

		// set the new last node
		if (newLast)
			newLast->next = nullptr;
		m_lastNode = newLast;

		return toReturn;
	}

	/***
	 *  @brief Removes a node from the list
	 *
	 *  @param node Node to remove
	 *  @return The data in the node that was removed
	 */
	T erase(ListNode<T>* node)
	{
		// grab the data we're going to return
		T toReturn = node->value;
		if (node == m_firstNode)
		{
			popFront();
		}
		else if (node == m_lastNode)
		{
			popBack();
		}
		else
		{
			node->previous->next = node->next;
			node->next->previous = node->previous;
			delete node;
		}
		return toReturn;
	}

	/***
	 *  @brief Removes an item from the list by value
	 *
	 *  @param value Item to remove
	 */
	void remove(T value)
	{
		auto node = m_firstNode;
		while (node)
		{
			auto nextNode = node->next;
			if (node->value == value)
				erase(node);
			node = nextNode;
		}
	}

	ListNode<T>* begin() { return m_firstNode; }
	// slides say end() should return a null ListNode??
	ListNode<T>* end() { return m_lastNode; }

	// grabbing values
	T& first() { return m_firstNode->value; }
	T& last() { return m_lastNode->value; }

private:
	ListNode<T>* m_firstNode;
	ListNode<T>* m_lastNode;
};