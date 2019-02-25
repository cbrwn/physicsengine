/* =================================
 *  Actor
 *  The base class for all entities in the game
 * ================================= */
#pragma once

#include <vector>
#include <glm/glm.hpp>

 // include these because all actors basically use it
#include "world.h"
#include "objectpool.h"

// the 'type' of actor - allows identifying classes derived from actor
enum ActorType
{
	ACTORTYPE_NORMAL = 0,
	ACTORTYPE_PHYSICS,

	ACTORTYPE_COUNT
};

// the 'class' of actor - the individual actor types
enum ActorClass
{
	ACTORCLASS_DEFAULT = 0,
	ACTORCLASS_PLAYER,
	ACTORCLASS_TANK,

	ACTORCLASS_COUNT
};

class Actor
{
public:
	Actor(glm::vec3 pos);
	virtual ~Actor();

	virtual void update(float delta) = 0;
	virtual void draw() = 0;

	// heirarchy stuff
	void setParent(Actor* p);
	void addChild(Actor* c);

	World* getWorld();
	// virtual so we can override it and do something when added to a world
	virtual void setWorld(World* w);

	ActorType  getType() { return m_type; }
	ActorClass getClass() { return m_class; }

	glm::vec3 getPosition();
	// sets the actor's local position - position relative to its parent
	void setLocalPosition(glm::vec3 const& p);
	// sets the global position - its absolute position in world space
	void setGlobalPosition(glm::vec3 const& p);

	// updates its transform matrices and also its children's matrices
	void updateTransform();

	glm::mat4& getLocalTransform() { return m_localTransform; }
	glm::mat4& getGlobalTransform() { return m_globalTransform; }

	virtual void setEnabled(bool e);
	bool isEnabled() { return m_enabled; }

	// function used to re-activate disabled actors in an object pool
	virtual void reset();

protected:
	glm::mat4 m_localTransform;
	glm::mat4	m_globalTransform;

	ActorType  m_type;
	ActorClass m_class;

	bool m_enabled;

	World* m_world;
	Actor* m_parent;
	std::vector<Actor*> m_children;
};
