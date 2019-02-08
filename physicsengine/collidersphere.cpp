/* =================================
 *  ColliderSphere
 *  Simple sphere collider
 * ================================= */
#include "collidersphere.h"

#include <glm/glm.hpp>
#include <gmath.h>

ColliderSphere::ColliderSphere(Vector3 const& c, float r, int rows, int cols)
{
	center = c;
	radius = r;
	type = COLLIDER_SPHERE;

	// copied from Gizmos::addsphere
	using namespace glm;

	const float longMin = 0.0f;
	const float longMax = 360.0f;
	const float latMin = -90.0f;
	const float latMax = 90;

	const float DEG2RAD = PI / 180.0f;

	//Lets put everything in radians first
	const float latitiudinalRange = (latMax - latMin) * DEG2RAD;
	const float longitudinalRange = (longMax - longMin) * DEG2RAD;

	float inverseRadius = 1 / radius;

	// invert these first as the multiply is slightly quicker
	float invColumns = 1.0f / cols;
	float invRows = 1.0f / rows;

	for (int row = 0; row <= rows; ++row)
	{
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis = ratioAroundXAxis * latitiudinalRange +
			(latMin * DEG2RAD);
		float y = radius * sin(radiansAboutXAxis);
		float z = radius * cos(radiansAboutXAxis);

		for (int col = 0; col <= cols; ++col)
		{
			float ratioAroundYAxis = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange +
				(longMin * DEG2RAD);
			vec3 v4Point(-z * sinf(theta), y, -z * cosf(theta));
			vec3 v4Normal(inverseRadius * v4Point.x,
				inverseRadius * v4Point.y, inverseRadius * v4Point.z);

			points.add(Vector3(v4Point.x, v4Point.y, v4Point.z));
			normals.add(Vector3(v4Normal.x, v4Normal.y, v4Normal.z));
		}
	}
}
