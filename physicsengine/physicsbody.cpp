#include "physicsbody.h"

#include <vector>
#include <cassert>
#include <Gizmos.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include "shapes.h"
#include "collider.h"
#include "collideraabb.h"
#include "collidersphere.h"
#include "physicsmanager.h"

PhysicsBody::PhysicsBody(Collider* collider)
	: m_collider(collider)
{
	if (collider)
		collider->body = this;

	m_collideCallback = nullptr;

	// default velocities
	m_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	m_angularVelocity = glm::vec3(0.0f, 0.0f, 0.0f);

	// default values
	m_zone = false;
	m_debug = false;
	m_asleep = false;
	m_static = false;
	m_enabled = true;
	m_useGravity = true;
	m_gravityStrength = 1.0f;

	// sleeping values
	m_stillTime = 0.0f;
	m_stillPos = glm::vec3();

	m_transform = glm::mat4(1.0f);

	// physical values
	m_drag = 0.0f;
	m_angularDrag = 1.0f;
	m_mass = 1.0f;
	m_bounce = 0.0f;
	m_momentOfInertia = 1.0f;

	m_friction = 0.05f;
    m_frictionMode = FRICTION_AVG;
}

PhysicsBody::~PhysicsBody()
{
	delete m_collider;
}

void PhysicsBody::update(float delta)
{
	if (!m_enabled)
		return;
	if (!m_collider)
		return;

	// sleeping bodies don't check their own collision
	// instead, other bodies check if they collided with sleeping bodies
	// and wake them up if so
	if (m_asleep)
		return;

	if (!m_static)
	{
		// apply gravity
		if (m_useGravity)
			m_velocity.y -= m_gravityStrength * PhysicsManager::gravity * delta;

		// drags
		m_velocity -= (m_velocity * m_drag) * delta;
		m_angularVelocity -= (m_angularVelocity * m_angularDrag) * delta;

		// apply minimum velocity thresholds
		if (glm::length2(m_velocity) <
			MIN_LINEAR_THRESHOLD*MIN_LINEAR_THRESHOLD)
			m_velocity = { 0,0,0 };
		if (glm::length2(m_angularVelocity) <
			MIN_ROTATIONAL_THRESHOLD*MIN_ROTATIONAL_THRESHOLD)
			m_angularVelocity = { 0,0, 0 };

		// apply velocities
		glm::vec3 position = m_transform[3];
		glm::vec3 newPos = position + (m_velocity * delta);
		m_transform[3] = glm::vec4(newPos, m_transform[3].w);

		glm::vec3 av = m_angularVelocity * delta;
		this->rotate(av);

		// check for sleeping
		// by checking how much it's moved
		glm::vec3 dif = m_stillPos - newPos;
		if (glm::length2(dif) < 0.1f &&
			glm::length2(m_velocity) < 0.1f &&
			glm::length2(m_angularVelocity) < 0.1f)
		{
			// body hasn't moved much, keep a timer on that
			m_stillTime += delta;
			// if we haven't moved for a long time, it's safe to put this
			// body to sleep!
			if (m_stillTime >= 3.0f)
				m_asleep = true;
		}
		else
		{
			// body moved enough to keep it awake
			// reset our timer
			m_stillTime = 0.0f;
			// and update our last position
			m_stillPos = newPos;
			m_asleep = false;
		}
	}

	updateBroadExtents();

	// draw debug information if needed
	if (m_debug)
	{
		// draw collider information
		m_collider->drawPoints();
		m_collider->drawNormals();

		// draw its broad phase collision box
		aie::Gizmos::addAABB(m_transform[3],
			getBroadExtents(), glm::vec4(1, 0, 0, 1));
	}

	// static objects don't need to check their collision, as dynamic objects
	// check their collision against static objects
	// BUT zones should check regardless, since they need to keep track of
	// bodies inside them
	if (m_static && !m_zone)
		return;

	checkCollision();
}

