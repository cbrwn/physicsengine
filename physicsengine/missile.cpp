/* =================================
 *  Missile
 *  Target-seeking missile shot by tanks
 * ================================= */
#include "missile.h"

#include <color.h>
#include <gmath.h>

#include "world.h"
#include "player.h"
#include "shapes.h"
#include "explosion.h"
#include "objectpool.h"
#include "collideraabb.h"
#include "missilesmoke.h"

Missile::Missile()
	: PhysicsActor(Vector3()), m_radius(0.2f), m_height(2.0f)
{
	auto collider = new ColliderAABB(Vector3(m_radius,
		m_height / 2.0f, m_radius));
	m_body->setCollider(collider);
	m_body->setUseGravity(false);

	// make it call collided() when it collides
	m_body->setCollideCallback([this](auto c) { return collided(c); });

	m_lifeTimer = 0.0f;
	m_smokeTimer = 0.0f;

	m_target = nullptr;
	setEnabled(false);
}

void Missile::update(float delta)
{
	m_lifeTimer += delta;
	m_smokeTimer += delta;

	// when the body is disabled, this is in the beam
	// make it so the missile falls when it's released
	if (!m_body->isEnabled() && !m_falling)
	{
		m_falling = true;
		m_body->setUseGravity(true);
		m_target = nullptr;
	}

	if (m_target)
	{
		// push towards the target
		Vector3 dif = m_target->getPosition() - m_body->getPosition();
		dif.normalise();
		const float pushAmount = 40.0f;
		m_body->addForce(dif * pushAmount * delta);
	}

	PhysicsActor::update(delta);

	if (m_target)
	{
		// grab the position after the physics update
		Vector3 movedPos = m_body->getPosition();

		// make rotation matrix to face the target
		Matrix4 rotationMatrix = Matrix4::lookAt(movedPos,
			m_target->getPosition()).inverse();
		// grab the position from the body
		rotationMatrix.setPosition(movedPos);

		// make a matrix to turn it 90 degrees so it faces the target
		Matrix4 turnMatrix;
		turnMatrix.setRotateX(PI / 2.0f);
		// apply it
		rotationMatrix = rotationMatrix * turnMatrix;

		// apply it to the body so it changes the collider too
		m_body->setTransform(rotationMatrix);
	}

	// explode if we're active for too long
	if (m_lifeTimer > 10.0f)
		explode();

	if (m_smokeTimer > 0.01f && !m_falling)
	{
		// the up direction points towards the top of the missile
		Vector3 dir = m_body->getTransformMatrix().getUp();

		// distance to spawn the smoke from the middle,
		// half the height so it's coming out of the back
		float offset = m_height / 2.0f;
		Vector3 spawnPos = getPosition() + (dir * offset);

		// make our smoke
		MissileSmoke* m = m_world->getPool()->getMissileSmoke();
		m->reset(spawnPos, dir);
		m_smokeTimer = 0.0f;
	}
}

void Missile::draw()
{
	// grab a copy of our transform so it's not so horribly long
	Matrix4 transform = m_body->getTransformMatrix();

	Vector4 colour = intToVector(0x333333ff);
	drawCylinder(Vector3(), m_radius, m_height, 8, colour,
		&transform);

	// draw fins, using two "cones" which turn into triangles when given
	// 2 segments

	// move fins down so it's attached to the bottom
	Matrix4 finTranslate;
	finTranslate.setPosition(0, 0.5f, 0);
	Matrix4 finTransform = transform * finTranslate;

	const float finHeight = 1.0f;
	drawCone(Vector3(), 0.0f, 0.5f, -finHeight, 2, Vector4(1, 1, 1, 1), &finTransform);

	// draw the other two fins by rotating another cone 90 degrees
	Matrix4 finRotate;
	finRotate.setRotateY(PI / 2.0f);
	Matrix4 otherFin = finTransform * finRotate;
	drawCone(Vector3(), 0.0f, 0.5f, -finHeight, 2, Vector4(1, 1, 1, 1), &otherFin);
}

void Missile::reset(Vector3 pos, PhysicsActor* target)
{
	m_target = target;
	m_falling = false;
	m_lifeTimer = 0.0f;

	// move it slightly towards its target, so it doesn't spawn inside the
	// object that's spawning it
	Vector3 spawnPos = pos +
		((target->getPosition() - pos).normalised() * 3.0f);

	m_body->setUseGravity(false);
	m_body->setPosition(spawnPos);
	m_body->setVelocity(Vector3());

	setEnabled(true);
}

void Missile::collided(PhysicsBody* other)
{
	// make sure it won't explode before the player even knows it exists
	if (m_lifeTimer < 0.2f)
		return;

	if (other->isZone())
		return;

	explode();

	// check if it collided with the player so we can do damage to it
	// we only do damage to the player if the missile itself collides with
	// the player, because it feels nicer than getting hurt from the explosions
	// where you might have dodged the missile successfully
	PhysicsActor* act = m_world->getActorWithBody(other);
	if (!act)
		return;

	if (act->getClass() == ACTORCLASS_PLAYER)
		((Player*)act)->doDamage();
}

void Missile::explode()
{
	// kaboom
	Explosion* e = m_world->getPool()->getExplosion();
	e->reset(m_body->getPosition());
	setEnabled(false);
}
