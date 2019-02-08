#pragma once

#include "basestate.h"
#include <vector2.h>

class World;
class PhysicsBody;

class DemoState : public BaseState {
public:
	DemoState(Game* game);
	~DemoState();

	void onEnter() override;
	void onLeave() override;

	void update(float delta) override;
	void draw() override;

private:
	World* m_world;

	PhysicsBody* m_grabbed;
	float m_timer;
	float m_grabMagnitude;
	float m_targetMagnitude;
	float m_lastScroll;

	Vector2 m_lastMouse;

	unsigned int randomColor();

	void doCameraMovement(float delta);
};