void PhysicsBody::setCollider(Collider* c)
{
	m_collider = c;
	// make sure the collider knows we own it too
	c->body = this;
}

void PhysicsBody::setTransform(glm::mat4 const& m)
{
	m_transform = m;
}

void PhysicsBody::setPosition(glm::vec3 const& v)
{
	m_transform[3] = glm::vec4(v, m_transform[3].w);
	updateBroadExtents();
}

void PhysicsBody::setRotation(glm::vec3 const& v)
{
	glm::mat4 xRot = glm::mat4(1.0f);
	glm::mat4 yRot = glm::mat4(1.0f);
	glm::mat4 zRot = glm::mat4(1.0f);

	xRot = glm::rotate(xRot, v.x, glm::vec3(1, 0, 0));
	yRot = glm::rotate(yRot, v.y, glm::vec3(0, 1, 0));
	zRot = glm::rotate(zRot, v.z, glm::vec3(0, 0, 1));

	glm::mat4 rotationMatrix = xRot * yRot * zRot;
	rotationMatrix[3] = m_transform[3];

	m_transform = rotationMatrix;
	updateBroadExtents();
}

void PhysicsBody::rotate(glm::vec3 const& v)
{
	// make rotation matrix
	glm::mat4 xRot = glm::mat4(1.0f);
	glm::mat4 yRot = glm::mat4(1.0f);
	glm::mat4 zRot = glm::mat4(1.0f);

	xRot = glm::rotate(xRot, v.x, glm::vec3(1, 0, 0));
	yRot = glm::rotate(yRot, v.y, glm::vec3(0, 1, 0));
	zRot = glm::rotate(zRot, v.z, glm::vec3(0, 0, 1));

	glm::mat4 rotationMatrix = zRot * yRot * xRot;

	m_transform = m_transform * rotationMatrix;
}

glm::vec3 PhysicsBody::getPosition()
{
	return m_transform[3];
}

void PhysicsBody::addForce(glm::vec3 force, glm::vec3 pos)
{
	m_velocity += force;

    // abandon all hope for rotation

	glm::vec3 deltaAng = glm::cross(force, pos);
	//deltaAng /= m_momentOfInertia;

	deltaAng.x = 0.0f;
	deltaAng.y = 0.0f;
	deltaAng.z = 0.0f;
    
	if (glm::length2(deltaAng) > 0.001f)
		m_angularVelocity -= deltaAng;
	if (m_debug && glm::length2(pos) > 0.0f)
	{
		glm::vec3 p = pos + getPosition();
		aie::Gizmos::addLine(p,
			p + glm::normalize(force) * 5.0f,
			glm::vec4(0, 1, 0, 1));
		drawSphere(p, 0.2f, glm::vec4(0, 1, 0, 1));
	}
}

glm::vec3 PhysicsBody::transformPoint(glm::vec3 const& pt)
{
	// make a position matrix using this point
	glm::mat4 pointMatrix = glm::mat4(1.0f);
	pointMatrix[3] = glm::vec4(pt, pointMatrix[3].w);

	// transform it with our current transform
	glm::mat4 resultMatrix = getTransformMatrix() * pointMatrix;

	// we just want the position, not the whole matrix
	return resultMatrix[3];
}

glm::vec3 PhysicsBody::rotatePoint(glm::vec3 const & pt)
{
	// make a position matrix using this point
	glm::mat4 pointMatrix = glm::mat4(1.0f);
	pointMatrix[3] = glm::vec4(pt, pointMatrix[3].w);

	// transform it with JUST our rotation matrix
	glm::mat4 resultMatrix = getRotationMatrix() * pointMatrix;

	// we just want the resulting position
	return resultMatrix[3];
}

glm::mat4 PhysicsBody::getTransformMatrix()
{
	return m_transform;
}

// gets just the rotation matrix by setting the transform matrix's position
// to 0,0,0
glm::mat4 PhysicsBody::getRotationMatrix()
{
	glm::mat4 temp = m_transform;
	temp[3] = { 0,0,0,1 };
	return temp;
}

