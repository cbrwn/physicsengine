/* =================================
 *  PhysicsManager
 *  A singleton that handles everything to do with physics objects
 * ================================= */
#include "physicsmanager.h"

#include <Gizmos.h>

#include "util.h"
#include "physicsbody.h"
#include "collideraabb.h"

PhysicsManager* PhysicsManager::m_instance = nullptr;

PhysicsManager::PhysicsManager()
{
	// arbitrary extent of the octree
	// no collision will work outside of this range
	const float treeSize = 3000.0f;
	m_tree = new Octree<PhysicsBody*>(8,
		{ -treeSize, -treeSize, -treeSize,
		treeSize, treeSize, treeSize });
}

PhysicsManager::~PhysicsManager()
{
	delete m_tree;
}

void PhysicsManager::drawTree(Octree<PhysicsBody*>* tree)
{
	// turn this tree's bounds into vectors so we can use them
	OctCube volume = tree->getBounds();

	Vector3 min(volume.minX, volume.minY, volume.minZ);
	Vector3 max(volume.maxX, volume.maxY, volume.maxZ);

	Vector3 extents = (max - min) / 2.0f;
	Vector3 center = min + extents;

	aie::Gizmos::addAABB(toVec3(center), toVec3(extents),
		glm::vec4(0, 0, 0, 1));

	// recursively draw this tree's children
	if (tree->isDivided())
		for (int i = 0; i < 8; ++i)
			drawTree(tree->getChild(i));
}

void PhysicsManager::create()
{
	m_instance = new PhysicsManager();
}

void PhysicsManager::destroy()
{
	delete m_instance;
}

PhysicsManager* PhysicsManager::getInstance()
{
	return m_instance;
}

void PhysicsManager::addPhysicsBody(PhysicsBody* b)
{
	m_bodies.add(b);
}

void PhysicsManager::update(float delta)
{
	// SUPER BASIC APPROACH, just clearing and re-inserting all bodies into
	// the tree each frame
	// not ideal, but the performance improvement from not using an octree
	// at all is already substantial using this
	m_tree->clear();
	for (int i = 0; i < m_bodies.getCount(); ++i)
	{
		auto body = m_bodies[i];
		if (body->isEnabled())
		{
			Vector3 pos = body->getPosition();
			Vector3 extents = body->getBroadExtents();
			// turn the body's extents into a volume usable by the octree
			OctCube cube;
			cube.minX = pos.x - extents.x;
			cube.minY = pos.y - extents.y;
			cube.minZ = pos.z - extents.z;

			cube.maxX = pos.x + extents.x;
			cube.maxY = pos.y + extents.y;
			cube.maxZ = pos.z + extents.z;
			m_tree->insert(body, cube);
		}
	}

	for (int i = 0; i < m_bodies.getCount(); ++i)
		m_bodies[i]->update(delta);
}

void PhysicsManager::clear()
{
	m_bodies.clear();
}

DArray<PhysicsBody*> PhysicsManager::getBodiesInRange(Vector3 const& min, 
	Vector3 const& max)
{
	// turn these vectors into a usable OctCube
	OctCube volume;
	volume.minX = min.x;
	volume.minY = min.y;
	volume.minZ = min.z;

	volume.maxX = max.x;
	volume.maxY = max.y;
	volume.maxZ = max.z;

	return m_tree->getInRange(volume);
}

// performs a ray cast on all bodies WITH AN AABB COLLIDER, not implemented for
// any other kind of collider!
PhysicsBody* PhysicsManager::rayCast(Vector3 const& start, 
	Vector3 const& dir, Vector3& outPos)
{
	auto col = new ColliderAABB(Vector3(0.01f, 0.01f, 0.01f));
	PhysicsBody testBody(col);

	Vector3 nDir = dir.normalised();
	Vector3 nStart = start;

	float closestDist = INFINITY;
	PhysicsBody* closest = nullptr;

	for (int i = 0; i < m_bodies.getCount(); ++i)
	{
		PhysicsBody* body = m_bodies[i];
		Collider* collider = body->getCollider();

		if (!collider || !body->isEnabled())
			continue;
		float d = 0.0f;
		//// ray casting only works for boxes!
		//if (collider->type != COLLIDER_AABB)
		//	continue;

		//auto aabb = (ColliderAABB*)collider;
		if (body->rayTestBroad(nStart, nDir, &d))
		{
			Vector3 pos = (nDir * d);
			float f = pos.dot(dir);
			// make sure the object isn't behind the start position
			if (d < closestDist && d >= 0.0f)
			{
				closestDist = d;
				closest = body;
			}
		}
	}

	outPos = nStart + (nDir * closestDist);

	return closest;
}
