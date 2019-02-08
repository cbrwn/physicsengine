/* =================================
 *  Beam
 *  The tractor beam under the UFO which can pick things up
 * ================================= */
#include "beam.h"

#include <Input.h>

#include "player.h"
#include "shapes.h"
#include "collidercone.h"
#include "collidercylinder.h"

Beam::Beam(Vector3 pos, Player* plr)
	: PhysicsActor(pos), m_player(plr)
{
	// default values for collider
	m_height = 10.0f;
	m_radius = 1.0f;
	m_segments = 8;

	// initial size of the collider doesn't really matter, since it's set
	// in update
	m_collider = new ColliderCone(m_height, m_radius*10.0f, m_segments);

	// set body properties
	m_body->setStatic(true);
	m_body->setCollider(m_collider);
	// make it a zone - doesn't collide but keeps track of things inside it
	m_body->setZone(true);

	// start our beamed object (object we're picking up) at nullptr
	m_beamed = nullptr;

	m_wobbleTime = 0.0f;
}

void Beam::update(float delta)
{
	using namespace aie;

	m_height = m_player->getGlobalTransform().getPosition().y;

	// subtract half of the height so the position is the top of the cone
	Vector3 pos = m_globalTransform.getPosition();
	pos -= Vector3(0, m_height / 2.0f, 0);

	m_body->setPosition(pos);

	Input* input = Input::getInstance();
	if (input->wasKeyPressed(INPUT_KEY_SPACE))
	{
		// either drop or pick up an actor when space is pressed
		if (m_children.getCount() == 0)
			pickupActor(findActor());
		else
			dropActor();
	}

	if (m_beamed)
	{
		// increase timer used to make it bob up and down
		m_wobbleTime += delta;

		// we want to rotate around its up axis
		Vector3 rotateVec = m_beamed->getGlobalTransform().getUp() * delta;

		// make our rotate matrix
		Matrix4 rotate;
		rotate.setRotate(rotateVec);

		// apply the matrix - getLocalTransform returns a reference which is
		//	why it looks so ugly
		m_beamed->getLocalTransform() = m_beamed->getLocalTransform() * rotate;

		// get the position that it should smoothly move to
		Vector3 targetLocal(0, -4 - sinf(m_wobbleTime), 0);

		// smoothly move between its current position and the target position
		Vector3 beamLocal = m_beamed->getLocalTransform().getPosition();
		Vector3 newLocal = beamLocal - (beamLocal - targetLocal) * 1.0f * delta;

		// update the position
		m_beamed->setLocalPosition(newLocal);

		// set the physics thingymajig thing doohickey whatsit
		m_beamed->getBody()->setPosition(m_beamed->getPosition());

		// drop if the actor disappears
		if (!m_beamed->isEnabled())
			dropActor();
	}

	// change our radius based on height
	m_drawRadius = m_height / 4.0f;
	// but don't let it get too small
	if (m_drawRadius < 1)
		m_drawRadius = 1;

	m_collider->updateShape(m_height, m_radius * m_drawRadius, 8);
}

void Beam::draw()
{
	drawCone(m_body->getPosition(), 0, m_radius * m_drawRadius, m_height, 128,
		Vector4(0, 1.0f, 0, 0.4f), nullptr, false);
}

void Beam::setEnabled(bool e)
{
	// drop our beamed 
	if (!e && m_beamed)
		dropActor();

	PhysicsActor::setEnabled(e);
}

PhysicsActor* Beam::findActor()
{
	// get the ship's position so we can get the closest object to it
	Vector3 thisPos = m_parent->getPosition();

	// grab all bodies inside the beam
	DArray<PhysicsBody*> colliding = m_body->getCollidingBodies();

	// keep track of the closest body
	PhysicsActor* closest = nullptr;
	float closestDistance = INFINITY;
	for (int i = 0; i < colliding.getCount(); ++i)
	{
		PhysicsBody* b = colliding[i];
		// ignore static objects so we're not picking up the floor
		if (b->isStatic())
			continue;

		PhysicsActor* a = m_world->getActorWithBody(b);
		if (a == m_parent)
			continue;
		// keep track of it if it's closer
		if (thisPos.distanceToSquared(a->getPosition()) < closestDistance)
			closest = a;
	}

	// return its actor - possible to return nullptr
	return closest;
}

void Beam::pickupActor(PhysicsActor* a)
{
	// findActor could return nullptr, so check for that first
	if (!a)
		return;

	// attach to us, becomes a third level of our matrix hierarchy 
	// player -> beam -> object in the beam
	addChild(a);
	m_beamed = a;

	// change the local position so that its global position stays the same
	a->setLocalPosition(a->getPosition() - getPosition());
	// disable physics so it's not interfering with our positions
	a->getBody()->setEnabled(false);
}

void Beam::dropActor()
{
	// re-enable physics
	m_beamed->getBody()->setEnabled(true);
	// make sure the body's awake so it doesn't get stuck in the air
	m_beamed->getBody()->wakeUp();
	// stick the actor's transform into its physics body
	m_beamed->getBody()->setTransform(m_beamed->getGlobalTransform());

	// get the velocity from our parent
	Vector3 parentVelocity = 
		((PhysicsActor*)m_parent)->getBody()->getVelocity();

	// set velocity to parent's velocity so we can throw it :)
	m_beamed->getBody()->setVelocity(parentVelocity);

	// detach from us
	m_beamed->setParent(nullptr);
	m_children.clear();

	// make sure we can tell we're not holding anything
	m_beamed = nullptr;
}