bool PhysicsBody::AABBvsAABB(Collider* c1, Collider* c2)
{
	// make sure colliders are both AABBs
	assert(c1->type == COLLIDER_AABB && c2->type == COLLIDER_AABB);

	// cast to AABB colliders so we can get the extents
	ColliderAABB* a1 = (ColliderAABB*)c1;
	ColliderAABB* a2 = (ColliderAABB*)c2;

	// get the minimum/maximum point of c1/a1
	glm::vec3 min1 = a1->body->getPosition() - a1->extents;
	glm::vec3 max1 = a1->body->getPosition() + a1->extents;

	// get the minimum/maximum point of c2/a2
	glm::vec3 min2 = a2->body->getPosition() - a2->extents;
	glm::vec3 max2 = a2->body->getPosition() + a2->extents;

	auto lt = [](glm::vec3 const& v1, glm::vec3 const& v2) {
		return v1.x < v2.x && v1.y < v2.y && v1.z < v2.z;
	};
	auto gt = [](glm::vec3 const& v1, glm::vec3 const& v2) {
		return v1.x > v2.x && v1.y > v2.y && v1.z > v2.z;
	};

	// check if any part of them is NOT colliding
	if (gt(min1,  max2) || lt(max1, min2) ||
		gt(min2, max1) || lt(max2, min1))
		return false;

	// no side is not colliding, so we must be colliding
	return true;
}

bool PhysicsBody::AABBvsSphere(Collider* _aabb, Collider* _sphere)
{
	ColliderSphere* sphere = (ColliderSphere*)_sphere;
	ColliderAABB* aabb = (ColliderAABB*)_aabb;

	// make sure BOTH colliders aren't null
	assert(sphere && aabb);
	// double check sphere is actually a sphere
	assert(sphere->type == COLLIDER_SPHERE);
	// double check aabb is actually an aabb
	assert(aabb->type == COLLIDER_AABB);

	// grab their associated bodies so we can get their position more easily
	PhysicsBody* sphereBody = sphere->body;
	PhysicsBody* aabbBody = aabb->body;

	// grab the actual center of the sphere collider
	// (its attached object's position + its center offset)
	glm::vec3 sphereCenter = sphereBody->getPosition() + sphere->center;

	// grab the minimum extents
	glm::vec3 aabbMin = aabbBody->getPosition() - aabb->extents;
	// and the maximum extents
	glm::vec3 aabbMax = aabbBody->getPosition() + aabb->extents;

	auto clamp = [](glm::vec3 v, glm::vec3 const& n, glm::vec3 const& x) {
		if (v.x < n.x)
			v.x = n.x;
		if (v.y < n.y)
			v.y = n.y;
		if (v.z < n.z)
			v.z = n.z;

		if (v.x > x.x)
			v.x = x.x;
		if (v.y > x.y)
			v.y = x.y;
		if (v.z > x.z)
			v.z = x.z;
		return v;
	};

	// clamp the center of the sphere to the box, making it so this new point
	// will be within the radius of the sphere if they're colliding
	glm::vec3 clampedCenter = clamp(sphereCenter, aabbMin, aabbMax);

	// get the squared distance to this new point
	float distSquared = glm::length2(clampedCenter - sphereCenter);

	// test that against the squared radius of the sphere
	return distSquared <= sphere->radius*sphere->radius;
}

bool PhysicsBody::SpherevsSphere(Collider* c1, Collider* c2)
{
	// make sure the colliders are both spheres
	assert(c1->type == COLLIDER_SPHERE && c2->type == COLLIDER_SPHERE);

	// cast to spheres for easy access to their center
	ColliderSphere* s1 = (ColliderSphere*)c1;
	ColliderSphere* s2 = (ColliderSphere*)c2;

	// grab the world position of each sphere
	glm::vec3 center1 = s1->center + s1->body->getPosition();
	glm::vec3 center2 = s2->center + s2->body->getPosition();

	// get the total radius
	float rad = s1->radius + s2->radius;

	// test the distance between their centers against their combined radii
	// if it's less, then they're colliding!
	return glm::length2(center1 - center2) <= rad * rad;
}

