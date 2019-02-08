/* =================================
 *  Explosion
 *  Just a zone which collides with things when something explodes
 *  Also does some visual effects like spawning particles and a quick flash
 * ================================= */
#include "explosion.h"

#include <gmath.h>

#include "shapes.h"
#include "collideraabb.h"
#include "missilesmoke.h"

Explosion::Explosion()
	: PhysicsActor(Vector3(0, 0, 0))
{
	// the size of the collider, or the range which will hit things
	const float colSize = 5.0f;
	auto col = new ColliderAABB(Vector3(colSize, colSize, colSize));
	m_body->setCollider(col);
	m_body->setCollideCallback([this](auto a) { return collided(a); });

	m_body->setZone(true);
	m_body->setUseGravity(false);

	setEnabled(false);
}

void Explosion::update(float delta)
{
	PhysicsActor::update(delta);

	// amount to change the size by
	float amt = 100.0f;
	// make that negative if we're not supposed to grow
	if (!m_grow)
		amt = -100.0f;
	m_size += delta * amt;

	// once it gets big enough it can start to shrink
	if (m_size > 5.0f)
		m_grow = false;

	// disable once it has disappeared
	if (m_size <= 0.0f)
		setEnabled(false);
}

void Explosion::draw()
{
	drawSphere(Vector3(), m_size, Vector4(1, 1, 1, 0.3f), &m_globalTransform,
		8, 8, false);
}

void Explosion::reset(Vector3 pos)
{
	// maximum distance from the center that a particle can be spawned
	const float offset = 0.4f;
	// make a bunch of smoke particles
	for (int i = 0; i < 40; ++i)
	{
		// generate position
		Vector3 startPos = pos;
		startPos.x += randBetween(-offset, offset);
		startPos.y += randBetween(-offset, offset);
		startPos.z += randBetween(-offset, offset);

		// shoot it in any direction
		Vector3 dir;
		dir.x = randBetween(-1.0f, 1.0f);
		dir.y = randBetween(-1.0f, 1.0f);
		dir.z = randBetween(-1.0f, 1.0f);

		MissileSmoke* m = m_world->getPool()->getMissileSmoke();
		// give it a size of 1 which will hang around for a while
		m->reset(startPos, dir, 1.0f);
	}

	// starts by pulsing out
	m_grow = true;
	m_size = 1.0f;
	m_body->setPosition(pos);
	setEnabled(true);
}

void Explosion::collided(PhysicsBody* bod)
{
	PhysicsActor* act = m_world->getActorWithBody(bod);
	if (!act)
		return;

	// right now there's only support for blowing up tanks
	if (act->getClass() != ACTORCLASS_TANK)
		return;
	act->setEnabled(false);
}
