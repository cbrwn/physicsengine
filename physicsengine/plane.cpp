#include "plane.h"

#include "shapes.h"

Plane::Plane(glm::vec3 topLeft, glm::vec3 topRight, 
	glm::vec3 bottomRight, glm::vec3 bottomLeft, glm::vec4 col)
	: Actor(glm::vec3())
{
	m_points[0] = topLeft;
	m_points[1] = topRight;
	m_points[2] = bottomRight;
	m_points[3] = bottomLeft;

	m_colour = col;
}

void Plane::update(float delta) { }

void Plane::draw()
{
	drawPlane(m_points[0], m_points[1], m_points[2], m_points[3], m_colour);
}
