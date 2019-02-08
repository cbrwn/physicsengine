/* =================================
 *  Lava
 *  Beautiful red square that destroys anything that touches it
 * ================================= */
#pragma once

#include "physicsactor.h"

// takes this number of seconds to go between the colours
#define GLOW_SPEED 1.0f

class Lava : public PhysicsActor
{
public:
	Lava(Vector3 pos, Vector3 extents);

	void update(float delta) override;
	void draw() override;

private:
	// keep track of the size of the box so we can spawn particles in its range
	Vector3 m_extents;

	// glow colours
	Vector4 m_startColour;
	Vector4 m_endColour;

	// timer controlling particle spawns
	float m_particleTimer;

	float m_colourTimer;
	// whether we're going to the start or the end colour
	bool m_colourDirection;

	void collided(PhysicsBody* bod);

	void spawnParticles();

	// makes a smoke puff when things get devoured by the lava
	void makeSmokePuff(Vector3 const& spawnPos);

};