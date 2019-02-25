/* =================================
 *  Game
 *  Main class of the game, controlling its states
 * ================================= */
#pragma once

#include <vector>
#include <Application.h>

class BaseState;

enum States
{
	GAMESTATE_GAME = 0,
	GAMESTATE_MENU,

	GAMESTATE_COUNT
};

#define FIXED_TIMESTEP 0.016f

class Game : public aie::Application
{
public:
	Game();
	virtual ~Game();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	// state controls
	void changeState(States state);
	BaseState* getCurrentState();

protected:
	BaseState* m_currentState;
	std::vector<BaseState*> m_states;

	float m_accumulatedDelta;

};