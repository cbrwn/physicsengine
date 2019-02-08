/* =================================
 *  Tank
 *  The missile-shooting enemies
 * ================================= */
#pragma once

#include "physicsactor.h"

#include <functional> // for std::function

class TankBarrel;

class Tank : public PhysicsActor
{
public:
	Tank(Vector3 pos);

	void update(float delta) override;
	void draw() override;

	void setEnabled(bool e) override;

	// allows us to change the target at which it shoots missiles
	void setTarget(PhysicsActor* a);

	// override this so we can add the tank's barrel actor to the same world
	void setWorld(World* w) override;

	void setDestroyCallback(std::function<void()> func);

private:
	TankBarrel* m_barrel;

	// lets us know when the tank is destroyed so we can check if we won
	std::function<void()> m_destroyCallback;

	// which actor to shoot the missile at
	// (usually the player)
	PhysicsActor* m_target;

	// how much time has passed since we shot our last missile
	float m_shotTimer;

	// the length of the longest side of the tank's body
	const float m_length;
};