// makes the broad phase collision box by checking each point of the collider
// and keeping track of the lowest and highest points on each axis
void PhysicsBody::updateBroadExtents()
{
	// we don't want to update our broad box if we have no collider
	if (!m_collider)
		return;

	// update our broad bounding box
	glm::vec3 min(INFINITY, INFINITY, INFINITY);
	glm::vec3 max(-INFINITY, -INFINITY, -INFINITY);
	// go through all points and get the min/max positions
	for (int i = 0; i < m_collider->points.size(); ++i)
	{
		glm::vec3 p = transformPoint(m_collider->points[i]);

		// check if any coordinate is smaller
		if (p.x < min.x)
			min.x = p.x;
		if (p.y < min.y)
			min.y = p.y;
		if (p.z < min.z)
			min.z = p.z;

		// and check if any is larger
		if (p.x > max.x)
			max.x = p.x;
		if (p.y > max.y)
			max.y = p.y;
		if (p.z > max.z)
			max.z = p.z;
	}

	// get the difference between these vectors,
	// half of that will be the extents of an AABB collider
	m_broadExtents = (max - min)*0.5f;
}

// wakes up the body so it starts checking collisions again
void PhysicsBody::wakeUp()
{
	if (!m_asleep)
		return;
	m_asleep = false;
	m_stillTime = 0.0f;
	m_stillPos = getPosition();
}

void PhysicsBody::checkCollision()
{
	// turn our body's broad phase extents into a cube that the Octree can use
	glm::vec3 pos = getPosition();
	glm::vec3 extents = getBroadExtents();
	OctCube cube;
	cube.minX = pos.x - extents.x;
	cube.minY = pos.y - extents.y;
	cube.minZ = pos.z - extents.z;

	cube.maxX = pos.x + extents.x;
	cube.maxY = pos.y + extents.y;
	cube.maxZ = pos.z + extents.z;

	// grab a reference to all the bodies in range
	PhysicsManager* p = PhysicsManager::getInstance();
	std::vector<PhysicsBody*> bodies = p->getTree()->getInRange(cube);

	m_colliding.clear();

	for (int i = 0; i < bodies.size(); ++i)
	{
		PhysicsBody* body = bodies[i];

		// there shouldn't be null bodies in here
		assert(body);

		// make sure we're not colliding with ourself
		if (body == this)
			continue;
		if (!body->isEnabled())
			continue;

		// get the other body's collider
		Collider* col = body->getCollider();

		// do broad phase check
		if (!isCollidingBroad(col))
			continue;

		// broad phase collision said we're colliding!

		// grab the actual collider of the other object
		Collider* acol = body->getCollider();

		// perform SAT collision and resolve it if it happened
		glm::vec3 axis = { 0.0f, 0.0f, 0.0f };
		glm::vec3 point = { 0.0f, 0.0f, 0.0f };
		float penetration;
		if (isCollidingSAT(acol, penetration, axis, point))
		{
			if (!m_zone && !body->isZone())
				resolveCollision(acol, penetration, axis, point);
			if (m_collideCallback)
				m_collideCallback(body);

			m_colliding.push_back(body);

			if (m_debug)
				drawSphere(point, 0.05f, glm::vec4(1, 0, 0, 1));
		}
	}
}

