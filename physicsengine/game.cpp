/* =================================
 *  Game
 *  Main class of the game, controlling its states
 * ================================= */
#include "game.h"
#include <Gizmos.h>
#include <Input.h>
#include <imgui.h>
#include <glm/glm.hpp>

#include "physicsmanager.h"

 // states
#include "basestate.h"
#include "gamestate.h"
#include "demostate.h"

Game::Game() {}
Game::~Game() {}

bool Game::startup()
{
	using namespace aie;

	// make singleton instance
	PhysicsManager::create();

	// put a sky colour as the background colour
	setBackgroundColour(0.3f, 0.6f, 0.9f);

	// initialise gizmo primitive counts
	// VERY excessive counts
	Gizmos::create(100000, 500000, 10000, 10000);

	m_currentState = nullptr;
	m_states.push_back(new DemoState(this));
	//m_states.add(new DemoState(this));

	changeState(GAMESTATE_GAME);

	m_accumulatedDelta = 0.0f;

	return true;
}

void Game::shutdown()
{
	aie::Gizmos::destroy();
	PhysicsManager::destroy();

	// things can be deleted in onLeave so make sure to call that
	if (m_currentState)
		m_currentState->onLeave();

	for (int i = 0; i < m_states.size(); ++i)
		delete m_states[i];
}

void Game::update(float deltaTime)
{
	PhysicsManager* physics = PhysicsManager::getInstance();

	// attempt to give the physics system a fixed timestep
	m_accumulatedDelta += deltaTime;
	if (m_accumulatedDelta >= FIXED_TIMESTEP) {
		if (m_currentState)
			m_currentState->update(FIXED_TIMESTEP);

		physics->update(FIXED_TIMESTEP);
		m_accumulatedDelta -= FIXED_TIMESTEP;
	}

	aie::Input* input = aie::Input::getInstance();

	if (input->wasKeyPressed(aie::INPUT_KEY_ESCAPE))
		this->quit();
}

void Game::draw()
{
	clearScreen();

	if (m_currentState)
		m_currentState->draw();
}

void Game::changeState(States state)
{
	// put this here instead of putting it in EVERY state's onLeave
	PhysicsManager::getInstance()->clear();

	if (m_currentState)
		m_currentState->onLeave();

	m_currentState = m_states[state];

	if (m_currentState)
		m_currentState->onEnter();
}

BaseState* Game::getCurrentState()
{
	return m_currentState;
}
