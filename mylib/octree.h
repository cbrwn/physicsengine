#pragma once

#include "darray.h"

// forward declare so we can make a pointer in OctObject
template <class T>
class Octree;

struct OctCube
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
};

template <class T>
struct OctObject
{
	T data;
	OctCube volume;
	Octree<T>* parent;
};

template <class T>
class Octree
{
public:
	Octree(int density, OctCube const& bounds)
		: m_density(density), m_bounds(bounds)
	{
		m_divided = false;

		// this tree isn't dividable (divisible?) if it gets small enough
		// - stops an eventual stack overflow when trees get tiny
		const float dividableLimit = 0.2f;
		m_dividable = bounds.maxX - bounds.minX >= dividableLimit &&
			bounds.maxY - bounds.minY >= dividableLimit;
	}

	Octree(int density, float minX, float minY, float minZ,
		float maxX, float maxY, float maxZ)
		: Octree(density, { minX, minY, minZ, maxX, maxY, maxZ })
	{
	}

	~Octree()
	{
		if (m_divided)
			for (int i = 0; i < 8; ++i)
				delete m_children[i];

		for (int i = 0; i < m_objects.getCount(); ++i)
			delete m_objects[i];
		m_objects.clear();
	}

	/***
	 * @brief Puts an object into the tree, passing it into child trees if
	 *			possible
	 *
	 * @param object Object to put into the tree
	 * @param vol Bounding box of this object
	 */
	void insert(T object, OctCube const& vol)
	{
		if ((m_divided || m_objects.getCount() >= m_density)
			&& m_dividable)
		{
			if (!m_divided)
				split();

			// pass it to all relevant segments
			// (not just one, so large objects get added to every tree they
			//		touch)
			for (int i = 0; i < 8; ++i)
			{
				Octree<T>* child = m_children[i];
				if (child->intersects(vol))
					child->insert(object, vol);
			}
		}
		else
		{
			// not divided, so the object can go in this node

			// make an OctObject for this object
			auto obj = new OctObject<T>();
			obj->data = object;
			obj->volume = vol;
			obj->parent = this;

			// stick it into the array
			m_objects.add(obj);
		}
	}

	/***
	 * @brief Removes all objects and child trees from this tree
	 */
	void clear()
	{
		if (m_divided)
			for (int i = 0; i < 8; ++i)
				delete m_children[i];
		for (int i = 0; i < m_objects.getCount(); ++i)
			delete m_objects[i];
		m_objects.clear();

		m_divided = false;
	}

	/***
	 * @brief Checks of a bounding box intersects this tree
	 *
	 * @param vol Bounding box to check for intersection
	 */
	bool intersects(OctCube const& vol)
	{
		// if any of these are true, we're NOT colliding
		if (vol.minY > m_bounds.maxY || vol.maxY < m_bounds.minY ||
			vol.minX > m_bounds.maxX || vol.maxX < m_bounds.minX ||
			vol.minZ > m_bounds.maxZ || vol.maxZ < m_bounds.minZ)
			return false;

		return true;
	}

	/***
	 * @brief Gets all objects in all trees that intersect with a box
	 *
	 * @param range Bounding box to check for intersection
	 * @return Dynamic Array containing all objects within these trees
	 */
	DArray<T> getInRange(OctCube const& range)
	{
		DArray<T> result;
		getInRange(range, &result);
		return result;
	}

	/***
	 * @brief Gets all objects in all trees that intersect with a set of bounds
	 *
	 * @param minX Minimum x-axis extent of the bounds
	 * @param minY Minimum y-axis extent of the bounds
	 * @param minZ Minimum z-axis extent of the bounds
	 * @param maxX Maximum x-axis extent of the bounds
	 * @param maxY Maximum y-axis extent of the bounds
	 * @param maxZ Maximum z-axis extent of the bounds
	 * @return Dynamic Array containing all objects within these trees
	 */
	DArray<T> getInRange(float minX, float minY, float minZ,
		float maxX, float maxY, float maxZ)
	{
		return getInRange({ minX, minY, minZ, maxX, maxY, maxZ });
	}

	bool isDivided() { return m_divided; }
	Octree* getChild(int i) { return m_children[i]; }
	OctCube getBounds() { return m_bounds; }

private:
	// how many objects can be in this tree before it splits
	int m_density;
	// bounding box of this tree
	OctCube m_bounds;

	DArray<OctObject<T>*> m_objects;

	// has this tree split
	bool m_divided;
	// is this tree large enough to split
	bool m_dividable;
	Octree* m_children[8];

	/***
	 * @brief Splits this tree into 8 equal-sized child trees
	 */
	void split()
	{
		// get the size of each box (just half as big as this)
		float depth = (m_bounds.maxZ - m_bounds.minZ) / 2.0f;
		float width = (m_bounds.maxX - m_bounds.minX) / 2.0f;
		float height = (m_bounds.maxY - m_bounds.minY) / 2.0f;

		// index of child we're creating
		int childIndex = 0;
		for (int x = 0; x < 2; ++x)
		{
			for (int y = 0; y < 2; ++y)
			{
				for (int z = 0; z < 2; ++z)
				{
					OctCube bounds;

					bounds.minX = m_bounds.minX + width * x;
					bounds.minY = m_bounds.minY + height * y;
					bounds.minZ = m_bounds.minZ + depth * z;

					bounds.maxX = bounds.minX + width;
					bounds.maxY = bounds.minY + height;
					bounds.maxZ = bounds.minZ + depth;

					m_children[childIndex] = new Octree<T>(m_density, bounds);

					childIndex++;
				}
			}
		}
		m_divided = true;

		// move objects into children
		for (int i = 0; i < m_objects.getCount(); ++i)
		{
			// check each child tree to see if this object intersects
			for (int j = 0; j < 8; ++j)
			{
				Octree<T>* child = m_children[j];
				// and insert it if it does
				if (child->intersects(m_objects[i]->volume))
					child->insert(m_objects[i]->data, m_objects[i]->volume);
			}
			// we can delete this since we're creating new objects in insert
			delete m_objects[i];
		}
		// all the objects have (hopefully) been passed onto our children
		m_objects.clear();
	}

	/***
	 * @brief Recursively adds items into a list from child trees that
	 *			intersect with a box
	 *
	 * @param cube Box to check for intersections with
	 * @param list Pointer to a dynamic array to add objects to
	 */
	void getInRange(OctCube const& cube, DArray<T>* list)
	{
		// if this doesn't intersect with this cube, none of our children would
		//	either, so we can leave
		if (!intersects(cube))
			return;

		if (m_divided)
		{
			// call this on all our children
			for (int i = 0; i < 8; ++i)
				m_children[i]->getInRange(cube, list);
		}
		else
		{
			// only add objects which aren't already in the list
			for (int i = 0; i < m_objects.getCount(); ++i)
			{
				bool wasInList = false;
				for (int j = 0; j < list->getCount(); ++j)
				{
					if (m_objects[i]->data == (*list)[j])
					{
						wasInList = true;
						break;
					}
				}
				if (!wasInList)
					list->add(m_objects[i]->data);
			}
		}
	}
};