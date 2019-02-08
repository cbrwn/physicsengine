/* =================================
 *  Lava
 *  Beautiful red square that destroys anything that touches it
 * ================================= */
#include "lava.h"

#include <color.h>
#include <gmath.h>

#include "shapes.h"
#include "collideraabb.h"
#include "missilesmoke.h"

Lava::Lava(Vector3 pos, Vector3 extents)
	: PhysicsActor(pos), m_extents(extents)
{
	// set up collider
	auto col = new ColliderAABB(extents);
	m_body->setZone(true);
	m_body->setStatic(true);
	m_body->setCollider(col);
	m_body->setCollideCallback([this](auto a) { return collided(a); });

	m_colourTimer = 0.0f;
	m_particleTimer = 0.0f;
	m_colourDirection = true;

	m_endColour = Vector4(0.5f, 0.1f, 0, 1);
	m_startColour = Vector4(0.8f, 0.1f, 0, 1);
}

void Lava::update(float delta)
{
	m_particleTimer += delta;

	// change colour timer based on whether or not we're going to or from the
	// end colour
	if (m_colourDirection)
		m_colourTimer += delta;
	else
		m_colourTimer -= delta;

	// make sure to check if we need to switch directions
	if (m_colourTimer >= GLOW_SPEED)
		m_colourDirection = false;
	if (m_colourTimer <= 0.0f)
		m_colourDirection = true;

	// spawn particles every once in a while
	if (m_particleTimer > 0.01f)
	{
		m_particleTimer = 0.0f;
		spawnParticles();
	}
}

void Lava::draw()
{
	// get the colour at this point in between the start and end colours
	float colTime = m_colourTimer / GLOW_SPEED;
	Vector4 col = Vector4::lerpBetween(m_startColour, m_endColour, colTime);

	drawBox(m_body->getPosition(), m_extents, col, nullptr);
}

void Lava::collided(PhysicsBody* bod)
{
	PhysicsActor* act = m_world->getActorWithBody(bod);
	if (act)
	{
		// destroy the object regardless of what it is
		act->setEnabled(false);
		makeSmokePuff(act->getPosition());
	}
}

void Lava::spawnParticles()
{
	// we want to spawn the particle at the same colour of the lava itself
	float colTime = m_colourTimer / GLOW_SPEED;
	// make the start colour a little brighter so it doesn't completely
	// blend in to the lava
	const float addAmt = 0.3f;
	Vector4 startCol = m_startColour + Vector4(addAmt, addAmt / 2.0f, 
		addAmt / 4.0f, 0);

	Vector4 col = Vector4::lerpBetween(startCol, m_endColour, colTime);

	// spawn 3 particles each time
	for (int i = 0; i < 3; ++i)
	{
		auto smoke = m_world->getPool()->getMissileSmoke();

		// set the position to somewhere inside the lava
		const float offsetFactor = 0.8f;
		Vector3 pos = m_body->getPosition();
		pos.x += randBetween(-m_extents.x, m_extents.x) * offsetFactor;
		pos.z += randBetween(-m_extents.z, m_extents.z) * offsetFactor;
		pos.y += randBetween(0.0f, m_extents.y / 2.0f);

		// the particle should move upwards
		Vector3 dir(0, 3, 0);
		// but let's give it some x/z velocity too
		const float dirVar = 1.0f;
		dir.x += randBetween(-dirVar, dirVar);
		dir.z += randBetween(-dirVar, dirVar);

		// slightly randomize the size
		float size = randBetween(1.0f, 1.5f);

		smoke->reset(pos, dir, size, col);
	}
}

void Lava::makeSmokePuff(Vector3 const& spawnPos)
{
	// make 50 particles for each puff
	for (int i = 0; i < 50; ++i)
	{
		Vector3 pos = spawnPos;
		// vary the starting position a little bit
		const float posVariance = 1.0f;
		pos.x += randBetween(-posVariance, posVariance);
		pos.y += randBetween(-posVariance, 0.0f);
		pos.z += randBetween(-posVariance, posVariance);

		auto smoke = m_world->getPool()->getMissileSmoke();

		Vector3 vel(0, 4, 0);
		// vary the velocity so all the particles don't move exactly the same
		vel.y += randBetween(-2.0f, 2.0f);

		// start it at size 2 which is pretty large
		smoke->reset(pos, vel, 2.0f);
	}
}