/* =================================
 *  Physics Actor
 *  A specific kind of Actor which has a PhysicsBody
 * ================================= */
#pragma once

#include "actor.h"
#include "physics.h"

class PhysicsActor : public Actor
{
public:
	PhysicsActor(Vector3 pos);
	virtual ~PhysicsActor();

	virtual void update(float delta) override;
	virtual void draw() override;

	// enables/disabled the physics body as well as the actor itself
	void setEnabled(bool e) override;

	virtual void reset() override;

	PhysicsBody* getBody() { return m_body; }

protected:
	PhysicsBody* m_body;
};