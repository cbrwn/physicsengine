#include "physicsbody.h"

#include <cassert>
#include <darray.h>
#include <Gizmos.h>

#include "util.h"
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
	m_velocity = Vector3(0, 0, 0);
	m_angularVelocity = Vector3(1, 0, 0);

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
	m_stillPos = Vector3();

	// physical values
	m_drag = 0.0f;
	m_mass = 1.0f;
	m_bounce = 0.0f;
	m_friction = 0.1f;
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
			m_velocity.y -= m_gravityStrength * 30.0f * delta;
		m_velocity -= (m_velocity * m_drag) * delta;

		// apply velocities
		Vector3 position = m_transform.getPosition();
		Vector3 newPos = position + (m_velocity * delta);
		m_transform.setPosition(newPos);

		Matrix4 xrot, yrot, zrot;
		Vector3 av = m_angularVelocity * delta;
		xrot.setRotateX(av.x);
		yrot.setRotateY(av.y);
		zrot.setRotateZ(av.z);
		Matrix4 rot = xrot * yrot * zrot;
		m_transform = m_transform * rot;

		//Vector3 rotation = m_transform.getAngles();
		//rotation += m_angularVelocity * delta;
		//m_transform.setRotate(rotation);

		// check for sleeping
		// by checking how much it's moved
		Vector3 dif = m_stillPos - newPos;
		if(dif.magnitudeSquared() < 0.1f)
		{
			// body hasn't moved much, keep a timer on that
			m_stillTime += delta;
			// if we haven't moved for a long time, it's safe to put this
			// body to sleep!
			if(m_stillTime >= 3.0f)
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
		aie::Gizmos::addAABB(toVec3(m_transform.getPosition()), 
			toVec3(getBroadExtents()), glm::vec4(1, 0, 0, 1));
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

void PhysicsBody::setTransform(Matrix4 const& m)
{
	m_transform = m;
}

void PhysicsBody::setPosition(Vector3 const& v)
{
	m_transform.setPosition(v);
	//m_position = v;
	updateBroadExtents();
}

void PhysicsBody::setRotation(Vector3 const& v)
{
	Matrix4 xRot, yRot, zRot;
	xRot.setRotateX(v.x);
	yRot.setRotateY(v.y);
	zRot.setRotateZ(v.z);

	Matrix4 rotationMatrix = xRot * yRot * zRot;
	rotationMatrix.setPosition(m_transform.getPosition());

	m_transform = rotationMatrix;
	updateBroadExtents();
}

void PhysicsBody::rotate(Vector3 const& v)
{
	// make rotation matrix
	Matrix4 xRot, yRot, zRot;
	xRot.setRotateX(v.x);
	yRot.setRotateY(v.y);
	zRot.setRotateZ(v.z);

	Matrix4 rotationMatrix = xRot * yRot * zRot;

	m_transform = m_transform * rotationMatrix;
}

Vector3 PhysicsBody::getPosition()
{
	return m_transform.getPosition();
}

void PhysicsBody::addForce(Vector3 force)
{
	wakeUp();
	m_velocity += force;// *m_mass;
}

Vector3 PhysicsBody::transformPoint(Vector3 const& pt)
{
	// make a position matrix using this point
	Matrix4 pointMatrix;
	pointMatrix.setPosition(pt);

	// transform it with our current transform
	Matrix4 resultMatrix = getTransformMatrix() * pointMatrix;

	// we just want the position, not the whole matrix
	return resultMatrix.getPosition();
}

Vector3 PhysicsBody::rotatePoint(Vector3 const & pt)
{
	// make a position matrix using this point
	Matrix4 pointMatrix;
	pointMatrix.setPosition(pt);

	// transform it with JUST our rotation matrix
	Matrix4 resultMatrix = getRotationMatrix() * pointMatrix;

	// we just want the resulting position
	return resultMatrix.getPosition();
}

Matrix4 PhysicsBody::getTransformMatrix()
{
	return m_transform;
}

// gets just the rotation matrix by setting the transform matrix's position
// to 0,0,0
Matrix4 PhysicsBody::getRotationMatrix()
{
	Matrix4 temp = m_transform;
	temp.setPosition(Vector3());
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
	Vector3 min1 = a1->body->getPosition() - a1->extents;
	Vector3 max1 = a1->body->getPosition() + a1->extents;

	// get the minimum/maximum point of c2/a2
	Vector3 min2 = a2->body->getPosition() - a2->extents;
	Vector3 max2 = a2->body->getPosition() + a2->extents;

	// check if any part of them is NOT colliding
	if (min1 > max2 || max1 < min2 ||
		min2 > max1 || max2 < min1)
		return false;

	// no side is not colliding, so we must be colliding
	return true;
}

bool PhysicsBody::AABBvsSphere(Collider* c1, Collider* c2)
{
	// parameters aren't strict with which order these should be passed in
	// let's try to figure it out
	ColliderSphere* sphere = nullptr;
	ColliderAABB* aabb = nullptr;

	// figure out the type of c1
	switch (c1->type)
	{
	case COLLIDER_AABB:
		aabb = (ColliderAABB*)c1;
		break;
	case COLLIDER_SPHERE:
		sphere = (ColliderSphere*)c1;
		break;
	default:
		// we don't want to continue if it's not an AABB or a sphere
		assert(false && "Invalid type passed into AABBvsSphere");
		return false;
	}

	// figure out the type of c2
	switch (c2->type)
	{
	case COLLIDER_AABB:
		aabb = (ColliderAABB*)c2;
		break;
	case COLLIDER_SPHERE:
		sphere = (ColliderSphere*)c2;
		break;
	default:
		// we don't want to continue if it's not an AABB or a sphere
		assert(false && "Invalid type passed into AABBvsSphere");
		return false;
	}

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
	Vector3 sphereCenter = sphereBody->getPosition() + sphere->center;

	// grab the minimum extents
	Vector3 aabbMin = aabbBody->getPosition() - aabb->extents;
	// and the maximum extents
	Vector3 aabbMax = aabbBody->getPosition() + aabb->extents;

	// clamp the center of the sphere to the box, making it so this new point
	// will be within the radius of the sphere if they're colliding
	Vector3 clampedCenter = sphereCenter;
	clampedCenter.clamp(aabbMin, aabbMax);

	// get the squared distance to this new point
	float distSquared = (clampedCenter - sphereCenter).magnitudeSquared();

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
	Vector3 center1 = s1->center + s1->body->getPosition();
	Vector3 center2 = s2->center + s2->body->getPosition();

	// get the total radius
	float rad = s1->radius + s2->radius;

	// test the distance between their centers against their combined radii
	// if it's less, then they're colliding!
	return center1.distanceToSquared(center2) <= rad * rad;
}

// makes the broad phase collision box by checking each point of the collider
// and keeping track of the lowest and highest points on each axis
void PhysicsBody::updateBroadExtents()
{
	// we don't want to update our broad box if we have no collider
	if (!m_collider)
		return;

	// update our broad bounding box
	Vector3 min(INFINITY, INFINITY, INFINITY);
	Vector3 max(-INFINITY, -INFINITY, -INFINITY);
	// go through all points and get the min/max positions
	for (int i = 0; i < m_collider->points.getCount(); ++i)
	{
		Vector3 p = transformPoint(m_collider->points[i]);

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
	Vector3 pos = getPosition();
	Vector3 extents = getBroadExtents();
	OctCube cube;
	cube.minX = pos.x - extents.x;
	cube.minY = pos.y - extents.y;
	cube.minZ = pos.z - extents.z;

	cube.maxX = pos.x + extents.x;
	cube.maxY = pos.y + extents.y;
	cube.maxZ = pos.z + extents.z;

	// grab a reference to all the bodies in range
	PhysicsManager* p = PhysicsManager::getInstance();
	DArray<PhysicsBody*> bodies = p->getTree()->getInRange(cube);

	m_colliding.clear();

	for (int i = 0; i < bodies.getCount(); ++i)
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
		Vector3 axis;
		float penetration;
		if (isCollidingSAT(acol, penetration, axis))
		{
			if (!m_zone && !body->isZone())
				resolveCollision(acol, penetration, axis);
			if (m_collideCallback)
				m_collideCallback(body);

			m_colliding.add(body);
		}
	}
}

void PhysicsBody::resolveCollision(Collider* other, float pen, Vector3 axis)
{
	// slightly shorter reference to the other object's body
	PhysicsBody* otherBody = other->body;
	otherBody->wakeUp();

	// draw collision axis 
	if (m_debug)
	{
		aie::Gizmos::addLine(toVec3(m_transform.getPosition()),
			toVec3(m_transform.getPosition()+ axis * 5.0f), 
			glm::vec4(1, 0, 0, 1));
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
	Vector3 moveA = -1.0f * (pen * bumpA) * axis;
	Vector3 moveB = -1.0f * (pen * bumpB) * axis;

	// move the bodies
	setPosition(getPosition() + moveA);
	otherBody->setPosition(otherBody->getPosition() + moveB);

	// get the absolute velocities for handling impulse and stuff
	Vector3 absVelA;
	absVelA.x = fabsf(getVelocity().x);
	absVelA.y = fabsf(getVelocity().y);
	absVelA.z = fabsf(getVelocity().z);

	Vector3 absVelB;
	absVelB.x = fabsf(otherBody->getVelocity().x);
	absVelB.y = fabsf(otherBody->getVelocity().y);
	absVelB.z = fabsf(otherBody->getVelocity().z);

	// the amount of force to totally cancel out velocity in the
	//		direction we collided
	Vector3 pushBackA = axis * absVelA;
	Vector3 pushBackB = axis * absVelB;

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

	// make new vectors to make calculations easier to read
	Vector3 newVelA = getVelocity();
	Vector3 newVelB = otherBody->getVelocity();

	// this is actually subtracting the velocity from itself,
	//		since pushBack is in the direction of the normal
	newVelA += pushBackA * passOnA;
	newVelB += pushBackB * passOnB;

	// give a percentage of the velocity to the other object
	newVelB -= pushBackA * passOnA;
	newVelA -= pushBackB * passOnB;

	// update our velocities
	setVelocity(newVelA);
	otherBody->setVelocity(newVelB);

	// temporary friction stuff
	const float friction = 0.1f;
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
	if (m_collider->points.getCount() > 0)
		thisCol = &broad;

	// and get the broad for the other one too
	ColliderAABB otherBroad(other->body->getBroadExtents());
	otherBroad.body = other->body;
	if (other->points.getCount() > 0)
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

bool PhysicsBody::isCollidingSAT(Collider* other, float& penOut, Vector3& axisOut)
{
	// get a slightly shorter reference to our collider
	Collider* thisCol = m_collider;

	// grab a list of all the objects' normals
	DArray<Vector3> axes;
	for (int i = 0; i < thisCol->normals.getCount(); ++i)
		axes.add(rotatePoint(thisCol->normals[i]));
	for (int i = 0; i < other->normals.getCount(); ++i)
		axes.add(other->body->rotatePoint(other->normals[i]));

	// transform the colliders' points to world position
	DArray<Vector3> thisPoints;
	for (int i = 0; i < thisCol->points.getCount(); ++i)
	{
		// transform the point based on this body's transform matrix
		Vector3 pt = transformPoint(thisCol->points[i]);
		thisPoints.add(pt);
	}

	DArray<Vector3> otherPoints;
	for (int i = 0; i < other->points.getCount(); ++i)
	{
		// transform the point based on its body's transform matrix
		Vector3 pt = other->body->transformPoint(other->points[i]);
		otherPoints.add(pt);
	}

	DArray<float> penetrations;
	// start checking for overlaps!
	for (int i = 0; i < axes.getCount(); ++i)
	{
		Vector3 axis = axes[i];

		float mina =  INFINITY;
		float maxa = -INFINITY;
		float minb =  INFINITY;
		float maxb = -INFINITY;

		// project each vertex of this shape onto the current axis
		for (int j = 0; j < thisPoints.getCount(); ++j)
		{
			Vector3 pt = thisPoints[j];
			float dot = axis.dot(pt);

			// keep track of it if it's lower than min
			if (dot < mina)
				mina = dot;
			// or higher than max
			if (dot > maxa)
				maxa = dot;
		}
		// project each vertex of the other shape onto the current axis
		for (int j = 0; j < otherPoints.getCount(); ++j)
		{
			Vector3 pt = otherPoints[j];
			float dot = axis.dot(pt);

			// keep track of it if it's lower than min
			if (dot < minb)
				minb = dot;
			// or higher than max
			if (dot > maxb)
				maxb = dot;
		}

		// get the amount that these objects overlap on this axis
		float overlap = fminf(maxa - minb, maxb - mina);

		// they're not colliding if there's no overlap on an axis
		if (overlap <= 0.0f)
			return false;

		// keep track of this amount of overlap to find the axis of collision
		penetrations.add(overlap);
	}

	// get the lowest penetration's index
	int lowest = 0;
	for (int i = 0; i < penetrations.getCount(); ++i)
		if (penetrations[i] < penetrations[lowest])
			lowest = i;

	// output this penetration value
	penOut = penetrations[lowest];

	// use the objects' relative positions to figure out if the collision
	// is happening in the same direction as the axis or not
	Vector3 p1 = getPosition();
	Vector3 p2 = other->body->getPosition();
	Vector3 dif = p1 - p2;
	// output a flipped version of the axis if it's the wrong direction
	if (dif.dot(axes[lowest]) < 0)
		axisOut = axes[lowest] * -1;
	else
		axisOut = axes[lowest];

	// we did collide! yay!
	return true;
}

bool PhysicsBody::rayTestBroad(Vector3 const& start, Vector3 const& dir, 
	float* outDist)
{
	Matrix4 m = getTransformMatrix();
	Vector3 pos = m.getPosition();

	Vector3 startDif = pos - start;

	// these look wrong, but tMin should be the FARTHEST near face
	float tMin = -INFINITY;
	// and tMax should be the CLOSEST far face
	float tMax = INFINITY;

	// test each of the three axes
	for (int i = 0; i < 3; ++i)
	{
		// easily grab the right/up/forward vectors by casting the column of
		// the matrix to a Vector3, dropping the w value
		Vector3 axis = (Vector3)m[i];

		// dot this axis direction with the start difference
		float e = axis.dot(startDif);
		// and the ray direction
		float f = axis.dot(dir);

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
