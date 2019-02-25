/* =================================
 *  ColliderSphere
 *  Simple sphere collider
 * ================================= */
#pragma once

#include "collider.h"

struct ColliderSphere : public Collider
{
	ColliderSphere(glm::vec3 const& c, float r, int rows = 8, int cols = 8);

	glm::vec3 center;
	float radius;
};
