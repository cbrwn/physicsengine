/* =================================
 *  Actor
 *  The base class for all entities in the game
 * ================================= */
#include "actor.h"

#include "world.h"

Actor::Actor(Vector3 pos)
{
	m_type  = ACTORTYPE_NORMAL;
	m_class = ACTORCLASS_DEFAULT;

	m_enabled = true;
	m_world   = nullptr;
	m_parent  = nullptr;

	setGlobalPosition(pos);
}

Actor::~Actor() { }

void Actor::setParent(Actor* p)
{
	m_parent = p;
}

void Actor::addChild(Actor* c)
{
	m_children.add(c);
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

void Actor::setGlobalPosition(Vector3 const& p)
{
	if (!m_parent)
	{
		m_localTransform.setPosition(p);
	}
	else
	{
		Vector3 parentPos = m_parent->getPosition();
		m_localTransform.setPosition(parentPos - p);
	}
	updateTransform();
}

void Actor::setLocalPosition(Vector3 const& p)
{
	m_localTransform.setPosition(p);
	updateTransform();
}

Vector3 Actor::getPosition()
{
	return m_globalTransform.getPosition();
}

void Actor::updateTransform()
{
	if (m_parent)
		m_globalTransform = m_parent->getGlobalTransform() * m_localTransform;
	else
		m_globalTransform = m_localTransform;

	for (int i = 0; i < m_children.getCount(); ++i)
		m_children[i]->updateTransform();
}

void Actor::setEnabled(bool e)
{
	m_enabled = e;
	for (int i = 0; i < m_children.getCount(); ++i)
		m_children[i]->setEnabled(e);
}

void Actor::reset()
{
	setEnabled(true);
}
