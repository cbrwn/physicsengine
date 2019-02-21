#include "demostate.h"

#include <color.h>
#include <gmath.h>
#include <Input.h>

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

	m_world->addActor(new Box(Vector3(0, 0, 0), Vector3(100, 1, 100), 0x333333ff));

	//for (int i = 0; i < 10; ++i) {
	//	Vector3 siz(randBetween(0.1f, 0.8f), randBetween(0.1f, 0.8f), randBetween(0.1f, 0.8f));
	//	Vector3 pos(randBetween(-5.0f, 5.0f), randBetween(1.1f, 5.8f), randBetween(-5.1f, 5.8f));
	//	//Box* b = new Box(pos, siz, randomColor());
	//	Ball* b = new Ball(pos, siz.y*2.0f, randomColor());
	//	b->getBody()->setMass(randBetween(0.1f, 2.0f));
	//	b->getBody()->setStatic(false);
	//	m_world->addActor(b);
	//}aaaaaaaaaaa

	for (int i = 0; i < 20; ++i) {
		Vector3 siz(randBetween(0.1f, 0.8f), randBetween(0.1f, 0.8f), randBetween(0.1f, 0.8f));
		Vector3 pos(randBetween(-5.0f, 5.0f), randBetween(1.1f, 5.8f), randBetween(-5.1f, 5.8f));
		Box* b = new Box(pos, siz, randomColor());
		b->getBody()->setMass(randBetween(0.1f, 2.0f));
		b->getBody()->setStatic(false);
		m_world->addActor(b);
		if (i == 0)
			b->getBody()->setDebug(true);
	}

	const float height = 1.0f;
	for (int i = 0; i < 2; ++i) {
		Vector3 siz(height, height, height);
		Vector3 pos(0.0f, (i+1)*height * 2, 0.0f);
		Ball* b = new Ball(pos, siz.y, randomColor());
		b->getBody()->setMass(1.0f);
		b->getBody()->setStatic(false);
		m_world->addActor(b);

		if (i == 0)
			b->getBody()->setDebug(true);
	}

	Ball* staticBall = new Ball(Vector3(8, 3, 8), 1.5f, 0xffffffff);
	staticBall->getBody()->setStatic(true);
	m_world->addActor(staticBall);

	m_world->getCamera()->setPosition(Vector3(0, 6, 20));
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
	//unsigned int result = 0;
	//result |= randBetween(0x00, 0xff) << 24;
	//result |= randBetween(0x00, 0xff) << 16;
	//result |= randBetween(0x00, 0xff) << 8;
	//result |= 0xff;
	//return result;
}

void DemoState::doCameraMovement(float delta)
{
	aie::Input* input = aie::Input::getInstance();

	Camera* c = m_world->getCamera();
	Vector3 camPos = c->getPosition();
	Matrix4 camTransform = c->getViewMatrix().inverse();
	Vector3 movement;

	int _mx, _my;
	input->getMouseXY(&_mx, &_my);
	Vector2 thisMouse(_mx, _my);
	Vector2 mouseDelta = thisMouse - m_lastMouse;
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
		c->setRotation(c->getRotation() + Vector3(mouseDelta.y, -mouseDelta.x, 0.0f));
	}

	if (movement.magnitudeSquared() > 1.0f)
		movement.normalise();
	Vector3 offset;
	offset += camTransform.getForward() * movement.z;
	offset += camTransform.getRight() * movement.x;
	offset += camTransform.getUp() * movement.y;

	camPos += offset * camSpd * delta;
	c->setPosition(camPos);
}
