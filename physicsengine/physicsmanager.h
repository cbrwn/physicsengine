/* =================================
 *  PhysicsManager
 *  A singleton that handles everything to do with physics objects
 *  
 *  Get the instance:
 *		PhysicsManager* phys = PhysicsManager::getInstance();
 *	Then it can be used!
 *		phys->addPhysicsBody(body);
 * ================================= */
#pragma once

#include <darray.h>
#include <octree.h>
#include <vector3.h>

class PhysicsBody;

class PhysicsManager
{
public:
	static void create();
	static void destroy();

	static PhysicsManager* getInstance();

	void update(float delta);
	void clear();

	// adds a physics body to the world
	void addPhysicsBody(PhysicsBody* b);

	// returns the closest body that intersects with a ray and also outputs
	// the position that the intersection occured at
	// can return nullptr if the ray intersected nothing
	PhysicsBody* rayCast(Vector3 const& start, Vector3 const& dir, 
		Vector3& outPos);

	// grabs a pointer to the octree containing all our bodies
	Octree<PhysicsBody*>* getTree() { return m_tree; }
	
	// gets a pointer to our list of bodies
	DArray<PhysicsBody*>* getBodies() { return &m_bodies; }
	// uses the octree to get a list of bodies in a certain range
	DArray<PhysicsBody*> getBodiesInRange(Vector3 const& min, 
		Vector3 const& max);
private:
	PhysicsManager();
	~PhysicsManager();

	static PhysicsManager* m_instance;

	DArray<PhysicsBody*> m_bodies;
	Octree<PhysicsBody*>* m_tree;

	// draws an octree for debug purposes
	void drawTree(Octree<PhysicsBody*>* tree);
};