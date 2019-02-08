/* =================================
 *  Tank Barrel
 *  Part of the tank which aims at its target
 * ================================= */
#include "tankbarrel.h"

#include <color.h>
#include <gmath.h>

#include "util.h"
#include "shapes.h"

TankBarrel::TankBarrel(Vector3 pos)
	: Actor(pos)
{
	m_target = nullptr;
}

void TankBarrel::update(float delta)
{
	if (m_target)
	{
		// use the weird lookAt function (which was stolen from glm) to
		//	get a matrix which faces the player
		m_rotationMatrix = Matrix4::lookAt(getPosition(),
			m_target->getPosition(), Vector3(0, 1, 0)).inverse();

		// spin it around 90 degrees so the tip of the cylinder points
		//	where we're facing
		Matrix4 spinnyspin;
		spinnyspin.setRotateX(PI / 2.0f);
		m_rotationMatrix = m_rotationMatrix * spinnyspin;
	}

	updateTransform();
}

void TankBarrel::draw()
{
	Vector4 colour = intToVector(0x255418ff);

	// barrel base
	const float baseHeight = 0.5f;
	drawCylinder(Vector3(0, baseHeight / 3.3f, 0), 0.8f, baseHeight, 8,
		colour, &m_globalTransform);

	// barrel itself
	// move it slightly up out of the base
	Vector3 offset = -0.8f * m_rotationMatrix.getUp();
	drawCylinder(offset, 0.2f, 1.5f, 8, colour,
		&m_rotationMatrix);
}

void TankBarrel::setTarget(Actor* t) { m_target = t; }

Vector3 TankBarrel::getShootDirection()
{
	return m_rotationMatrix.getForward();
}
