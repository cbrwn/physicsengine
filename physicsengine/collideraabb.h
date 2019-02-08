/* =================================
 *  ColliderAABB
 *  Simple box collider
 * ================================= */
#pragma once

#include "collider.h"
#include <vector3.h>

struct ColliderAABB : public Collider
{
	ColliderAABB(Vector3 const& ext);

	// how far the box extends from the center on each axis
	Vector3 extents;

	// ability to test collision with a ray
	bool rayTest(Vector3 start, Vector3 dir, float& outDist);
};