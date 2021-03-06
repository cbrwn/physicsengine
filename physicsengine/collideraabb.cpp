/* =================================
 *  ColliderAABB
 *  Simple box collider
 * ================================= */
#include "collideraabb.h"

#include <matrix4.h>

#include "util.h"
#include "physicsbody.h"

ColliderAABB::ColliderAABB(Vector3 const& ext)
{
	extents = ext;
	type = COLLIDER_AABB;

	// grab all the corners (super ugly, sorry)
	for (int i = -1; i <= 1; i += 2)
		for (int j = -1; j <= 1; j += 2)
			for (int k = -1; k <= 1; k += 2)
				points.add(Vector3(i*ext.x, j*ext.y, k*ext.z));

	// the normals for a cube are easy, just point in each direction
	for (int i = -1; i <= 1; i += 2) {
		normals.add(Vector3(i, 0, 0));
		normals.add(Vector3(0, i, 0));
		normals.add(Vector3(0, 0, i));
	}
}

// checks collision with a ray
// returns whether or not the ray intersects, and also spits out the distance
// to the nearest face in outDist
bool ColliderAABB::rayTest(Vector3 start, Vector3 dir, float& outDist)
{
	Matrix4 m = body->getTransformMatrix();
	Vector3 pos = m.getPosition();

	Vector3 startDif = pos - start;

	// these look wrong, but tMin should be the FARTHEST near face
	float tMin = -INFINITY;
	// and tMax should be the CLOSEST far face
	float tMax = INFINITY;

	// test each of the three axes
	for (int i = 0; i < 3; ++i)
	{
		// easily grab the right/up/forward vectors by casting the column of
		// the matrix to a Vector3, dropping the w value
		Vector3 axis = (Vector3)m[i];

		// dot this axis direction with the start difference
		float e = axis.dot(startDif);
		// and the ray direction
		float f = axis.dot(dir);

		// (badly) make sure we don't divide by 0
		if (fabsf(f) < 0.001f)
			continue;

		// subtract e from the distance from the middle on this axis to get
		// the near face distance (assuming the AABB is in front of the ray)
		float t1 = (e - extents[i]) / f;
		// and the far face by adding this axis' extent
		float t2 = (e + extents[i]) / f;

		// t2 SHOULD be the largest, but might not if we intersected
		// behind the start of the ray
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		// keep track of the closest far face
		if (t2 < tMax)
			tMax = t2;
		// and the farthest near face
		if (t1 > tMin)
			tMin = t1;

		// if the far face is ever closer than the near face, there's no way
		// it could be intersecting
		if (tMax < tMin)
			return false;
	}

	// tMin will be the smallest near face to the ray
	// which I think can be negative, meaning it's behind the ray
	outDist = tMin;

	return true;
}
