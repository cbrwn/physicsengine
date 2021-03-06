/* =================================
 *	Box 
 *  A generic box shape with physics
 * ================================= */
#pragma once

#include "physicsactor.h"

class PhysicsBody;

#define BOX_GROW_TIME 1.8f

class Box : public PhysicsActor
{
public:
	Box(Vector3 pos, Vector3 size, unsigned int col, bool lines = true);

	void update(float delta) override;
	void draw() override;

private:
	bool m_drawLines;

	float m_growTime;

	unsigned int m_color;
	Vector3 m_size;
};