void PhysicsBody::resolveCollision(Collider* other, float pen, glm::vec3 axis, glm::vec3 vertex)
{
	// slightly shorter reference to the other object's body
	PhysicsBody* otherBody = other->body;
	otherBody->wakeUp();

	// draw collision axis 
	if (m_debug)
	{
		aie::Gizmos::addLine(m_transform[3],
			glm::vec3(m_transform[3]) + axis * 5.0f,
			glm::vec4(1, 0, 0, 1));
	}

	// spinny stuff
	if (false) {

		//glm::vec3 perp = glm::cross(axis, glm::vec3(0, 1, 0));

		//float r1 = (vertex - getPosition()).dot(-1.0 * perp);
		//float r2 = (vertex - other->body->getPosition()).dot(perp);

		//glm::vec3 v1;
		//v1.x = m_velocity.dot(axis) - (r1 * m_angularVelocity.x);
		//v1.y = m_velocity.dot(axis) - (r1 * m_angularVelocity.y);
		//v1.z = m_velocity.dot(axis) - (r1 * m_angularVelocity.z);

		//glm::vec3 v2;
		//v2.x = other->body->getVelocity().dot(axis) - (r2 * other->body->getAngularVelocity().x);
		//v2.y = other->body->getVelocity().dot(axis) - (r2 * other->body->getAngularVelocity().y);
		//v2.z = other->body->getVelocity().dot(axis) - (r2 * other->body->getAngularVelocity().z);

		//if (v1.dot(v2) > 0.0f) {
		//	float mass1 = 1.0f / (1.0f / getMass() + (r1*r1) / m_momentOfInertia);
		//	float mass2 = 1.0f / (1.0f / other->body->getMass() + (r2*r2) / other->body->getMomentOfInertia());

		//	glm::vec3 force = mass1 * mass2 / (mass1 + mass2)*(v1*v2)*axis;

		//	addForce(force*-1.0f, vertex - getPosition());
		//	other->body->addForce(force, vertex - other->body->getPosition());
		//}
	}

	// get the total mass so we can compare them
	float totalMass = getMass() + otherBody->getMass();

	// get each body's percentage of the total mass
	float massA = getMass() / totalMass;
	float massB = otherBody->getMass() / totalMass;

	// handle "bumping" - moving out of the way when penetration 
	//		occurs
	float bumpA = -massA; // percentage of the way A should move
	float bumpB = massB; // percentage of the way B should move

	// we don't want to bump static objects
	// instead, we want to force the dynamic object to
	//		stop intersecting
	if (otherBody->isStatic())
	{
		bumpA = -1.0f; // so we give it 100% of the movement
		bumpB = 0.0f;
	}

	// grab the actual distances we'll move each body
	glm::vec3 moveA = -1.0f * (pen * bumpA) * axis;
	glm::vec3 moveB = -1.0f * (pen * bumpB) * axis;

	// move the bodies
	setPosition(getPosition() + moveA);
	otherBody->setPosition(otherBody->getPosition() + moveB);

	// get the absolute velocities for handling impulse and stuff
	glm::vec3 absVelA;
	absVelA.x = fabsf(getVelocity().x);
	absVelA.y = fabsf(getVelocity().y);
	absVelA.z = fabsf(getVelocity().z);

	glm::vec3 absVelB;
	absVelB.x = fabsf(otherBody->getVelocity().x);
	absVelB.y = fabsf(otherBody->getVelocity().y);
	absVelB.z = fabsf(otherBody->getVelocity().z);

	// the amount of force to totally cancel out velocity in the
	//		direction we collided
	glm::vec3 pushBackA = axis * absVelA;
	glm::vec3 pushBackB = axis * absVelB;

	// the percentage of the velocity to give to the other object
	float passOnA = massA;
	float passOnB = massB;

	// static objects don't want to give us any velocity
	//		because they're greedy (and always have no velocity)
	// so we'll make sure A tries to give ALL of its velocity to
	//		B, which won't use it but A will lose that velocity
	if (otherBody->isStatic())
	{
		passOnA = 1.0f;
		passOnB = 0.0f;
	}

	glm::vec3 forceA, forceB;

	forceA = (pushBackA * passOnA) - (pushBackB * passOnB);
	forceB = (pushBackB * passOnB) - (pushBackA * passOnA);

	addForce(forceA, vertex - getPosition());
	otherBody->addForce(forceB, vertex - otherBody->getPosition());

    // friction
    float friction = 0.0f;
    switch(m_frictionMode) {
        case FRICTION_MIN:
            friction = fminf(getFriction(), otherBody->getFriction());
            break;
        case FRICTION_MAX:
            friction = fmaxf(getFriction(), otherBody->getFriction());
            break;
        case FRICTION_AVG:
            friction = (getFriction() + otherBody->getFriction())/2.0f;
            break;
        default:
            printf("unhandled friction mode %i\n", m_frictionMode);
    }
	m_velocity -= m_velocity * friction;

	// update both bodies' transforms
	updateBroadExtents();
	otherBody->updateBroadExtents();
}

