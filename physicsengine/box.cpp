/* =================================
 *	Box 
 *  A generic box shape with physics
 * ================================= */
#include "box.h"

#include <color.h>

#include "util.h"
#include "shapes.h"
#include "physics.h"
#include "collideraabb.h"

Box::Box(Vector3 pos, Vector3 size, unsigned int col, bool lines)
	: PhysicsActor(pos), m_drawLines(lines), m_color(col), m_size(size)
{
	auto collider = new ColliderAABB(m_size);
	m_body->setCollider(collider);
	m_body->setStatic(true);
	m_body->setMass(1.0f);
}

void Box::update(float delta)
{
	PhysicsActor::update(delta);
}

void Box::draw()
{
	Vector4 c = intToVector(m_color);

	drawBox(Vector3(0, 0, 0), m_size, c, &m_globalTransform, m_drawLines);
}
