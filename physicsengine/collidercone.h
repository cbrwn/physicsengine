/* =================================
 *  ColliderCone
 *  Simple cone collider
 * ================================= */
#pragma once

#include "collider.h"

struct ColliderCone : public Collider
{
	ColliderCone(float height, float radius, int segments);

	// re-sets the points and normals of a new shape
	// implemented because we needed to dynamically change the UFO beam's
	// shape
	void updateShape(float height, float radius, int segments);
};
