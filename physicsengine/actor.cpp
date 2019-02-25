/* =================================
 *  Actor
 *  The base class for all entities in the game
 * ================================= */
#include "actor.h"

#include "world.h"

Actor::Actor(glm::vec3 pos)
{
	m_type = ACTORTYPE_NORMAL;
	m_class = ACTORCLASS_DEFAULT;

	m_localTransform = glm::mat4(1.0f);
	m_globalTransform = glm::mat4(1.0f);

	m_enabled = true;
	m_world = nullptr;
	m_parent = nullptr;

	setGlobalPosition(pos);
}

Actor::~Actor() { }

void Actor::setParent(Actor* p)
{
	m_parent = p;
}

void Actor::addChild(Actor* c)
{
	m_children.push_back(c);
	c->setParent(this);
}

void Actor::setWorld(World* w)
{
	m_world = w;
}

World* Actor::getWorld()
{
	return m_world;
}

void Actor::setGlobalPosition(glm::vec3 const& p)
{
	if (!m_parent)
	{
		m_localTransform[3] = glm::vec4(p, m_localTransform[3].w);
	}
	else
	{
		glm::vec3 parentPos = m_parent->getPosition();
		m_localTransform[3] = glm::vec4(parentPos - p, m_localTransform[3].w);
	}
	updateTransform();
}

void Actor::setLocalPosition(glm::vec3 const& p)
{
	m_localTransform[3] = glm::vec4(p, m_localTransform[3].w);
	updateTransform();
}

glm::vec3 Actor::getPosition()
{
	return m_globalTransform[3];
}

void Actor::updateTransform()
{
	if (m_parent)
		m_globalTransform = m_parent->getGlobalTransform() * m_localTransform;
	else
		m_globalTransform = m_localTransform;

	for (int i = 0; i < m_children.size(); ++i)
		m_children[i]->updateTransform();
}

void Actor::setEnabled(bool e)
{
	m_enabled = e;
	for (int i = 0; i < m_children.size(); ++i)
		m_children[i]->setEnabled(e);
}

void Actor::reset()
{
	setEnabled(true);
}
