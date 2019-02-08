/* =================================
 *  Collider
 *  Base collider class for all physics colliders
 * ================================= */
#pragma once

#include <darray.h>
#include <vector3.h>

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
	DArray<Vector3> points;
	DArray<Vector3> normals;

	// ability to draw that information for debug 
	void drawPoints();
	void drawNormals();
};
