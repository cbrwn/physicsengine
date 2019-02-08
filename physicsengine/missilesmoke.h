/* =================================
 *  Missile Smoke
 *  A little "particle" that comes out the back of a missile
 * ================================= */
#pragma once

#include "actor.h"

class MissileSmoke : public Actor
{
public:
	MissileSmoke();

	void update(float delta) override;
	void draw() override;

	void reset(Vector3 const& pos, Vector3 const& dir, float sz = 0.3f,
		Vector4 startCol = Vector4(1.0f, 0.4f, 0.0f, 1.0f));

private:
	Vector3 m_velocity;

	// colour this particle starts at and fades to black
	Vector4 m_startColour;

	// radius of the sphere
	float m_size;
	// time this has been active
	float m_time;
	// the speed at which its size decays
	float m_fadeRate;
	// total time between the colours
	float m_colourTime;
};