bool PhysicsBody::isCollidingBroad(Collider* other)
{
	if (!other)
		return false;

	// grab this body's collider
	Collider* thisCol = m_collider;

	// use the broad collider if it can be used
	ColliderAABB broad(getBroadExtents());
	broad.body = this;
	if (m_collider->points.size() > 0)
		thisCol = &broad;

	// and get the broad for the other one too
	ColliderAABB otherBroad(other->body->getBroadExtents());
	otherBroad.body = other->body;
	if (other->points.size() > 0)
		other = &otherBroad;

	// broad collision should only be done with AABBs or spheres
	// so let's check for that
	assert(thisCol->type == COLLIDER_AABB || thisCol->type == COLLIDER_SPHERE);
	assert(other->type == COLLIDER_AABB || other->type == COLLIDER_SPHERE);

	// make flags of whether or not the colliders are spheres
	bool thisSphere = (thisCol->type == COLLIDER_SPHERE);
	bool otherSphere = (other->type == COLLIDER_SPHERE);

	if (thisSphere)
	{
		// this collider is a sphere

		// check collision if the other is a sphere
		if (otherSphere && PhysicsBody::SpherevsSphere(thisCol, other))
			return true;
		// check collision if the other is an AABB
		if (!otherSphere && PhysicsBody::AABBvsSphere(thisCol, other))
			return true;
	}
	else
	{
		// this collider is an AABB

		// check collision if the other is a sphere
		if (otherSphere && PhysicsBody::AABBvsSphere(thisCol, other))
			return true;
		// check collision if the other is an AABB
		if (!otherSphere && PhysicsBody::AABBvsAABB(thisCol, other))
			return true;
	}

	// no collision!
	return false;
}

