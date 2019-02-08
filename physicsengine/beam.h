/* =================================
 *  Beam
 *  The tractor beam under the UFO which can pick things up
 * ================================= */
#pragma once

#include "physicsactor.h"

class Player;
struct ColliderCone;

class Beam : public PhysicsActor
{
public:
	Beam(Vector3 pos, Player* plr);

	void update(float delta) override;
	void draw() override;

	// override this so we can control what happens if this gets disabled
	void setEnabled(bool e) override;

private:
	// keep a pointer to the collider because we'll be changing its shape
	ColliderCone* m_collider;

	// keep a pointer to the player attached to this so we can use its height
	// off the ground to determine the size
	Player* m_player;

	// the actor that's picked up by the beam
	PhysicsActor* m_beamed;
	// timer used to move the actor up and down when it's in the beam
	float m_wobbleTime;

	// cone dimensions
	float m_height;
	float m_radius;
	int m_segments;

	// the actual radius to draw - it stays above a certain size when super
	// close to the ground
	float m_drawRadius;

	// gets the closest actor inside the beam
	PhysicsActor* findActor();

	// attaches an actor to the beam
	void pickupActor(PhysicsActor* a);
	// drops the actor that's attached to the beam
	void dropActor();
};