#pragma once

#include "darray.h"

template <typename T>
struct GraphNode;

template <typename T>
struct GraphEdge
{
	GraphNode<T>* end;
	int cost;
};

template <typename T>
struct GraphNode
{
	explicit GraphNode(T value) { 
		data = value; 
		prev = nullptr;
		G = 4000000000;
		traversed = false;
	}
	DArray<GraphEdge<T>> edges;
	T data;

	GraphNode<T>* prev;
	int G;
	bool traversed;
};

template <class T>
class Graph
{
public:
	Graph() = default;

	~Graph()
	{
		// deleting data potentially inside the nodes is up to the user
		for (int i = 0; i < m_nodes.getCount(); ++i)
			delete m_nodes[i];
		m_nodes.clear();
	}

	/***
	 * @brief Inserts an item into the graph
	 * 
	 * @param item Value to put into a new node
	 * @return A pointer to the newly created node
	 */
	GraphNode<T>* insert(T item)
	{
		// make a new node
		auto node = new GraphNode<T>(item);

		// add it disjointed from everything else
		m_nodes.add(node);

		// and return it so we can use it for edges and stuff
		return node;
	}

	/***
	 * @brief Removes a node from the graph
	 * 
	 * @param node Pointer to the node to remove
	 * @return The data inside the deleted node
	 */
	T remove(GraphNode<T>* node)
	{
		// go through all nodes and check if any are connected
		for (int i = 0; i < m_nodes.getCount(); ++i)
		{
			auto otherNode = m_nodes[i];
			for (int j = otherNode->edges.getCount() - 1; j >= 0; --j)
			{
				GraphEdge<T> edge = otherNode->edges[j];
				// check if edge is connected to the node we're removing
				if (edge.end == node)
					// remove the edge!
					otherNode->edges.removeAt(j);
			}
		}

		m_nodes.remove(node);
		
		// make a copy of the data to return it
		T data = node->data;
		delete node;
		return data;
	}

	/***
	 * @brief Connects two nodes in one direction
	 * 		Meaning only the start will know about this connection,
	 * 		the end is only pointed to in the new edge
	 * 
	 * @param start Pointer to the beginning of the connection
	 * @param end Pointer to the end of the connection
	 * @return Pointer to the newly created edge
	 */
	GraphEdge<T>* connect(GraphNode<T>* start, GraphNode<T>* end)
	{
		if (start == end)
			return nullptr;

		GraphEdge<T> edge;
		edge.end = end;
		start->edges.add(edge);
		return &start->edges[start->edges.getCount() - 1];
	}

	/***
	 * @brief Disconnects two nodes in one direction
	 *			Meaning it will only work if start is connected to end,
	 *			and not the other way around
	 *
	 * @param start Pointer to the beginning of the connection
	 * @param end Pointer to the end of the connection
	 * @return Whether or not the disconnection was successful
	 */
	bool disconnect(GraphNode<T>* start, GraphNode<T>* end)
	{
		for (int i = 0; i < start->edges.getCount(); ++i)
			if (start->edges[i].end == end)
			{
				start->edges.removeAt(i);
				return true;
			}
		return false;
	}

	/***
	 * @brief Checks if a node has an edge connecting it to another node
	 *			Only works if start was connected to end, returns false if end
	 *			contains a connection to start
	 *
	 * @param start Pointer to the beginning of the connection
	 * @param end Pointer to the end of the connection
	 * @return Whether or not a connection exists
	 */
	bool isConnected(GraphNode<T>* start, GraphNode<T>* end)
	{
		for (int i = 0; i < start->edges.getCount(); ++i)
			if (start->edges[i].end == end)
				return true;

		return false;
	}

	/***
	 * @brief Gets the number of nodes in the graph
	 *
	 * @return Number of nodes in the graph
	 */
	int getCount() { return m_nodes.getCount(); }
	/***
	 * @brief Allows indexing the graph like an array
	 *
	 * @param index The index of the node
	 * @return A pointer to the node at the index
	 */
	GraphNode<T>* operator[](int index) { return m_nodes[index]; }

private:
	DArray<GraphNode<T>*> m_nodes;
};