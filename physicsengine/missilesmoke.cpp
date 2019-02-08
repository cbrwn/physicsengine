/* =================================
 *  Missile Smoke
 *  A little "particle" that comes out the back of a missile
 * ================================= */
#include "missilesmoke.h"

#include <gmath.h>

#include "shapes.h"

MissileSmoke::MissileSmoke()
	: Actor(Vector3())
{
	setEnabled(false);
}

void MissileSmoke::update(float delta)
{
	// decay the velocity like there's some sort of air resistance
	m_velocity -= (m_velocity)* delta;

	// apply its velocity
	Vector3 pos = getPosition();
	pos += m_velocity * delta * 5.0f;
	setGlobalPosition(pos);

	// decay the size
	m_size -= delta * m_fadeRate;

	// limit m_time to m_colourTime so the lerp doesn't go over its end point
	if (m_time < m_colourTime)
		m_time += delta;

	// disable the particle once we can't see it
	if (m_size <= 0.0f)
		setEnabled(false);
}

void MissileSmoke::draw()
{
	// lerp between the start colour and black
	float percentage = m_time / m_colourTime;
	Vector4 colour = Vector4::lerpBetween(m_startColour, Vector4(0, 0, 0, 1),
		percentage);

	drawSphere(getPosition(), m_size, colour, nullptr, 4, 4, false);
}

void MissileSmoke::reset(Vector3 const& pos, Vector3 const& dir, float sz,
	Vector4 startCol)
{
	setEnabled(true);

	// vary the starting size a little bit
	m_size = sz * randBetween(0.8f, 1.5f);
	// make larger particles fade out slower
	m_fadeRate = (m_size*1.5f) + randBetween(0.0f, 0.2f);
	// make the colour changing based on the length of the particle's life
	m_colourTime = m_fadeRate / 3.0f;
	m_time = 0.0f;
	m_startColour = startCol;

	setGlobalPosition(pos);

	m_velocity = dir;
	// vary the velocity a little
	const float directionVariance = 1.0f;
	m_velocity.x += randBetween(-directionVariance, directionVariance);
	m_velocity.y += randBetween(-directionVariance, directionVariance);
	m_velocity.z += randBetween(-directionVariance, directionVariance);
}
