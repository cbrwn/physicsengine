/* =================================
 *  Explosion	
 *  Just a zone which collides with things when something explodes
 *  Also does some visual effects like spawning particles and a quick flash
 * ================================= */
#pragma once

#include "physicsactor.h"

class Explosion : public PhysicsActor
{
public:
	Explosion();

	void update(float delta) override;
	void draw() override;

	void reset(Vector3 pos);

private:
	bool m_grow;
	float m_size;

	void collided(PhysicsBody* bod);
};