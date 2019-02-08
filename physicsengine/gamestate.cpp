/* =================================
 *  Game State
 *  The state where the game takes place
 * ================================= */
#include "gamestate.h"

#include <gmath.h>
#include <imgui.h>

#include "world.h"
#include "camera.h"
#include "shapes.h"
#include "gamehud.h"
#include "healthorb.h"
#include "objectpool.h"

// actors
#include "box.h"
#include "beam.h"
#include "game.h"
#include "lava.h"
#include "tank.h"
#include "plane.h"
#include "player.h"
#include "Gizmos.h"
#include "color.h"

GameState::GameState(Game* game)
	: BaseState(game), m_resetCooldown(2.0f)
{
	m_renderer = new aie::Renderer2D();

	m_hud = nullptr;
	m_world = nullptr;
	m_player = nullptr;

	// size of the play area
	m_worldSize = 100.0f;

	// used to use mouse wheel to scroll
	m_lastScroll = 0.0;
	// distance from the player
	m_cameraDistance = 40.0f;
}

GameState::~GameState()
{
	delete m_renderer;
}

void GameState::onEnter()
{
	// make our world
	m_world = new World(m_game);

	// make hud
	m_hud = new GameHud();

	// create a specific amount of each object
	m_world->getPool()->createMissiles(50);
	m_world->getPool()->createMissileSmokes(20000);
	m_world->getPool()->createExplosions(50);

	// default end game values
	m_won = false;
	m_resetTime = 0.0f;
	m_gameOver = false;

	// giant functions that deal with making the surroundings
	makeWalls();
	makeFloor();

	// and ones that deal with making the other actors 
	makePlayer();
	makeTanks();

	// make sure we know about the player dying
	m_player->setDeathCallback([this]()
	{
		m_gameOver = true;
		m_hud->showGameOver(true);
	});
}

void GameState::onLeave()
{
	delete m_hud;

	delete m_world;
	m_world = nullptr;

	// player will have been deleted by the world
	m_player = nullptr;
}

void GameState::update(float delta)
{
	using namespace aie;

	m_world->update(delta);
	m_hud->update(delta);

	// do reset cooldown timing
	if (m_won || m_gameOver)
	{
		m_resetTime += delta;
		if (m_resetTime > m_resetCooldown)
			m_hud->showReset(true);
	}

	// deals with moving the camera to a nice viewing point of the player
	setCameraOffset();

	Input* input = Input::getInstance();

	// use escape to quit the game
	if (input->wasKeyPressed(INPUT_KEY_ESCAPE))
		m_game->quit();

	// zooming in/out with the mousewheel
	// the bootstrap's getMouseScroll returns the amount the scrollwheel has
	//	scrolled *in total* since the start of the program
	// - so we need to keep track of this and only scroll when it changes
	double scroll = input->getMouseScroll();
	double dif = scroll - m_lastScroll;
	// keep track of the current scroll value
	m_lastScroll = scroll;
	// zoom in/out the amount that was scrolled this frame
	m_cameraDistance -= (float)dif;

	// allow resetting after 2 seconds
	// the purpose of this cooldown is to stop accidentally resetting since
	// the button to pick things up is the same as the reset button
	if (m_resetTime > m_resetCooldown)
		if (input->wasKeyPressed(INPUT_KEY_SPACE))
			m_game->changeState(GAMESTATE_GAME);

	// some quick ImGui text to show controls
	ImGui::Text("Destroy all tanks to win!");
	ImGui::Text(" - WASD to move");
	ImGui::Text(" - Q/E to move down/up");
	ImGui::Text(" - Space to pick up/drop objects in the beam");
	ImGui::Text(" - Right click + drag to move camera");

	// instant win button to test things
	if (ImGui::Button("Win"))
		for (int i = 0; i < m_tanks.getCount(); ++i)
			m_tanks[i]->setEnabled(false);
}

void GameState::draw()
{
	m_world->draw();
	

	float windowWidth = (float)m_game->getWindowWidth();
	float windowHeight = (float)m_game->getWindowHeight();

	m_renderer->begin();

	m_hud->setWindowSize(windowWidth, windowHeight);
	m_hud->draw(m_renderer);

	m_renderer->end();
}