bool PhysicsBody::isCollidingSAT(Collider* other, float& penOut, glm::vec3& axisOut, glm::vec3& pointOut)
{
	// get a slightly shorter reference to our collider
	Collider* thisCol = m_collider;

	// grab a list of all the objects' normals
	std::vector<glm::vec3> axes;
	for (int i = 0; i < thisCol->normals.size(); ++i)
		axes.push_back(rotatePoint(thisCol->normals[i]));
	for (int i = 0; i < other->normals.size(); ++i)
		axes.push_back(other->body->rotatePoint(other->normals[i]));

	// transform the colliders' points to world position
	std::vector<glm::vec3> thisPoints;
	for (int i = 0; i < thisCol->points.size(); ++i)
	{
		// transform the point based on this body's transform matrix
		glm::vec3 pt = transformPoint(thisCol->points[i]);
		thisPoints.push_back(pt);
	}

	std::vector<glm::vec3> otherPoints;
	for (int i = 0; i < other->points.size(); ++i)
	{
		// transform the point based on its body's transform matrix
		glm::vec3 pt = other->body->transformPoint(other->points[i]);
		otherPoints.push_back(pt);
	}

	std::vector<glm::vec3> penPoints;
	std::vector<float> penetrations;
	// start checking for overlaps!
	for (int i = 0; i < axes.size(); ++i)
	{
		glm::vec3 axis = axes[i];

		float mina = INFINITY;
		float maxa = -INFINITY;
		float minb = INFINITY;
		float maxb = -INFINITY;

		int minIndex = -1;
		int maxIndex = -1;
		// project each vertex of this shape onto the current axis
		for (int j = 0; j < thisPoints.size(); ++j)
		{
			glm::vec3 pt = thisPoints[j];
			float dot = glm::dot(axis, pt);

			// keep track of it if it's lower than min
			if (dot < mina) {
				mina = dot;
				minIndex = j;
			}
			// or higher than max
			if (dot > maxa) {
				maxa = dot;
				maxIndex = j;
			}
		}
		// project each vertex of the other shape onto the current axis
		for (int j = 0; j < otherPoints.size(); ++j)
		{
			glm::vec3 pt = otherPoints[j];
			float dot = glm::dot(axis, pt);

			// keep track of it if it's lower than min1.0f);
			if (dot < minb)
				minb = dot;
			// or higher than max
			if (dot > maxb)
				maxb = dot;
		}

		// get the amount that these objects overlap on this axis
		//float overlap = fminf(maxa - minb, maxb - mina);
		float overlap = maxa - minb;
		int pointIndex = maxIndex;
		if (maxb - mina < overlap) {
			overlap = maxb - mina;
			pointIndex = minIndex;
		}

		// they're not colliding if there's no overlap on an axis
		if (overlap <= 0.0f)
			return false;

		// keep track of this amount of overlap to find the axis of collision
		penetrations.push_back(overlap);

		if (pointIndex < 0)
			return false;

		// and keep track of the point most responsible for this overlap
		penPoints.push_back(thisPoints[pointIndex]);
	}

	// get the lowest penetration's index
	int lowest = 0;
	for (int i = 0; i < penetrations.size(); ++i)
		if (penetrations[i] < penetrations[lowest])
			lowest = i;

	// output this penetration value
	penOut = penetrations[lowest];
	// and that penetration's most responsible point
	pointOut = penPoints[lowest];

	// use the objects' relative positions to figure out if the collision
	// is happening in the same direction as the axis or not
	glm::vec3 p1 = getPosition();
	glm::vec3 p2 = other->body->getPosition();
	glm::vec3 dif = p1 - p2;
	// output a flipped version of the axis if it's the wrong direction
	if (glm::dot(dif, axes[lowest]) < 0)
		axisOut = axes[lowest] * -1.0f;
	else
		axisOut = axes[lowest];

	// we did collide! yay!
	return true;
}

bool PhysicsBody::rayTestBroad(glm::vec3 const& start, glm::vec3 const& dir,
	float* outDist)
{
	glm::mat4 m = getTransformMatrix();
	glm::vec3 pos = m[3];

	glm::vec3 startDif = pos - start;

	// these look wrong, but tMin should be the FARTHEST near face
	float tMin = -INFINITY;
	// and tMax should be the CLOSEST far face
	float tMax = INFINITY;

	// test each of the three axes
	for (int i = 0; i < 3; ++i)
	{
		// easily grab the right/up/forward vectors by casting the column of
		// the matrix to a glm::vec3, dropping the w value
		glm::vec3 axis = (glm::vec3)m[i];

		// dot this axis direction with the start difference
		float e = glm::dot(axis, startDif);
		// and the ray direction
		float f = glm::dot(axis, dir);

		// (badly) make sure we don't divide by 0
		if (fabsf(f) < 0.001f)
			continue;

		// subtract e from the distance from the middle on this axis to get
		// the near face distance (assuming the AABB is in front of the ray)
		float t1 = (e - m_broadExtents[i]) / f;
		// and the far face by adding this axis' extent
		float t2 = (e + m_broadExtents[i]) / f;

		// t2 SHOULD be the largest, but might not if we intersected
		// behind the start of the ray
		if (t1 > t2)
		{
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		// keep track of the closest far face
		if (t2 < tMax)
			tMax = t2;
		// and the farthest near face
		if (t1 > tMin)
			tMin = t1;

		// if the far face is ever closer than the near face, there's no way
		// it could be intersecting
		if (tMax < tMin)
			return false;
	}

	// tMin will be the smallest near face to the ray
	// which I think can be negative, meaning it's behind the ray
	*outDist = tMin;

	return true;
}
