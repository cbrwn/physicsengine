/* =================================
 *	Box
 *  A generic box shape with physics
 * ================================= */
#include "box.h"

#include <color.h>

#include "shapes.h"
#include "physics.h"
#include "collideraabb.h"

Box::Box(glm::vec3 pos, glm::vec3 size, unsigned int col, bool lines)
	: PhysicsActor(pos), m_drawLines(lines), m_color(col), m_size(size)
{
	auto collider = new ColliderAABB(m_size);
	m_body->setCollider(collider);
	m_body->setStatic(true);
	m_body->setMass(1.0f);

	m_growTime = 0.0f;
}

void Box::update(float delta)
{
	PhysicsActor::update(delta);

	if (m_growTime < BOX_GROW_TIME) {
		m_growTime += delta;
		if (m_growTime > BOX_GROW_TIME)
			m_growTime = BOX_GROW_TIME;
	}

	if (getPosition().y <= -5.0f) {
		setEnabled(false);
	}
}

void Box::draw()
{
	glm::vec4 c = intToVector(m_color);

	auto easelambda = [](float t) {
		return -1.0f * powf(4.0f, -8.0f*t) * sinf((t*6.0f - 1.0f)*(2.0f*3.14159f) / 2.0f) + 1.0f;
	};

	float sz = easelambda(m_growTime / BOX_GROW_TIME);

	drawBox(glm::vec3(0, 0, 0), m_size*sz, c, &m_globalTransform, m_drawLines);
}
