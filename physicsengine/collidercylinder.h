/* =================================
 *  ColliderCylinder
 *  Simple cylinder collider
 * ================================= */
#pragma once

#include "collider.h"

struct ColliderCylinder : public Collider
{
	ColliderCylinder(float height, float radius, int segments);
};