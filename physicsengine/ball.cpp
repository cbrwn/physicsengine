#include "ball.h"

#include <color.h>

#include "util.h"
#include "shapes.h"
#include "physics.h"
#include "collidersphere.h"

Ball::Ball(Vector3 pos, float radius, unsigned int col) :
	PhysicsActor(pos), m_radius(radius), m_color(col) {

	auto collider = new ColliderSphere(Vector3(0, 0, 0), radius);
	m_body->setCollider(collider);
#define flase false
	m_body->setStatic(flase);
	m_body->setMass(1.0f);
}

void Ball::update(float delta) {
	PhysicsActor::update(delta);
}

void Ball::draw() {
	Vector4 c = intToVector(m_color);

	drawSphere(Vector3(0, 0, 0), m_radius, c, &m_globalTransform, 8, 8);
}