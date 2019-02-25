#include "demostate.h"

#include <color.h>
#include <gmath.h>
#include <Input.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include "game.h"
#include "world.h"
#include "camera.h"

// actors
#include "box.h"
#include "ball.h"

DemoState::DemoState(Game* game) : BaseState(game) {
	m_world = nullptr;
}

DemoState::~DemoState() { }

void DemoState::onEnter()
{
	m_world = new World(m_game);

	m_world->addActor(new Box(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(100.0f, 1.0f, 100.0f), 0x333333ff));

	for (int i = 0; i < 2; ++i) {
		glm::vec3 siz(randBetween(0.1f, 0.8f), randBetween(0.1f, 0.8f), randBetween(0.1f, 0.8f));
		glm::vec3 pos(randBetween(-5.0f, 5.0f), randBetween(1.1f, 5.8f), randBetween(-5.1f, 5.8f));
		Box* b = new Box(pos, siz, randomColor());
		b->getBody()->setMass(randBetween(0.1f, 2.0f));
		b->getBody()->setStatic(false);
		m_world->addActor(b);
	}

	const float height = 1.0f;
	for (int i = 0; i < 2; ++i) {
		glm::vec3 siz(height, height, height);
		glm::vec3 pos(0.0f, (i+1)*height * 2, 0.0f);
		Ball* b = new Ball(pos, siz.y, randomColor());
		b->getBody()->setMass(1.0f);
		b->getBody()->setStatic(false);
		m_world->addActor(b);
	}

	Ball* staticBall = new Ball(glm::vec3(8.0f, 3.0f, 8.0f), 1.5f, 0xffffffff);
	staticBall->getBody()->setStatic(true);
	m_world->addActor(staticBall);

	m_world->getCamera()->setPosition(glm::vec3(0.0f, 6.0f, 20.0f));
	m_world->getCamera()->setPitch(-0.2f);

	m_timer = 0.0f;

	m_grabbed = nullptr;

	PhysicsManager::gravity = 18.0f;
}

void DemoState::onLeave()
{
	delete m_world;
	m_world = nullptr;
}

void DemoState::update(float delta)
{
	m_world->update(delta);

	aie::Input* input = aie::Input::getInstance();
	doCameraMovement(delta);

	// detect when we grab a box
	if (input->wasMouseButtonPressed(aie::INPUT_MOUSE_BUTTON_LEFT)) {
		// cast ray from camera
		glm::vec3 rayStart, rayDir;
		m_world->getMouseRay(&rayStart, &rayDir);

		glm::vec3 hitPoint;
		PhysicsBody* hitBod = PhysicsManager::getInstance()->rayCast(rayStart, rayDir, hitPoint);
		if (hitBod && !hitBod->isStatic()) {
			m_grabbed = hitBod;

			m_grabMagnitude = glm::length(rayStart - hitBod->getPosition());
			m_targetMagnitude = m_grabMagnitude;
			m_grabbed->setUseGravity(false);
			m_grabbed->setDebug(true);
			m_grabbed->wakeUp();
			m_grabbed->setDrag(5.0f);

			m_lastScroll = (float)input->getMouseScroll();
		}
	}

	if (m_grabbed) {
		glm::vec3 rayStart, rayDir;
		m_world->getMouseRay(&rayStart, &rayDir);

		float thisScroll = (float)input->getMouseScroll();
		m_targetMagnitude += thisScroll - m_lastScroll;
		m_lastScroll = thisScroll;

		m_grabMagnitude -= (m_grabMagnitude - m_targetMagnitude) * delta * 10.0f;

		auto destPos = rayStart + (rayDir * m_grabMagnitude);
		//m_grabbed->setPosition(rayStart + (rayDir * m_grabMagnitude));
		m_grabbed->addForce((destPos - m_grabbed->getPosition())*delta*20.0f, glm::vec3());
		m_grabbed->wakeUp();
	}

	// detect when box is dropped
	if (input->wasMouseButtonReleased(aie::INPUT_MOUSE_BUTTON_LEFT)) {
		if (m_grabbed) {
			m_grabbed->setUseGravity(true);
			m_grabbed->setDebug(false);
			m_grabbed->wakeUp();
			m_grabbed->setDrag(0.0f);
			m_grabbed = nullptr;
		}
	}
}

void DemoState::draw()
{
	m_world->draw();
}

unsigned int DemoState::randomColor()
{
	unsigned int result = hsb(randBetween(0.0f, 1.0f), 1.0f, 1.0f);

	result &= 0xffffff00;
	result |= 0xff;

	return result;
}

void DemoState::doCameraMovement(float delta)
{
	aie::Input* input = aie::Input::getInstance();

	Camera* c = m_world->getCamera();
	glm::vec3 camPos = c->getPosition();
	glm::mat4 camTransform = c->getViewMatrix();
	glm::vec3 movement;

	int _mx, _my;
	input->getMouseXY(&_mx, &_my);
	glm::vec2 thisMouse(_mx, _my);
	glm::vec2 mouseDelta = thisMouse - m_lastMouse;
	m_lastMouse = thisMouse;

	const float camSpd = 10.0f;

	if (input->isKeyDown(aie::INPUT_KEY_W))
		movement.z -= 1.0f;
	if (input->isKeyDown(aie::INPUT_KEY_S))
		movement.z += 1.0f;
	if (input->isKeyDown(aie::INPUT_KEY_A))
		movement.x -= 1.0f;
	if (input->isKeyDown(aie::INPUT_KEY_D))
		movement.x += 1.0f;
	if (input->isKeyDown(aie::INPUT_KEY_Q))
		movement.y -= 1.0f;
	if (input->isKeyDown(aie::INPUT_KEY_E))
		movement.y += 1.0f;

	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT)) {
		mouseDelta /= 500.0f;
		c->setRotation(c->getRotation() + glm::vec3(mouseDelta.y, -mouseDelta.x, 0.0f));
	}

	if (glm::length2(movement) > 1.0f)
		movement = glm::normalize(movement);
	glm::vec3 offset;
	offset += glm::vec3(camTransform[0]) * movement.z;
	offset += glm::vec3(camTransform[1]) * movement.x;
	offset += glm::vec3(camTransform[2]) * movement.y;

	camPos += offset * camSpd * delta;
	c->setPosition(camPos);
}
