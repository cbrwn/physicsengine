#pragma once

#include <iostream>

template <class T>
struct BinaryNode
{
	BinaryNode()
	{
		left = nullptr;
		right = nullptr;
		parent = nullptr;
	}
	T data;
	BinaryNode* left;
	BinaryNode* right;
	BinaryNode* parent;
};

template <class T>
class BinaryTree
{
public:
	typedef bool(*CompareFunction)(T lhs, T rhs);

	/***
	 *	@brief Creates an empty binary tree which uses the given
	 *			comparison function to order the tree
	 *
	 *	@param cmp Function pointer which returns a bool representing the
	 *				order of two objects of type T
	 */
	explicit BinaryTree(CompareFunction cmp)
	{
		m_cmp = cmp;
		m_root = nullptr;
	}

	~BinaryTree()
	{
		// the tree is already empty if there's no root node
		if (!m_root)
			return;

		// start all the way down the bottom/left of the tree
		auto cnode = m_root;
		while (cnode->left)
			cnode = cnode->left;

		// delete all the nodes
		while (cnode)
		{
			// go down as far as possible
			if (cnode->left)
			{
				cnode = cnode->left;
			}
			else if (cnode->right)
			{
				cnode = cnode->right;
			}
			else
			{
				// we're at the bottom of this path
				// delete this node and set the next node to the parent
				auto temp = cnode->parent;
				if (cnode->parent)
				{
					// make sure the parent knows this child is being deleted
					if (cnode == cnode->parent->left)
						cnode->parent->left = nullptr;
					if (cnode == cnode->parent->right)
						cnode->parent->right = nullptr;
				}
				delete cnode;
				cnode = temp;
			}
		}
	}

	/***
	 *  @brief Adds a new node to the tree and orders it 
	 *
	 *  @param data The data to be stored in the new node
	 */
	void add(T data)
	{
		// make our new node
		auto newNode = new BinaryNode<T>();
		newNode->data = data;

		// if it's the only thing in the tree, we don't need to do much
		if (!m_root)
		{
			m_root = newNode;
			return;
		}

		// start finding where this node should go
		auto checkNode = m_root;
		while (checkNode)
		{
			if (m_cmp(data, checkNode->data))
			{
				// new node belongs on the left of this node
				if (checkNode->left)
				{
					// there's already one to the left, continue down this path
					checkNode = checkNode->left;
				}
				else
				{
					// this is where the node belongs!
					checkNode->left = newNode;
					newNode->parent = checkNode;
					break;
				}
			}
			else
			{
				// new node belongs on the right of this node
				if (checkNode->right)
				{
					// there's already one to the right, continue down the path
					checkNode = checkNode->right;
				}
				else
				{
					// this is where the node belongs!
					checkNode->right = newNode;
					newNode->parent = checkNode;
					break;
				}
			}
		}
	}

	/***
	 *  @brief Finds a node with a specific value
	 *
	 *  @param data Value to look for in the tree
	 *  @return A pointer to the node containing the data
	 */
	BinaryNode<T>* find(T data)
	{
		auto checkNode = m_root;
		while (checkNode)
		{
			// check if we've found it
			if (checkNode->data == data)
				return checkNode;

			// check which side of this node the value should be on
			if (m_cmp(data, checkNode->data))
				checkNode = checkNode->left;
			else
				checkNode = checkNode->right;
		}

		// value wasn't found!
		return nullptr;
	}

	/***
	 *  @brief Removes a node from the tree, keeping its structure in tact
	 *
	 *  @param node The node to remove
	 *  @return The data of the removed node
	 */
	T remove(BinaryNode<T>* node)
	{
		// result of find() in the other remove functions can be nullptr
		if (!node)
			// I think this'll return 0 if T is int and nullptr if T is a pointer so that's fine
			// it won't work properly for other types and won't let you compile
			return (T)0;

		// grab some boolsies about the number of children
		bool twoChildren = node->left && node->right;
		bool noChildren = !node->left && !node->right;
		bool oneChild = !twoChildren && !noChildren;
		// and more about which child of the parent we are
		bool isLeft = node->parent ? node->parent->left == node : false;
		bool isRight = node->parent ? node->parent->right == node : false;

		if (noChildren)
		{
			// if it has no children, we can simply delete it

			// make sure its parent stops pointing at it
			if (isLeft)
				node->parent->left = nullptr;
			if (isRight)
				node->parent->right = nullptr;

			// check if it's our root node
			if (node == m_root)
				m_root = nullptr;
		}

		if (oneChild)
		{
			// if it has one child, move that child to its parents spot
			auto child = node->left ? node->left : node->right;
			if (node->parent)
			{
				// make sure the parent now points to the child
				if (isLeft)
					node->parent->left = child;
				if (isRight)
					node->parent->right = child;
			}
			// make sure the child knows about its new parent
			child->parent = node->parent;

			// check if we're removing our root node
			if (node == m_root)
				m_root = child;
		}

		if (twoChildren)
		{
			// if the selected node has two children, we grab a node further down
			// in the tree and put it in its spot

			// move one to the left and then all the way to the right
			auto n = node->left;
			while (n->right)
				n = n->right;

			// handle the replacement node's parents
			if (n->parent)
			{
				// give the node's children to its parents
				// (if it has any)
				if (n->parent->right == n)
					n->parent->right = n->left;
				if (n->parent->left == n)
					n->parent->left = n->left;
			}
			// make sure the newly adopted child knows its new parent
			if (n->left)
				n->left->parent = n->parent;

			// make the parent of the node we're removing know its new child
			if (isLeft)
				node->parent->left = n;
			if (isRight)
				node->parent->right = n;
			// and make the node know about its new parent
			n->parent = node->parent;

			// give the children of the removed node to the new one
			if (node->left != n)
				n->left = node->left;
			if (node->right != n)
				n->right = node->right;
			// and make sure they know about their new parent
			if (n->left)
				n->left->parent = n;
			if (n->right)
				n->right->parent = n;

			// replace the root node if we're deleting it
			if (node == m_root)
				m_root = n;
		}

		// grab a copy of the node's data so we can return it after deleting
		T result = node->data;
		delete node;
		return result;
	}

	/***
	 *	@brief Tries to remove a node containing a value from the tree
	 *
	 *	@param val The data to look for inside the node
	 *  @return Whether or not the value was successfully removed from the tree
	 */
	bool remove(T val)
	{
		// simply find the node and use that result to remove it
		// and then return whether or not the removed value was the desired one
		return remove(find(val)) == val;
	}

	/***
	 *  @brief Gets the root node, the first node added to the tree
	 *
	 *	@return A pointer to the root node
	 */
	BinaryNode<T>* getRoot() { return m_root; }

private:
	BinaryNode<T>* m_root;
	CompareFunction m_cmp;
};