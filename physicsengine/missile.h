/* =================================
 *  Missile
 *  Target-seeking missile shot by tanks
 * ================================= */
#pragma once

#include "physicsactor.h"

class Missile : public PhysicsActor
{
public:
	Missile();

	void update(float delta) override;
	void draw() override;

	void reset(Vector3 pos, PhysicsActor* target);

private:
	PhysicsActor* m_target;

	// missiles fall to the ground after being caught by the player
	bool m_falling;
	// keep track of the time it's alive so we can explode it after a while
	float m_lifeTimer;
	// timer to make smoke particles
	float m_smokeTimer;

	// dimensions of the missile
	const float m_radius;
	const float m_height;

	void collided(PhysicsBody* other);
	void explode();
};