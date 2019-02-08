/* =================================
 *  ColliderSphere
 *  Simple sphere collider
 * ================================= */
#pragma once

#include "collider.h"
#include <vector3.h>

struct ColliderSphere : public Collider
{
	ColliderSphere(Vector3 const& c, float r, int rows = 8, int cols = 8);

	Vector3 center;
	float radius;
};
