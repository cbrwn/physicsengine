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

	// add the normals for the flat top and bottom parts
	normals.add(Vector3(0, 1, 0));
	normals.add(Vector3(0, -1, 0));

	// this is straight up stolen from the bootstrap's Gizmos class' cylinder
	for (int i = 0; i < segments; ++i)
	{
		Vector3 top1(
			0.0f, 
			h, 
			0.0f);
		Vector3 top2(
			sinf(i * segmentSize) * radius, 
			h, 
			cosf(i*segmentSize)*radius);
		Vector3 top3(
			sinf((i + 1) * segmentSize) * radius, 
			h, 
			cosf((i + 1)*segmentSize)*radius);

		Vector3 bottom1(
			0.0f, 
			-h, 
			0.0f);
		Vector3 bottom2(
			sinf(i * segmentSize) * radius, 
			-h, 
			cosf(i*segmentSize)*radius);
		Vector3 bottom3(
			sinf((i + 1) * segmentSize) * radius, 
			-h, 
			cosf((i + 1)*segmentSize)*radius);

		points.add(top2);
		points.add(top3);
		points.add(bottom2);
		points.add(bottom3);

		// I don't think we need these points in the middle of the top/bottom
		// faces - but I haven't tested it so we'll keep this here for now
		//points.add(top1);
		//points.add(bottom1);

		// but here I calculate the normals which is super easy

		// i*segmentSize is already the angle of this segment, but we want to
		// get the angle of the middle of the segment - so just add half
		float normalAngle = (i * segmentSize) + segmentSize / 2.0f;

		// then sin/cos that angle and we have our super simple normals :)
		normals.add(Vector3(sinf(normalAngle), 0, 
			cosf(normalAngle)).normalised());
	}
}
