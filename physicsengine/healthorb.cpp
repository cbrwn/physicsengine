/* =================================
 *  Health Orb
 *  Little sphere to show how much health we have
 * ================================= */
#include "healthorb.h"

#include <color.h>

#include "shapes.h"

HealthOrb::HealthOrb(Vector3 pos)
	: Actor(pos)
{
	m_shield = false;
	m_pulseTime = 0.0f;
}

void HealthOrb::update(float delta)
{
	m_pulseTime += delta * 3.0f;
}

void HealthOrb::draw()
{
	// copy our transform matrix so we can modify it if we need to
	Matrix4 transform = m_globalTransform;

	if (m_shield)
	{
		// to show as a shield, we want to squish it down and expand it out on
		// the x and z axes
		Matrix4 scaleMatrix;
		const float aScale = 7.0f;
		scaleMatrix.setScale(aScale, 2.0f, aScale);

		transform = transform * scaleMatrix;
	}

	// used a hex code for the colour so convert it to a Vec4
	Vector4 col = intToVector(0x8c1207ff);

	const float radius = 0.3f;
	// the solid sphere shouldn't be drawn if we're drawing as a shield
	if (!m_shield)
		drawSphere(Vector3(), radius, col, &transform, 8, 8, false);

	// factor to pulse by
	float pulseAmt = 0.5f;
	// lessen the pulse if we're drawing this as a shield
	if (m_shield)
		pulseAmt = 0.1f;
	float pulseRad = radius + radius * fabsf(sinf(m_pulseTime)) * pulseAmt;

	// red usually
	Vector4 pulseCol = Vector4(1, 0, 0, 0.3f);
	// white as a shield
	if (m_shield)
		pulseCol = Vector4(1, 1, 1, 0.3f);

	drawSphere(Vector3(), pulseRad, pulseCol, &transform, 8, 8, false);
}
