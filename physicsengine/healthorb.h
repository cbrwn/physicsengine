/* =================================
 *  Health Orb
 *  Little sphere to show how much health we have
 * ================================= */
#pragma once

#include "actor.h"

class HealthOrb : public Actor
{
public:
	HealthOrb(Vector3 pos);

	void update(float delta) override;
	void draw() override;

	void setShield(bool e) { m_shield = e; }

private:
	// timer which controls the translucent pulsing
	float m_pulseTime;

	// when on 1 health, display the orb as a shield around the player
	bool m_shield;
};