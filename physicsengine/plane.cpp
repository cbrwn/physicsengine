#include "plane.h"

#include "shapes.h"

Plane::Plane(Vector3 topLeft, Vector3 topRight, 
	Vector3 bottomRight, Vector3 bottomLeft, Vector4 col)
	: Actor(Vector3())
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