void GameState::tankDestroyed()
{
	// check if any tanks exist
	// if none exist, we've won!
	m_won = true;
	for (int i = 0; i < m_tanks.getCount(); ++i)
	{
		if (m_tanks[i]->isEnabled())
		{
			m_won = false;
			break;
		}
	}

	if (!m_won)
		return;

	// show the win text when we win
	m_hud->showGameWon(true);
}

void GameState::makeTanks()
{
	PhysicsManager* phys = PhysicsManager::getInstance();

	m_tanks.clear();
	for (int i = 0; i < 10; ++i)
	{
		// _temp holds the output of the raycast which we don't need
		Vector3 pos, _temp;

		const float spawnLimit = m_worldSize * 0.9f;
		// try to generate a position that has floor underneath it
		do
		{
			pos.x = randBetween(-spawnLimit, spawnLimit);
			pos.y = 2.0f;
			pos.z = randBetween(-spawnLimit, spawnLimit);

			// repeat this if there's nothing under the tank
			// (will allow tanks to spawn in the pit! maybe change)
		} while (!phys->rayCast(pos, Vector3(0, -1, 0), _temp));

		// make a tank at this newly generated position
		auto t = new Tank(pos);
		// make sure it's targeting the player
		t->setTarget(m_player);

		// make sure we know when this tank's killed
		t->setDestroyCallback([this]() { tankDestroyed(); });

		// and add it :D
		m_world->addActor(t);

		// keep track of it so we know if we win
		m_tanks.add(t);
	}
}

void GameState::makePlayer()
{
	// make our player and its beam
	auto p = new Player(Vector3(0, 8, 0));
	m_player = p;
	auto b = new Beam(Vector3(), p);

	for (int i = 0; i < m_player->getHealth(); ++i)
	{
		auto orb = new HealthOrb(Vector3());
		p->addOrb(orb);
		m_world->addActor(orb);
	}

	// attach the beam to the player
	p->addChild(b);

	// add them both to the world
	m_world->addActor(p);
	m_world->addActor(b);
}

void GameState::makeWalls()
{
	const float wallHeight = 50.0f;
	// colours
	const unsigned int wallColLight = 0x484848ff;
	const unsigned int wallColDark = 0x393939ff;

	Vector4 vecLight = intToVector(wallColLight);
	Vector4 vecDark = intToVector(wallColDark);

	// shorten down worldSize - 1
	const float ws = m_worldSize - 1.0f;

	// -z wall
	m_world->addActor(new Box(Vector3(0, wallHeight - 2.0f, -ws),
		Vector3(m_worldSize, wallHeight, 1.0f), 0x00000000));

	// -x wall
	m_world->addActor(new Box(Vector3(-ws, wallHeight - 2.0f, 0),
		Vector3(1.0f, wallHeight, m_worldSize - 2.0f), 0x00000000));
	// z wall
	m_world->addActor(new Box(Vector3(0, wallHeight - 2.0f, ws),
		Vector3(m_worldSize, wallHeight, 1.0f), 0x00000000));

	// x wall
	m_world->addActor(new Box(Vector3(ws, wallHeight - 2.0f, 0),
		Vector3(1.0f, wallHeight, m_worldSize - 2.0f), 0x00000000));

	// instead of displaying the walls as boxes, replace them visually with
	// one-way planes, so when the camera goes through the wall they can still
	// see what's happening
	
	// -Z
	m_world->addActor(new Plane(
		Vector3(-m_worldSize, wallHeight*2, ws - 1),
		Vector3(m_worldSize, wallHeight*2, ws - 1),
		Vector3(m_worldSize, 0, ws-1),
		Vector3(-m_worldSize, 0, ws-1),
		vecLight));

	// +Z
	m_world->addActor(new Plane(
		Vector3(-m_worldSize, 0, -ws+1),
		Vector3(m_worldSize, 0, -ws+1),
		Vector3(m_worldSize, wallHeight*2, -ws + 1),
		Vector3(-m_worldSize, wallHeight*2, -ws + 1),
		vecLight));

	// -X
	m_world->addActor(new Plane(
		Vector3(-m_worldSize + 2, wallHeight * 2, -ws),
		Vector3(-m_worldSize + 2, wallHeight * 2, ws),
		Vector3(-m_worldSize + 2, 0, ws),
		Vector3(-m_worldSize + 2, 0, -ws),
		vecDark));

	// +X
	m_world->addActor(new Plane(
		Vector3(m_worldSize - 2, 0, -ws),
		Vector3(m_worldSize - 2, 0, ws),
		Vector3(m_worldSize - 2, wallHeight * 2, ws),
		Vector3(m_worldSize - 2, wallHeight * 2, -ws),
		vecDark));
}

