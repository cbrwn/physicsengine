/* =================================
 *  Player
 *  The UFO that the player controls during the game
 * ================================= */
#include "player.h"

#include <color.h>
#include <gmath.h>
#include <imgui.h>
#include <Input.h>


#include "util.h"
#include "world.h"
#include "camera.h"
#include "shapes.h"
#include "physics.h"
#include "healthorb.h"
#include "collideraabb.h"

Player::Player(Vector3 pos)
	: PhysicsActor(pos)
{
	m_class = ACTORCLASS_PLAYER;

	const float colSize = 1.5f;
	// set up collider - it's just an AABB despite the UFO itself being a 
	// sphere
	auto collider = new ColliderAABB(Vector3(colSize, 0.6f, colSize));
	m_body->setCollider(collider);
	m_body->setUseGravity(false);
	m_body->setMass(1.0f);

	m_health = 3;
	m_orbSpinTimer = 0.0f;

	m_deathCallback = nullptr;

	// default rotation of slightly pointing downwards
	m_cameraRotation = Vector3(-0.3f, 0, 0);
}

void Player::update(float delta)
{
	m_orbSpinTimer += delta;

	// make sure our body never goes to sleep
	m_body->wakeUp();
	PhysicsActor::update(delta);

	aie::Input* input = aie::Input::getInstance();
	// handle camera movement
	doMouseInput(input, delta);
	// handle actor movement
	doKeyboardInput(input, delta);

	// lock the camera to the player
	Camera* cam = m_world->getCamera();
	cam->setPosition(m_globalTransform.getPosition());
	cam->setRotation(m_cameraRotation);

	// simulate some sort of drag since it's not implemented in the physics
	m_body->setVelocity(
		m_body->getVelocity() - m_body->getVelocity() * 2.0f * delta);

	updateHealthOrbs();
}

void Player::draw()
{
	drawShipBody();
	drawShipTop();
}

void Player::setEnabled(bool e)
{
	PhysicsActor::setEnabled(e);
	if (!e)
		if (m_deathCallback)
			m_deathCallback();
}

void Player::addOrb(HealthOrb* orb)
{
	addChild(orb);
	m_orbs.add(orb);
}

void Player::doDamage()
{
	m_health--;

	if (m_health < 0)
		setEnabled(false);
	else
	{
		if (m_health > 0)
			m_orbs[m_orbs.getCount() - (m_health + 1)]->setEnabled(false);
	}
}

void Player::setDeathCallback(std::function<void()> func)
{
	m_deathCallback = func;
}

void Player::updateHealthOrbs()
{
	// get the angle between each orb
	float angBetween =(2 * PI) / (float)(m_health);

	// store the distance that orbs should be placed from the middle
	const float dist = 2.5f;

	// the angle to offset the orbs by
	// spins faster the lower health is
	float ang = m_orbSpinTimer / ((float)m_health/10.0f);

	for (int i = 0; i < m_orbs.getCount(); ++i)
	{
		Vector3 pos;

		// the orb is "squished" into a shield when health is 0,
		// so we want to keep it rooted in the center of the player
		bool squish = m_health == 0;
		if (!squish)
		{
			// otherwise, place the orb somewhere around the player
			pos.x = cosf(i * angBetween + ang) * dist;
			pos.z = sinf(i * angBetween + ang) * dist;
		}
		// here we can let the orb know if it should draw as a shield or not
		m_orbs[i]->setShield(squish);
		// and put it in its place
		m_orbs[i]->setLocalPosition(pos);
	}
}

void Player::drawShipBody()
{
	// the body is just a sphere squished on the y axis
	Matrix4 bodyScale;
	bodyScale.setScale(1, 0.3f, 1);

	// make the matrix to squish it by
	Matrix4 bodyTransform = m_globalTransform * bodyScale;

	Vector4 col = intToVector(0xf2f2f2ff);

	// draw the squishy sphere
	drawSphere(Vector3(), 2.0f, col, &bodyTransform,
		8, 8, true, Vector4(0.82f, 0.82f, 0.82f, 1));
}

void Player::drawShipTop()
{
	// the top is just a translucent sphere - the glass for the cockpit(?)
	Matrix4 topTransform;

	// move up a bit so it sticks out the top
	topTransform.setPosition(0, 0.3f, 0);
	// rotate it so its lines don't line up with the body's lines
	topTransform.setRotateY(0.35f);

	// apply the global transform matrix
	topTransform = m_globalTransform * topTransform;
	drawSphere(Vector3(), 0.9f, Vector4(0.8f, 0.8f, 0.8f, 0.7f),
		&topTransform);
}

void Player::doMouseInput(aie::Input* input, float delta)
{
	using namespace aie;

	// grab the mouse position
	int _mx, _my;
	input->getMouseXY(&_mx, &_my);
	// and stick it into a vector for ease of use
	Vector2 mouse((float)_mx, (float)_my);

	// reset the last mouse position on mouse press
	// so we start off using this to get the mouse distance
	if (input->wasMouseButtonPressed(INPUT_MOUSE_BUTTON_RIGHT))
		m_lastMouse = mouse;

	// mouse sensitivities
	const float hsense = 1.0f; // horizontal
	const float vsense = 1.0f; // vertical
	const float sens = 0.003f; // general sensitivity
	if (input->isMouseButtonDown(INPUT_MOUSE_BUTTON_RIGHT))
	{
		Vector2 dif = mouse - m_lastMouse;

		// rotate the body horizontally
		Vector3 toRotate(0, dif.x * hsense * -sens, 0);
		m_body->rotate(toRotate);

		// rotate the camera
		m_cameraRotation.y += toRotate.y;
		m_cameraRotation.x += dif.y * vsense * sens;

		// limit looking up/down so you can't spin all the way around
		const float upLimit = 1.2f;
		const float downLimit = -1.2f;
		if (m_cameraRotation.x > upLimit)
			m_cameraRotation.x = upLimit;
		if (m_cameraRotation.x < downLimit)
			m_cameraRotation.x = downLimit;

		m_lastMouse = mouse;
	}
}

void Player::doKeyboardInput(aie::Input* input, float delta)
{
	using namespace aie;

	// detect forward/backwards keypresses
	float forward = 0.0f;
	if (input->isKeyDown(INPUT_KEY_W))
		forward--;
	if (input->isKeyDown(INPUT_KEY_S))
		forward++;

	// detect left/right keypresses
	float sideways = 0.0f;
	if (input->isKeyDown(INPUT_KEY_D))
		sideways++;
	if (input->isKeyDown(INPUT_KEY_A))
		sideways--;

	// detect up/down keypressed
	float upways = 0.0f;
	if (input->isKeyDown(INPUT_KEY_Q))
		upways--;
	if (input->isKeyDown(INPUT_KEY_E))
		upways++;

	// get vectors to move by based on the inputs
	Vector3 front = m_localTransform.getForward() * forward;
	Vector3 side = m_localTransform.getRight() * sideways;
	Vector3 up = m_localTransform.getUp() * upways;

	Vector3 totalForce = front + side + up;
	m_body->addForce(totalForce * delta * 30.0f);
}
