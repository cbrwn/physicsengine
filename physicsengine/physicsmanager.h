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

#include <vector>
#include <octree.h>
#include <glm/glm.hpp>

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
	PhysicsBody* rayCast(glm::vec3 const& start, glm::vec3 const& dir, 
		glm::vec3& outPos);

	// grabs a pointer to the octree containing all our bodies
	Octree<PhysicsBody*>* getTree() { return m_tree; }
	
	// gets a pointer to our list of bodies
	std::vector<PhysicsBody*>* getBodies() { return &m_bodies; }
	// uses the octree to get a list of bodies in a certain range
	std::vector<PhysicsBody*> getBodiesInRange(glm::vec3 const& min, 
		glm::vec3 const& max);

    static float gravity;

	// draws an octree for debug purposes
	void drawTree(Octree<PhysicsBody*>* tree);
private:
	PhysicsManager();
	~PhysicsManager();

	static PhysicsManager* m_instance;

	std::vector<PhysicsBody*> m_bodies;
	Octree<PhysicsBody*>* m_tree;

};
