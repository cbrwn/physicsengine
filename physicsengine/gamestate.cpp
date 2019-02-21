#include "gamestate.h"

#include <color.h>
#include <gmath.h>
#include <Input.h>

#include "game.h"
#include "world.h"
#include "camera.h"

// actors
#include "box.h"
#include "ball.h"

GameState::GameState(Game* game) : BaseState(game) {
	m_world = nullptr;
}

GameState::~GameState() { }

void GameState::onEnter()
{
	m_world = new World(m_game);

	m_world->addActor(new Box(Vector3(0, 0, 0), Vector3(10, 1, 10), 0xbfe5ffff));

	m_world->getCamera()->setPosition(Vector3(0, 10, 20));
	m_world->getCamera()->setPitch(-0.4f);

	for (int i = 0; i < 20; ++i) {
	}

	m_grabbed = nullptr;
	m_dropTimer = 0.0f;
	m_timeBetweenDrops = 0.3f;

	PhysicsManager::gravity = 18.0f;
}

void GameState::onLeave()
{
	delete m_world;
	m_world = nullptr;
}

void GameState::update(float delta)
{
	m_world->update(delta);

	m_dropTimer += delta;
	if (m_dropTimer >= m_timeBetweenDrops){
		m_dropTimer -= m_timeBetweenDrops;
		spawnRandomBox();
	}

	doObjectGrabbing(delta);
}

void GameState::draw()
{
	m_world->draw();
}

unsigned int GameState::randomColor()
{
	unsigned int result = hsb(randBetween(0.0f, 1.0f), 1.0f, 1.0f);

	result &= 0xffffff00;
	result |= 0xff;

	return result;
}

void GameState::doObjectGrabbing(float delta)
{
	aie::Input* input = aie::Input::getInstance();
	// detect when we grab a box
	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)) {
		// cast ray from camera
		Vector3 rayStart, rayDir;
		m_world->getMouseRay(&rayStart, &rayDir);

		Vector3 hitPoint;
		PhysicsBody* hitBod = PhysicsManager::getInstance()->rayCast(rayStart, rayDir, hitPoint);
		if (hitBod && !hitBod->isStatic()) {
			m_grabbed = hitBod;

			m_grabMagnitude = (rayStart - hitBod->getPosition()).magnitude();
			m_targetMagnitude = m_grabMagnitude;
			m_grabbed->setUseGravity(false);
			m_grabbed->wakeUp();
			m_grabbed->setDrag(5.0f);

			m_lastScroll = (float)input->getMouseScroll();
		}
	}

	if (m_grabbed) {
		Vector3 rayStart, rayDir;
		m_world->getMouseRay(&rayStart, &rayDir);

		float thisScroll = (float)input->getMouseScroll();
		m_targetMagnitude += thisScroll - m_lastScroll;
		m_lastScroll = thisScroll;

		m_grabMagnitude -= (m_grabMagnitude - m_targetMagnitude) * delta * 10.0f;

		auto destPos = rayStart + (rayDir * m_grabMagnitude);
		//m_grabbed->setPosition(rayStart + (rayDir * m_grabMagnitude));
		m_grabbed->addForce((destPos - m_grabbed->getPosition())*delta*20.0f, Vector3());
	}

	// detect when box is dropped
	if (input->wasMouseButtonReleased(aie::INPUT_MOUSE_BUTTON_LEFT)) {
		if (m_grabbed) {
			m_grabbed->setUseGravity(true);
			m_grabbed->wakeUp();
			m_grabbed->setDrag(0.0f);
			m_grabbed = nullptr;
		}
	}
}

Box* GameState::spawnRandomBox()
{
	Vector3 siz(randBetween(0.1f, 0.8f), randBetween(0.1f, 0.8f), randBetween(0.1f, 0.8f));
	Vector3 pos(randBetween(-1.0f, 1.0f), randBetween(3.1f, 5.8f), randBetween(-1.0f, 1.0f));
	Box* b = new Box(pos, siz, randomColor());
	b->getBody()->setMass(randBetween(0.1f, 2.0f));
	b->getBody()->setStatic(false);
	m_world->addActor(b);

	b->getBody()->setFriction(0.0f);
	b->getBody()->setFrictionMode(FRICTION_MIN);

	// get random velocity
	const float vr = 3.0f;
	Vector3 vel(randBetween(-vr, vr), randBetween(-vr, vr), randBetween(-vr, vr));
	b->getBody()->setVelocity(vel);

	return b;
}