void GameState::makeFloor()
{
	// dimensions of pit itself
	const float pitx = 0.0f;
	const float pitz = 0.0f;
	const float pitw = 10.0f;
	const float pith = 10.0f;

	//
	// grab some numbers to help me visualise it
	//

	// start/end positions of the pit
	const float pitStartX = pitx - pitw;
	const float pitEndX = pitx + pitw;
	const float pitStartZ = pitz - pith;
	const float pitEndZ = pitz + pith;

	// width of the surrounding walls 
	const float wallWidth = 2.0f;

	// start and end of the play area
	const float worldStartX = -m_worldSize + wallWidth;
	const float worldEndX = m_worldSize - wallWidth;
	const float worldStartZ = -m_worldSize + wallWidth;
	const float worldEndZ = m_worldSize - wallWidth;

	// floor dimensions
	const float floorHeight = 10.0f;
	const float floorPos = -(floorHeight + 0.01f);

	// min and max so it's easier to put into our makeBoxFromBounds
	const float floorMin = floorPos - floorHeight;
	const float floorMax = floorPos + floorHeight;

	// left of pit
	m_world->addActor(makeBoxFromBounds(
		Vector3(worldStartX, floorMin, worldStartZ),
		Vector3(pitStartX, floorMax, worldEndZ)
	));

	// right of pit
	m_world->addActor(makeBoxFromBounds(
		Vector3(pitEndX, floorMin, worldStartZ),
		Vector3(worldEndX, floorMax, worldEndZ)
	));

	// top of pit
	m_world->addActor(makeBoxFromBounds(
		Vector3(pitStartX, floorMin, worldStartZ),
		Vector3(pitEndX, floorMax, pitStartZ)
	));

	// bottom of pit
	m_world->addActor(makeBoxFromBounds(
		Vector3(pitStartX, floorMin, pitEndZ),
		Vector3(pitEndX, floorMax, worldEndZ)
	));

	// make a hidden box which just shows lines
	auto hiddenBox = new Box(Vector3(pitx, floorPos, pitz),
		Vector3(pitw - 0.01f, floorHeight, pith - 0.01f), 0x00000000);
	hiddenBox->getBody()->setEnabled(false);
	m_world->addActor(hiddenBox);

	// make fire pit
	auto l = new Lava(Vector3(pitx, floorPos + 2.0f, pitz),
		Vector3(pitw, floorHeight - 7.0f, pith - 0.01f));
	m_world->addActor(l);
}

void GameState::setCameraOffset()
{
	// calculate the camera's offset from its actual position
	// - moving it back and angling it down to focus on its posiition
	Matrix4 camPos;
	Matrix4 camRot;

	camPos.setPosition(0, 0.0f, m_cameraDistance);
	camRot.setRotateX(-0.2f);

	Matrix4 camMatrix = camPos * camRot;

	m_world->getCamera()->setParentMatrix(camMatrix);
}

Box* GameState::makeBoxFromBounds(Vector3 min, Vector3 max, unsigned int col)
{
	// turn the bounds into a center point and extents
	Vector3 size = max - min;
	Vector3 middle = min + (size / 2.0f);

	return new Box(middle, size / 2.0f, col, false);
}
