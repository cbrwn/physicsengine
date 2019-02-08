/* =================================
 *  Game State
 *  The state where the game takes place
 * ================================= */
#pragma once

#include "basestate.h"

#include <darray.h>
#include <vector3.h>
#include <Renderer2D.h>

class Box;
class Tank;
class World;
class Player;
class GameHud;

class GameState : public BaseState
{
public:
	GameState(Game* game);
	~GameState();

	void onEnter() override;
	void onLeave() override;

	void update(float delta) override;
	void draw() override;
	
private:
	// make a 2D renderer to show the HUD
	aie::Renderer2D* m_renderer;
	GameHud* m_hud;

	// the world which contains all actors
	World* m_world;
	// size of the play area
	float m_worldSize;

	// end of game flags
	bool m_won;
	bool m_gameOver;

	// don't let the player reset until this timer is greater than
	// resetCooldown to stop accidental restarts
	float m_resetTime;
	const float m_resetCooldown;

	// keep a pointer to the player so we can tell the tanks who to shoot
	Player* m_player;
	// keep track of all the tanks so we know when they're all destroyed
	DArray<Tank*> m_tanks;

	// save the last mouse scroll value due to how the bootstrap's scroll input
	// works
	double m_lastScroll;
	// camera's distance from the player
	float m_cameraDistance;

	// called whenever a tank is destroyed so we can check if the player won
	void tankDestroyed();

	// move actor creation to their own functions to make onEnter less messy
	void makeTanks();
	void makePlayer();

	// makes walls at the edges of the play area based on m_worldSize
	void makeWalls();
	// makes the floor with the lava
	void makeFloor();

	// updates the camera's parent transform based on m_cameraDistance
	void setCameraOffset();

	// easier way to make a Box from bounds, as opposed to using a center
	// point and extents
	Box* makeBoxFromBounds(Vector3 min, Vector3 max, 
		unsigned int col = 0x999999ff);
};