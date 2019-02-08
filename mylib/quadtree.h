#pragma once

#include "darray.h"
#include "vector2.h"

// forward declare for use in QuadObject
template<class T>
class QuadTree;

// struct used to represent rectangles
struct QuadRect
{
	float minX, minY;
	float maxX, maxY;
};

// struct which stores data put into quadtrees
template <class T>
struct QuadObject
{
	Vector2 pos;
	T data;
	QuadTree<T>* parent;
};

template <class T>
class QuadTree
{
public:
	QuadTree(int density, QuadRect const& bounds)
		: m_density(density), m_bounds(bounds)
	{
		m_divided = false;
	}
	QuadTree(int density, float minX, float minY, float maxX, float maxY)
		: QuadTree(density, { minX, minY, maxX, maxY })
	{
	}

	~QuadTree()
	{
		if (m_divided)
		{
			for (int i = 0; i < 4; ++i)
				delete m_children[i];
			return;
		}

		for (int i = 0; i < m_objects.getCount(); ++i)
			delete m_objects[i];
		m_objects.clear();
	}

    /***
     *  @brief Puts an object into the quadtree with an associated position
	 *
     *  @param object Data to store in the quadtree
     *  @param position Position associated with the data in the tree
     */
	void insert(T object, Vector2 const& position)
	{
        // pass the object to children if they exist OR create children
        // if this segment is full
		if (isDivided() || m_objects.getCount() >= m_density)
		{
			if (!isDivided())
				split();

            // find the segment to pass it to
			for (int i = 0; i < 4; ++i)
			{
				if (m_children[i]->isInside(position))
				{
					m_children[i]->insert(object, position);
					break;
				}
			}
		}
		else
		{
			// make a QuadObject from the parameters
			auto obj = new QuadObject<T>();
			obj->data = object;
			obj->pos = position;
            // and stick it into this segment
			put(obj);
		}
	}

    /***
     *   @brief Grabs a list of every object within segments that intersect
     *          with a rectangle
	 *
     *   @param rect Region to get objects from
     *   @return A DArray containing everything in that range
     */
	DArray<T> getInRange(QuadRect const& rect)
	{
		DArray<T> result;
		getInRange(rect, &result);
		return result;
	}

    /***
     *  @brief Grabs a list of every object within segments that intersect
     *          given bounds
	 *
     *  @param minX Left side of the rectangle
     *  @param minY Bottom side of the rectangle
     *  @param maxX Right side of the rectangle
     *  @param maxY Top side of the rectangle
     *  @return A DArray containing everything in that range
     */
	DArray<T> getInRange(float minX, float minY, float maxX, float maxY)
	{
		return getInRange({ minX, minY, maxX, maxY });
	}

    /***
     *  @brief Checks if a point is within the bounds of this segment
	 *
     *  @param p Point to check
     *  @return Whether or not the point is inside this segment
     */
	bool isInside(Vector2 const& p)
	{
		return p.getX() > m_bounds.minX && p.getY() > m_bounds.minY &&
			p.getX() <= m_bounds.maxX && p.getY() <= m_bounds.maxY;
	}

    /***
     *  @brief Remove all objects from this segment and its children
     *          and also delete the children
     */
	void clear()
	{
		// get rid of all children and objects in those children
		if (m_divided)
			for (int i = 0; i < 4; ++i)
				delete m_children[i];
		for (int i = 0; i < m_objects.getCount(); ++i)
			delete m_objects[i];
		m_objects.clear();

		m_divided = false;
	}

    /***
     *  @brief Sticks an object into this segment and makes sure it knows what
     *          this is
	 *
     *  @param obj Object to put into the array
     */
	void put(QuadObject<T>* obj)
	{
		obj->parent = this;
		m_objects.add(obj);
	}

    /***
     *  @brief Gets the lowest level tree which contains a point
	 *
     *  @param pos Point to check
     *  @return Pointer to a QuadTree which contains the point
     */
	QuadTree<T>* getTreeWithPoint(Vector2 const& pos)
	{
		if (isInside(pos))
		{
			if (!m_divided)
				return this;
			for (int i = 0; i < 4; ++i)
				if (m_children[i]->isInside(pos))
					return m_children[i]->getTreeWithPoint(pos);
		}
		return nullptr;
	}

	DArray<QuadObject<T>*>* getObjects() { return &m_objects; }

	// stuff used for drawing the bounds in game
	inline bool isDivided() { return m_divided; }
	QuadTree* getChild(int i) { return m_children[i]; }
	QuadRect getBounds() { return m_bounds; }

private:
	int m_density;
	QuadRect m_bounds;

	DArray<QuadObject<T>*> m_objects;

	bool m_divided;
	QuadTree* m_children[4];

    /***
     *  @brief Divides this tree into four, and passes any objects into those
     *          children
     */
	void split()
	{
		// grab some numbers to make our new children with
		float left = m_bounds.minX;
		float bottom = m_bounds.minY;
		float width = m_bounds.maxX - m_bounds.minX;
		float height = m_bounds.maxY - m_bounds.minY;

		// bottom left bounds
		QuadRect bounds0 = { left, bottom, left + width / 2, bottom + height / 2 };
		// bottom right bounds
		QuadRect bounds1 = { left + width / 2, bottom, left + width, bottom + height / 2 };
		// top right bounds
		QuadRect bounds2 = { left + width / 2, bottom + height / 2, left + width, bottom + height };
		// top left bounds
		QuadRect bounds3 = { left, bottom + height / 2, left + width / 2, bottom + height };

		// make our new children
		m_children[0] = new QuadTree<T>(m_density, bounds0);
		m_children[1] = new QuadTree<T>(m_density, bounds1);
		m_children[2] = new QuadTree<T>(m_density, bounds2);
		m_children[3] = new QuadTree<T>(m_density, bounds3);

		m_divided = true;

		// move all the objects in this into our children
		/*for (int i = 0; i < m_objects.getCount(); ++i)
		{
			auto obj = m_objects[i];
			for (int j = 0; j < 4; ++j)
			{
				if (m_children[j]->isInside(obj->pos))
				{
					obj->parent = m_children[j];
					m_children[j]->put(obj);
					break;
				}
			}
		}*/

		// instead of moving the objects efficiently, just insert new ones
		// the other way has a memory leak and I'm too dumb to find it
		for (int i = 0; i < m_objects.getCount(); ++i)
		{
			insert(m_objects[i]->data, m_objects[i]->pos);
			delete m_objects[i];
		}
		m_objects.clear();
	}

    /***
     *  @brief Recursively puts objects into a DArray if the QuadTree it's in
     *          intersects with the range
	 *
     *  @param rect
     *  @param list
     */
	void getInRange(QuadRect const& rect, DArray<T>* list)
	{
        // rectangle intersection test
		if (rect.minX > m_bounds.maxX || rect.maxX < m_bounds.minX ||
			rect.minY > m_bounds.maxY || rect.maxY < m_bounds.minY)
			return;

		if (m_divided)
            // call this function on all child trees
			for (int i = 0; i < 4; ++i)
				m_children[i]->getInRange(rect, list);
		else
            // add all the objects to the list
			for (int i = 0; i < m_objects.getCount(); ++i)
				list->add(m_objects[i]->data);
	}
};