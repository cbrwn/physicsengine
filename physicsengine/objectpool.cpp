/* =================================
 *  Object Pool
 *  Creates a bunch of objects upfront to reduce the number of allocations
 *  during gameplay
 * ================================= */
#include "objectpool.h"

#include "world.h"

ObjectPool::ObjectPool(World* world)
	: m_world(world)
{
}

ObjectPool::~ObjectPool()
{
}
