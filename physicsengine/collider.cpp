/* =================================
 *  Collider
 *  Base collider class for all physics colliders
 * ================================= */
#include "collider.h"

#include <Gizmos.h>
#include <glm/glm.hpp>

#include "shapes.h"
#include "physicsbody.h"

// draws a small sphere at each point of the collider
void Collider::drawPoints()
{
	for (int i = 0; i < points.size(); ++i)
	{
		glm::vec3 p = points[i];
		glm::mat4 m;
		m[3] = glm::vec4(p, m[3].w);

		// translate the point with the body
		glm::mat4 transformed = body->getTransformMatrix() * m;

		// draw a nice red sphere
		// (which is sometimes transparent when there's a lot of gizmos)
		drawSphere(glm::vec3(), 0.05f, glm::vec4(1, 0, 0, 1), &transformed,
			8, 8, true, glm::vec4(1, 0, 0, 1));
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
	glm::vec3 p1 = body->getPosition();

	for (int i = 0; i < normals.size(); ++i)
	{
		glm::mat4 m;
		// extend normals out 1.5 units
		m[3] = glm::vec4(normals[i] * 1.5f, m[3].w);

		// translate it with its body
		m = body->getTransformMatrix() * m;

		glm::vec3 p2 = m[3];

		// draw a nice blue line
		aie::Gizmos::addLine(p1, p2, glm::vec4(0, 0, 1, 1));
	}
}
