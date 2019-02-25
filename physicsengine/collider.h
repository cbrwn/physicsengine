/* =================================
 *  Collider
 *  Base collider class for all physics colliders
 * ================================= */
#pragma once

#include <vector>
#include <glm/glm.hpp>

class PhysicsBody;

enum ColliderType
{
	COLLIDER_AABB,
	COLLIDER_SPHERE,
	COLLIDER_CYLINDER,
	COLLIDER_CONE
};

struct Collider
{
	ColliderType type;
	// keep track of the body it's attached to
	PhysicsBody* body;

	// all the information needed for SAT collision
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;

	// ability to draw that information for debug 
	void drawPoints();
	void drawNormals();
};
