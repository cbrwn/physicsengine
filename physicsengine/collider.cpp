/* =================================
 *  Collider
 *  Base collider class for all physics colliders
 * ================================= */
#include "collider.h"

#include <Gizmos.h>
#include <glm/glm.hpp>

#include "util.h"
#include "shapes.h"
#include "physicsbody.h"

// draws a small sphere at each point of the collider
void Collider::drawPoints()
{
	for (int i = 0; i < points.getCount(); ++i)
	{
		Vector3 p = points[i];
		Matrix4 m;
		m.setPosition(p);

		// translate the point with the body
		Matrix4 transformed = body->getTransformMatrix() * m;

		// draw a nice red sphere
		// (which is sometimes transparent when there's a lot of gizmos)
		drawSphere(Vector3(), 0.05f, Vector4(1, 0, 0, 1), &transformed,
			8, 8, true, Vector4(1, 0, 0, 1));
	}
}

// draws a line from the center of the collider in the direction of each
// normal
void Collider::drawNormals()
{
	// can't draw at the body's position if there's no body
	if (!body)
		return;

	// grab the center of the body to use as the origin point for lines
	glm::vec3 p1 = toVec3(body->getPosition());

	for (int i = 0; i < normals.getCount(); ++i)
	{
		Matrix4 m;
		// extend normals out 1.5 units
		m.setPosition(normals[i] * 1.5f);

		// translate it with its body
		m = body->getTransformMatrix() * m;

		glm::vec3 p2 = toVec3(m.getPosition());

		// draw a nice blue line
		aie::Gizmos::addLine(p1, p2, glm::vec4(0, 0, 1, 1));
	}
}
