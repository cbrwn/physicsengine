/* =================================
 *  PhysicsBody
 *  An object in the physics world which collides with other objects
 * ================================= */
#pragma once

#include <darray.h>
#include <matrix4.h>
#include <vector3.h>
#include <functional> // for std::function

#include <glm/ext.hpp>

struct Collider;
struct ColliderAABB;

#define MIN_LINEAR_THRESHOLD 0.1f
#define MIN_ROTATIONAL_THRESHOLD 0.1f

class PhysicsBody
{
public:
	PhysicsBody(Collider* collider = nullptr);
	~PhysicsBody();

	void update(float delta);

	// collider getter/setter
	void setCollider(Collider* c);
	inline Collider* getCollider() { return m_collider; }

	// set the entire transform of the body
	void setTransform(Matrix4 const& m);
	// set just the position part of the transform
	void setPosition(Vector3 const& v);
	// set just the rotation part of the transform
	void setRotation(Vector3 const& v);

	// adds a rotation to the body
	void rotate(Vector3 const& v);

	// grab the position out of the transform matrix
	Vector3 getPosition();

	// setters for velocities
	void setVelocity(Vector3 const& v) { m_velocity = v; }
	void setAngularVelocity(Vector3 const& v) { m_angularVelocity = v; }

	// getters for velocities
	inline Vector3 getVelocity() { return m_velocity; }
	inline Vector3 getAngularVelocity() { return m_angularVelocity; }

	// adds some velocity
	void addForce(Vector3 force, Vector3 pos);

	// set physical properties of the body
	// bounce and friction aren't implemented yet!
	void setDrag(float d) { m_drag = d; }
	void setMass(float m) { m_mass = m; }
	void setBounce(float b) { m_bounce = b; }
	void setFriction(float f) { m_friction = f; }

	// get physical properties of the body
	inline float getDrag() { return m_drag; }
	inline float getMass() { return m_mass; }
	inline float getBounce() { return m_bounce; }
	inline float getFriction() { return m_friction; }
	inline float getMomentOfInertia() { return m_momentOfInertia; }

	// change whether or not this body is affected by gravity
	void setUseGravity(bool g) { m_useGravity = g; }

	// getter/setter for enabled - whether or not physics happens on this body
	bool isEnabled() { return m_enabled; }
	void setEnabled(bool b) { m_enabled = b; }

	// getter/setter for staticness - whether or not the body can move
	bool isStatic() { return m_static; }
	void setStatic(bool s) { m_static = s; }

	// getter/setter for zone - zones allow objects to intersect them and
	// simply detect these objects, could be called a 'sensor'
	bool isZone() { return m_zone; }
	void setZone(bool z) { m_zone = z; }

	// set whether or not debug information is shown
	void setDebug(bool d) { m_debug = d; }

	// transforms a point by just the rotation of the matrix
	Vector3 rotatePoint(Vector3 const& pt);
	// tramsforms a point by the whole transform matrix
	Vector3 transformPoint(Vector3 const& pt);

	// get just the rotation portion of the transform matrix
	Matrix4 getRotationMatrix();
	// get the whole transform matrix
	Matrix4 getTransformMatrix();

	// grab the extents of a box which fits around the whole body,
	// used in broad phase collision detection
	Vector3	getBroadExtents() { return m_broadExtents; }
	// updates those extents to fit around the body
	void updateBroadExtents();

	DArray<PhysicsBody*>& getCollidingBodies() { return m_colliding; }

	// specific broad phase collision functions
	static bool AABBvsAABB(Collider* c1, Collider* c2);
	static bool AABBvsSphere(Collider* c1, Collider* c2);
	static bool SpherevsSphere(Collider* c1, Collider* c2);

	// general broad phase collision detection
	bool isCollidingBroad(Collider* other);
	// narrow phase collision detection
	bool isCollidingSAT(Collider* other, float& penOut, Vector3& axisOut, Vector3& pointOut);

	bool rayTestBroad(Vector3 const& start, Vector3 const& dir, float* outDist);

	void setCollideCallback(std::function<void(PhysicsBody*)> func) 
	{ m_collideCallback = func; }

	// stops the body from sleeping
	void wakeUp();

	glm::quat m_rotation;

private:
	Collider* m_collider;

	Vector3 m_velocity;
	Vector3 m_angularVelocity;

	float m_drag;
	float m_angularDrag;

	Matrix4 m_transform;

	// store broad extents so we don't have to calculate them each time we
	// test collision
	Vector3 m_broadExtents;

	// physical properties
	float m_mass;
	float m_bounce;
	float m_friction;
	float m_momentOfInertia;

	// list of currently colliding bodies
	DArray<PhysicsBody*> m_colliding;

	bool m_debug;

	// whether or not this body is a zone/sensor
	bool m_zone;
	// whether or not this body can move
	bool m_static;

	// sleeping variables
	bool m_asleep;
	// amount of time the body hasn't moved
	float m_stillTime;
	// vector to keep track of its last position so we know if we can put it
	// to sleep
	Vector3 m_stillPos;

	bool m_enabled;

	bool m_useGravity;
	float m_gravityStrength;

	std::function<void(PhysicsBody*)> m_collideCallback;

	// checks collision against every body in the world and resolves it
	void checkCollision();

	// resolves collision by pushing objects out of each other and applying
	// relevant forces
	void resolveCollision(Collider* other, float pen, Vector3 axis, Vector3 vertex);

};
