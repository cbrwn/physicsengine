#pragma once

#include "basestate.h"
#include <vector2.h>

class Box;
class World;
class PhysicsBody;

class GameState: public BaseState {
public:
	GameState(Game* game);
	~GameState();

	void onEnter() override;
	void onLeave() override;

	void update(float delta) override;
	void draw() override;

private:
	World* m_world;

	float m_dropTimer;
	float m_timeBetweenDrops;

	PhysicsBody* m_grabbed;
	float m_grabMagnitude;
	float m_targetMagnitude;
	float m_lastScroll;

	unsigned int randomColor();

	void doObjectGrabbing(float delta);
	Box* spawnRandomBox();
};
