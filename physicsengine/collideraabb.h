/* =================================
 *  ColliderAABB
 *  Simple box collider
 * ================================= */
#pragma once

#include "collider.h"

struct ColliderAABB : public Collider
{
	ColliderAABB(glm::vec3 const& ext);

	// how far the box extends from the center on each axis
	glm::vec3 extents;

	// ability to test collision with a ray
	bool rayTest(glm::vec3 start, glm::vec3 dir, float& outDist);
};