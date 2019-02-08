/* =================================
 *  Object Pool
 *  Creates a bunch of objects upfront to reduce the number of allocations
 *  during gameplay
 *  
 *  Create a specified number of objects:
 *		pool.createMissiles(100);
 *	Get one of those objects from the pool
 *		Missile* m = pool.getMissile();
 * ================================= */
#pragma once

class World;

// objects
class Box;

class ObjectPool
{
public:
	ObjectPool(World* world);
	~ObjectPool();
private:
	World* m_world;
};