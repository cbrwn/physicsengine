/* =================================
 *  Tank
 *  The missile-shooting enemies
 * ================================= */
#include "tank.h"

#include <color.h>
#include <gmath.h>

#include "world.h"
#include "shapes.h"
#include "missile.h"
#include "objectpool.h"
#include "tankbarrel.h"
#include "collideraabb.h"

Tank::Tank(Vector3 pos)
	: PhysicsActor(pos), m_length(1.3f)
{
	m_class = ACTORCLASS_TANK;

	auto collider = new ColliderAABB(Vector3(m_length, 0.5f, 1));
	m_body->setCollider(collider);

	// make our barrel actor
	m_barrel = new TankBarrel(Vector3(0, 0.6f, 0));
	addChild(m_barrel);

	m_target = nullptr;
	// start this off randomly so the tanks don't always shoot at once
	m_shotTimer = randBetween(-3.0f, 2.0f);

	m_destroyCallback = nullptr;
}

void Tank::update(float delta)
{
	PhysicsActor::update(delta);

	// the body will be enabled when picked up by the UFO, and getting shot
	// by a tank you're abducting feels bad
	// so let's not update our shoot time while we're in the beam
	if (m_body->isEnabled())
		m_shotTimer += delta;

	// shoot after ~3 seconds (depends on the starting value of shotTimer)
	if (m_shotTimer >= 3.0f)
	{
		// make it random again so it's not super predictable
		m_shotTimer = randBetween(-3.0f, 2.0f);

		// shoot a missile!
		Missile* m = m_world->getPool()->getMissile();
		// (if there's one in the object pool to shoot)
		if (m)
			m->reset(getPosition(), m_target);
	}

}

void Tank::draw()
{
	// this actor just draws the base of the tank and the TankBarrel child
	// draws the rest
	Vector4 colour = intToVector(0x2d661fff);
	drawBox(Vector3(), Vector3(m_length, 0.5f, 1),
		colour, &m_globalTransform);
}

void Tank::setEnabled(bool e)
{
	PhysicsActor::setEnabled(e);

	// call our callback if this is set to false
	if (!e && m_destroyCallback)
		m_destroyCallback();
}

void Tank::setTarget(PhysicsActor* a)
{
	m_target = a;
	// pass this target on to the barrel so it knows where to aim
	m_barrel->setTarget(a);
}

void Tank::setWorld(World* w)
{
	Actor::setWorld(w);
	// make sure our barrel's in the same world
	w->addActor(m_barrel);
}

void Tank::setDestroyCallback(std::function<void()> func)
{
	m_destroyCallback = func;
}
