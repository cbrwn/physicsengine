/* =================================
 *  Base State
 *  Base class for game states
 * ================================= */
#pragma once

class Game;

class BaseState
{
public:
	BaseState(Game* game);
	virtual ~BaseState();

	// called when the game changes to this state
	virtual void onEnter() = 0;
	// called when the game changes away from this state
	virtual void onLeave() = 0;

	virtual void update(float delta) = 0;
	virtual void draw() = 0;

protected:
	Game* m_game;
};