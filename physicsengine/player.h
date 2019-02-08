/* =================================
 *  Player
 *  The UFO that the player controls during the game
 * ================================= */
#pragma once

#include <Input.h>
#include <vector2.h>
#include <functional> // for std::function

#include "physicsactor.h"

class HealthOrb;
class PhysicsBody;
struct ColliderCylinder;

class Player : public PhysicsActor
{
public:
	Player(Vector3 pos);

	void update(float delta) override;
	void draw() override;

	void setEnabled(bool e) override;

	int getHealth() { return m_health; }

	// used to add a HealthOrb to our list and also make it a child
	void addOrb(HealthOrb* orb);

	// called by other objects to do damage to the player
	void doDamage();

	void setDeathCallback(std::function<void()> func);

private:
	// keep track of the last mouse position to know how much to turn when
	// dragging
	Vector2 m_lastMouse;

	// the current rotation of the camera - keep this separate from the actor's
	// rotation because the camera is able to rotate in all directions while
	// the actor is only able to rotate around the Y axis
	Vector3 m_cameraRotation;

	// called when the player is killed so we know the game is over
	std::function<void()> m_deathCallback;

	int m_health;

	// orbs which represent our health
	DArray<HealthOrb*> m_orbs;
	// timer used to spin these orbs around the player
	float m_orbSpinTimer;

	void updateHealthOrbs();

	// separate sections of the UFO into their own functions
	void drawShipBody();
	void drawShipTop();

	// handles camera movement
	void doMouseInput(aie::Input* input, float delta);
	// handles actor movement
	void doKeyboardInput(aie::Input* input, float delta);
};