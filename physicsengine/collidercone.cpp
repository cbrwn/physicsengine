/* =================================
 *  ColliderCone
 *  Simple cone collider
 * ================================= */
#include "collidercone.h"

#include <math.h>
#include <gmath.h>

ColliderCone::ColliderCone(float height, float radius, int segments)
{
	type = COLLIDER_CONE;
	updateShape(height, radius, segments);
}

void ColliderCone::updateShape(float height, float radius, int segments)
{
	normals.clear();
	points.clear();

	// how many radians each segment takes up in the base of the cone
	float segmentSize = (2.0f * PI) / segments;

	// shorten height/2 because the bottom points will only be that far away
	// from the center
	float h = height / 2.0f;

	// cones will always have a normal straight down, the top of the cone
	// will be a point so I don't think a normal is needed there
	normals.push_back(glm::vec3(0, -1, 0));

	// since we know the top will always just be a point, we can add that now
	points.push_back(glm::vec3(0.0f, h, 0.0f));

	for (int i = 0; i < segments; ++i)
	{
		glm::vec3 bottom3(sinf((i + 1) * segmentSize) * radius, -h, 
			cosf((i + 1)*segmentSize)*radius);

		points.push_back(bottom3);

		// make a new vector that's half way between this segment and the next
		// so we can base our normal off of that, instead of the edge
		glm::vec3 normalVector(sinf((i + 1.5f) * segmentSize) * radius, -h, 
			cosf((i + 1.5f)*segmentSize)*radius);

		// use the top point and our in-between position to get a third
		// vector perpendicular to them
		glm::vec3 perp = glm::cross(points[0], normalVector);

		// then use these three vectors to grab the normal
		glm::vec3 normal = glm::normalize(glm::cross(perp, points[0] - normalVector));
		normals.push_back(normal);
	}
}

