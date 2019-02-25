/* =================================
 *  Physics Actor
 *  A specific kind of Actor which has a PhysicsBody
 * ================================= */
#include "physicsactor.h"

#include <cassert> // for assert()

#include "shapes.h"

PhysicsActor::PhysicsActor(glm::vec3 pos)
	: Actor(pos)
{
	m_type = ACTORTYPE_PHYSICS;
	m_body = new PhysicsBody();
	m_body->setPosition(pos);

	// add our body to the physics world
	PhysicsManager::getInstance()->addPhysicsBody(m_body);
}

PhysicsActor::~PhysicsActor()
{
	delete m_body;
}

void PhysicsActor::update(float delta)
{
	// make sure we have a body and that body has a collider
	assert(m_body);
	assert(m_body->getCollider());

	if (m_body->isEnabled())
	{
		// update the body's physics
		m_body->update(delta);

		// apply the body's transform to our actor transform
		m_localTransform = m_body->getTransformMatrix();
		updateTransform();
	}
}

void PhysicsActor::draw()
{
	// draw the broad bounding box by default
	glm::vec3 extents = m_body->getBroadExtents();

	drawBox(glm::vec3(), extents, glm::vec4(1, 1, 1, 1), &m_globalTransform);
}

void PhysicsActor::setEnabled(bool e)
{
	// enable/disable the physics body
	m_body->setEnabled(e);
	Actor::setEnabled(e);
}

void PhysicsActor::reset()
{
	setEnabled(true);
}
