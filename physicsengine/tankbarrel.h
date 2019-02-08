/* =================================
 *  Tank Barrel
 *  Part of the tank which aims at its target
 * ================================= */
#pragma once

#include "actor.h"

class TankBarrel : public Actor
{
public:
	TankBarrel(Vector3 pos);

	void update(float delta) override;
	void draw() override;

	void setTarget(Actor* t);

	// gets the direction vector of the way the barrel is pointing
	Vector3 getShootDirection();

private:
	Actor* m_target;

	Matrix4 m_rotationMatrix;
};