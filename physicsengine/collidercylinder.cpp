/* =================================
 *  ColliderCylinder
 *  Simple cylinder collider
 * ================================= */
#include "collidercylinder.h"

#include <math.h>
#include <gmath.h>

ColliderCylinder::ColliderCylinder(float height, float radius, int segments)
{
	type = COLLIDER_CYLINDER;

	// how many radians each segment takes up
	float segmentSize = (2.0f * PI) / segments;

	// shorten height/2 because that's how far each point will be away from
	// the center vertically
	float h = height / 2.0f;

	// push_backthe normals for the flat top and bottom parts
	normals.push_back(glm::vec3(0, 1, 0));
	normals.push_back(glm::vec3(0, -1, 0));

	// this is straight up stolen from the bootstrap's Gizmos class' cylinder
	for (int i = 0; i < segments; ++i)
	{
		glm::vec3 top1(
			0.0f, 
			h, 
			0.0f);
		glm::vec3 top2(
			sinf(i * segmentSize) * radius, 
			h, 
			cosf(i*segmentSize)*radius);
		glm::vec3 top3(
			sinf((i + 1) * segmentSize) * radius, 
			h, 
			cosf((i + 1)*segmentSize)*radius);

		glm::vec3 bottom1(
			0.0f, 
			-h, 
			0.0f);
		glm::vec3 bottom2(
			sinf(i * segmentSize) * radius, 
			-h, 
			cosf(i*segmentSize)*radius);
		glm::vec3 bottom3(
			sinf((i + 1) * segmentSize) * radius, 
			-h, 
			cosf((i + 1)*segmentSize)*radius);

		points.push_back(top2);
		points.push_back(top3);
		points.push_back(bottom2);
		points.push_back(bottom3);

		// I don't think we need these points in the middle of the top/bottom
		// faces - but I haven't tested it so we'll keep this here for now
		//points.push_back(top1);
		//points.push_back(bottom1);

		// but here I calculate the normals which is super easy

		// i*segmentSize is already the angle of this segment, but we want to
		// get the angle of the middle of the segment - so just push_backhalf
		float normalAngle = (i * segmentSize) + segmentSize / 2.0f;

		// then sin/cos that angle and we have our super simple normals :)
		normals.push_back(glm::normalize(glm::vec3(sinf(normalAngle), 0.0f, 
			cosf(normalAngle))));
	}
}
